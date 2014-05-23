#include "tracker.h"
#include "condensation/condensation.h"
#include "condensation/data_types.h"
#include "condensation/model_parameters.h"

#include <QtGlobal>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
//#include <omp.h>

#include <QDebug>


/* The following are some utility routines. */

/* This is the worst random number routine in the known universe, but
   I use it for portability. Feel free to replace it. */
double uniform_random(void)
{
    return (double) rand() / (double) RAND_MAX;
}

/* This Gaussian routine is stolen from Numerical Recipes and is their
   copyright. */

double gaussian_random(void)
{
    static int next_gaussian = 0;
    static double saved_gaussian_value;

    double fac, rsq, v1, v2;

    if (next_gaussian == 0) {
        do {
            v1 = 2.0*uniform_random()-1.0;
            v2 = 2.0*uniform_random()-1.0;
            rsq = v1*v1+v2*v2;
        } while (rsq >= 1.0 || rsq == 0.0);
        fac = sqrt(-2.0*log(rsq)/rsq);
        saved_gaussian_value=v1*fac;
        next_gaussian=1;
        return v2*fac;
    } else {
        next_gaussian=0;
        return saved_gaussian_value;
    }
}

double evaluate_gaussian(double val, double sigma)
{
    /* This private definition is used for portability */
    static const double Condense_PI = 3.14159265358979323846;

    return 1.0/(sqrt(2.0*Condense_PI) * sigma) *
            exp(-0.5 * (val*val / (sigma*sigma)));
}

/* The process model for a first-order auto-regressive process is:

   x_{t+1} - mean = (x_t - mean)*scaling + sigma*w_t

   where w_t is unit iid Gaussian noise. gaussian_random can be found
   in utility.c */
QPointF iterate_first_order_arp(QPointF previous, ProcessModel process)
{
    double val_x = process.sigma * gaussian_random();
    double val_y = process.sigma * gaussian_random();
    return QPointF(process.mean, process.mean) +
            ((previous - QPointF(process.mean, process.mean)) * process.scaling) + QPointF(val_x, val_y);
}

/* All of the global information is packaged into the following two
   structures. `global' contains information which is constant over a
   run of the algorithm and `data' contains all of the current state
   at a given iteration. */

GlobalData global;
IterationData data;

Tracker::Tracker(QObject *parent) :
    QObject(parent)
{
    m_initialized = false;
    m_initX = 0.0f;    // start x position
    m_initY = 0.0f;    // start y position
    m_initWidth = 0;
    m_initHeight = 0;

    m_isLost = false;
}

void Tracker::setInitialState(double x, double y, int width, int height)
{
    m_initX = x;
    m_initY = y;
    m_trajectory.append(QPointF(x, y));

    m_initWidth = width;
    m_initHeight = height;
}

void Tracker::propagate(cv::Mat frame)
{
    //    qDebug() << "Tracker::propagate -->";
    m_currentFrame = frame;

    if (!m_initialized)
        initialise();

    condensation_obtain_observations();     /* Go make necessary measurements */
    condensation_predict_new_bases();       /* Push previous state through process model */
    condensation_calculate_base_weights();  /* Apply Bayesian measurement weighting */
    condensation_update_after_iterating(); /* Tidy up, display output, etc. */
    //    qDebug() << "<-- Tracker::propagate";
}

QList<QPointF> Tracker::getCandidates()
{
    QList<QPointF> candidates;
    for (int i = 0; i < global.nsamples; i++)
    {
        candidates.append(data.old_positions[i]);
    }

    return candidates;
}

bool Tracker::isLost()
{
    return m_isLost;
}

QPointF Tracker::getTrackedPos()
{
    if (!m_isLost)
        return m_trajectory.last();
    else
        return QPointF();
}

QRect Tracker::getTrackedRectangle()
{
    if (!m_isLost && !m_trajectory.isEmpty())
    {
        QPointF pos =  m_trajectory.last();
        int tl_x = qMax((int)(pos.x() - m_initWidth/2), 0);
        int tl_y = qMax((int)(pos.y() - m_initHeight/2), 0);
        return QRect(tl_x, tl_y, m_initWidth, m_initHeight);
    }

    return QRect(0,0,0,0);
}

QList<QPointF> Tracker::getTrajectory()
{
    return m_trajectory;
}

QList<QRect> Tracker::getTrajectoryRects()
{
    QList<QRect> trajectoryRects;
    for (int i = 0; i < m_trajectory.size(); i++)
    {
        QPointF pos =  m_trajectory.at(i);
        int tl_x = qMax((int)(pos.x() - m_initWidth/2), 0);
        int tl_y = qMax((int)(pos.y() - m_initHeight/2), 0);
        QRect rect = QRect(tl_x, tl_y, m_initWidth, m_initHeight);
        trajectoryRects.append(rect);
    }

    return trajectoryRects;
}

bool Tracker::initialise()
{
    if (!condensation_init())
    {
        qDebug() << "Failed to init Condensation algorithm";
        return false;
    }

    m_trajectory.clear();

    m_initialized = true;
    return true;
}

/* Create all the arrays, then fill in the prior distribution for the
   first iteration. The prior is model-dependent, so
   set_up_prior_conditions can be found in model_specific.c */
bool Tracker::condensation_init()
{
    condensation_init_defaults();

    data.new_positions = (QPointF*)malloc(sizeof(QPointF) * global.nsamples);
    data.old_positions = (QPointF*)malloc(sizeof(QPointF) * global.nsamples);

    data.sample_weights = (double*)malloc(sizeof(double) * global.nsamples);
    data.cumul_prob_array = (double*)malloc(sizeof(double) * global.nsamples);

    if (!data.new_positions || !data.old_positions ||
            !data.sample_weights || !data.cumul_prob_array)
    {
        fprintf(stderr, "Failed to allocate memory for sample arrays\n");
        qDebug() << "Failed to allocate memory for sample arrays";
        return false;
    }

    condensation_set_up_prior_conditions();

    return true;
}

/* This routine fills in the data structures with default constant
   values. It could be enhanced by reading informatino from the
   command line to allow e.g. N to be altered without recompiling. */
void Tracker::condensation_init_defaults(void)
{
    global.nsamples = NSamples;
    global.niterations = NIterations;

    /* The following routines are model-specific and should be replaced to
       implement an arbitrary process and observation model. */

    /* Set up the parameters of the simulation model, process and
       observation. */
    global.scene.process.mean = SimulatedMean;
    global.scene.process.scaling = SimulatedScaling;
    global.scene.process.sigma = SimulatedSigma;
    global.scene.sigma = SimulatedMeasSigma;

    /* Set up the parameters of the prior distribution */
    global.prior.mean = PriorMean;
    global.prior.sigma = PriorSigma;

    /* Set up the parameters of the process model */
    global.process.mean = ProcessMean;
    global.process.scaling = ProcessScaling;
    global.process.sigma = ProcessSigma;

    /* Set up the parameters of the observation model */
    global.obs.sigma = ObsSigma;
}

/* Set up the initial sample-set according to the prior model. The
   prior is a simple Gaussian, so each of the positions is filled in
   by sampling that Gaussian and the weights are initialised to be
   uniform. gaussian_random can be found in utility.c */
void Tracker::condensation_set_up_prior_conditions()
{
    int n;

    for (n = 0; n < global.nsamples; ++n)
    {
        double val_x = global.prior.sigma * gaussian_random();
        double val_y = global.prior.sigma * gaussian_random();
        data.old_positions[n] = QPointF(m_initX, m_initY) + QPointF(val_x, val_y);

        /* The probabilities are not normalised. */
        data.cumul_prob_array[n] = (double) n;
        data.sample_weights[n] = 1.0;
    }

    /* The probabilities are not normalised, so store the largest value
     here (for simplicity of the binary search algorithm,
     cumul_prob_array[0] = 0). This can then be used as a
     multiplicative normalisation constant for the sample_weights
     array, as well. */
    data.largest_cumulative_prob = (double) n;

    /* This is the initial positions of the simulated object. */
    //    data.meas.truePos = QPointF(m_initX, m_initY);
}

/* In a real implementation, this routine would go and actually make
   measurements and store them in the data.meas structure. This
   simulation consists of an `object' moving around obeying a
   first-order auto-regressive process, and being observed with its
   true positions corrupted by Gaussian measurement noise.
   Accordingly, this routine calculates the new simulated true and
   measured position of the object. */
void Tracker::condensation_obtain_observations()
{
    //    qDebug() << "Tracker::condensation_obtain_observations -->";
    //    data.meas.truePos = iterate_first_order_arp(data.meas.truePos, global.scene.process);
    //    double val_x = global.scene.sigma * gaussian_random();
    //    double val_y = global.scene.sigma * gaussian_random();
    //    data.meas.observed = data.meas.truePos + QPointF(val_x, val_y);
    //    qDebug() << "<-- Tracker::condensation_obtain_observations";
}

/* This routine computes all of the new (unweighted) sample
   positions. For each sample, first a base is chosen, then the new
   sample position is computed by sampling from the prediction density
   p(x_t|x_t-1 = base). predict_sample_position is obviously
   model-dependent and is found in model_specific.c, but it can be
   replaced by any process model required. */
void Tracker::condensation_predict_new_bases()
{
    for (int n = 0; n < global.nsamples; ++n)
    {
        int base = condensation_pick_base_sample();
        condensation_predict_sample_position(n, base);
    }
}

int Tracker::condensation_pick_base_sample()
{
    double choice = uniform_random() * data.largest_cumulative_prob;
    int low, middle, high;

    low = 0;
    high = global.nsamples;

    while (high > (low+1))
    {
        middle = (high+low)/2;
        if (choice > data.cumul_prob_array[middle])
            low = middle;
        else
            high = middle;
    }

    return low;
}

/* This routine samples from the distribution

   p(x_t | x_{t-1} = old_positions[old_sample])

   and stores the result in new_positions[new_sample]. This is
   straightforward for the simple first-order auto-regressive process
   model used here, but any model could be substituted. */
void Tracker::condensation_predict_sample_position(int new_sample, int old_sample)
{
    data.new_positions[new_sample] = iterate_first_order_arp(data.old_positions[old_sample], global.process);
}

/* Once all the unweighted sample positions have been computed using
   predict_new_bases, this routine computes the weights by evaluating
   the observation density at each of the positions. Cumulative
   probabilities are also computed at the same time, to permit an
   efficient implementation of pick_base_sample using binary
   search. evaluate_observation_density is obviously model-dependent
   and is found in model_specific.c, but it can be replaced by any
   observation model required. */
void Tracker::condensation_calculate_base_weights()
{
//    struct timeval timstr;      /* structure to hold elapsed time */
//    struct rusage ru;           /* structure to hold CPU time--system and user */
//    double tic, toc;             /* floating point numbers to calculate elapsed wallclock time */
//    double usrtim;              /* floating point number to record elapsed user CPU time */
//    double systim;              /* floating point number to record elapsed system CPU time */

//    gettimeofday(&timstr,NULL);
//    tic=timstr.tv_sec+(timstr.tv_usec/1000000.0);

    double cumul_total = 0.0;

//#pragma omp parallel for
    for (int n = 0; n < global.nsamples; ++n)
    {
        data.sample_weights[n] = condensation_evaluate_observation_density(data.new_positions[n]);
        data.cumul_prob_array[n] = cumul_total;
        cumul_total += data.sample_weights[n];
    }
    data.largest_cumulative_prob = cumul_total;

//    gettimeofday(&timstr,NULL);
//    toc = timstr.tv_sec+(timstr.tv_usec/1000000.0);
//    getrusage(RUSAGE_SELF, &ru);
//    timstr = ru.ru_utime;
//    usrtim = timstr.tv_sec+(timstr.tv_usec/1000000.0);
//    timstr = ru.ru_stime;
//    systim = timstr.tv_sec+(timstr.tv_usec/1000000.0);

//    qDebug() << "";
//    qDebug() << "Elapsed time: " << toc-tic << "(s)";
//    qDebug() << "Elapsed user CPU time: " << usrtim << "(s)";
//    qDebug() << "Elapsed system CPU time: " << systim << "(s)";
//    qDebug() << "";
}

/* Go and output the estimate for this iteration (which is a
   model-dependent routine found in model_specific.c) and then swap
   over the arrays ready for the next iteration. */
void Tracker::condensation_update_after_iterating()
{
    QPointF estimated_pos = condensation_cal_estimated_pos();

    if (!estimated_pos.isNull())
    {
        m_isLost = false;

        // save the trajectory
        m_trajectory.append(estimated_pos);

        QPointF *temp;

        temp = data.new_positions;
        data.new_positions = data.old_positions;
        data.old_positions = temp;
    } else {
        m_isLost = true;
        condensation_shut_down();
    }
}

/* This routine evaluates the observation density

   p(z_t|x_t = new_positions[new_sample])

   The observation model in this implementation is a simple mixture of
   Gaussians, where each simulated object is observed as a 1d position
   and measurement noise is represented as Gaussian. For a
   visual-tracking application, this routine would go and evaluate the
   likelihood that the object is present in the image at the position
   encoded by new_positions[new_sample]. evaluate_gaussian can be
   found in utility.c */
double Tracker::condensation_evaluate_observation_density(QPointF samplePoint)
{
    cv::Mat roiFrame;
    int tl_x = qMax((int)(samplePoint.x() - m_initWidth/2), 0);
    int tl_y = qMax((int)(samplePoint.y() - m_initHeight/2), 0);
    //    qDebug() << "roi: " << QPoint(tl_x, tl_y);

    int width = qMin(m_initWidth + 100, m_currentFrame.cols - tl_x);
    int height = qMin(m_initHeight + 100, m_currentFrame.rows - tl_y);

    cv::Rect roi = cv::Rect(tl_x, tl_y, width, height); // ROI rect in m_currentFrame

    m_currentFrame(roi).copyTo(roiFrame);

//    cv::namedWindow("ss",1);
//    cv::imshow("ss", roiFrame);

    QList< QVector<QPointF> > rects = m_vjClassifier.detectPenguins(roiFrame);
    if (rects.size() > 0)
    {
        return 1;
    }

    return 0;

    //    double distance = sqrt(pow(samplePoint.x() - data.meas.observed.x(), 2) + pow(samplePoint.y() - data.meas.observed.y(), 2));
    //    return evaluate_gaussian(distance, global.obs.sigma);
}

/* This routine computes the estimated position of the object by
   estimating the mean of the state-distribution as a weighted mean of
   the sample positions, then displays a histogram of the state
   distribution along with a character denoting the estimated position
   of the object. */
QPointF Tracker::condensation_cal_estimated_pos()
{
    QPointF aggregate;

    if (0 == data.largest_cumulative_prob)
        return aggregate;

    /* Compute the unnormalised weighted mean of the sample
     positions. */
    for (int n = 0; n < global.nsamples; ++n)
    {
        aggregate += (data.new_positions[n] * data.sample_weights[n]);
    }

    aggregate /= data.largest_cumulative_prob;

//    qDebug() << "Something good happened";
//    qDebug() << "Est: " << aggregate;

    return aggregate;
}

/* Tidy up */
void Tracker::condensation_shut_down(void)
{
    free(data.new_positions);
    free(data.old_positions);
    free(data.sample_weights);
    free(data.cumul_prob_array);
}
