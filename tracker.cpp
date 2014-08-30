#include "tracker.h"
#include "condensation/condensation.h"
#include "condensation/data_types.h"
#include "condensation/model_parameters.h"
#include "spatiotemporaldef.h"
#include "aralclustering.h"

#include <QtGlobal>
#include <QElapsedTimer>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <opencv2/core/version.hpp>

#include <QDebug>

void plot(cv::Mat& img, std::vector<cv::Point3f>& points, cv::Mat& labels)
{
    int total = points.size();
    if (total < 1)
        return;

    static cv::Scalar colors[] = {
        cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255),
        cv::Scalar(255, 255, 0), cv::Scalar(255, 0, 255), cv::Scalar(0, 255, 255)
    };

    int *labels_ptr = labels.ptr<int>(0);
    for (int i = 0; i < total; i++)
    {
        cv::Point2f p(points.at(i).x, points.at(i).y);
        cv::circle(img, p, 2, colors[labels_ptr[i]], 2);
    }
}

/**
 * Create a degree matrix out from the given adjacency matrix
 **/
cv::Mat degreeMatrix(cv::Mat& adjacency)
{
    int cols = adjacency.cols;

    cv::Mat degree(1, cols, CV_32FC1);
    for (int col = 0; col < cols; col++)
        degree.at<float>(0, col) = cv::sum(adjacency.col( col ))[0];

    return degree;
}

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

    return exp(-0.5 * (val*val / (sigma*sigma)));

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
    return QPointF(process.mean, process.mean)
            + (previous - QPointF(process.mean, process.mean)) * process.scaling
            + QPointF(val_x, val_y);
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

    m_frameWidth = 0;
    m_frameHeight = 0;

    m_initX = 0.0f;    // start x position
    m_initY = 0.0f;    // start y position
    m_initWidth = 0;
    m_initHeight = 0;

    m_isLost = false;
    m_sectionClusteringResult.clear();
    m_clusteringAlgorithm = Tracker::SpectralClustering;
    m_estimatedTargetAmount = 0;
}

void Tracker::setFrameSize(int width, int height)
{
    m_frameWidth = width;
    m_frameHeight = height;

    if (!m_initialized)
        initialise();
}

void Tracker::setInitialState(double x, double y, int width, int height)
{
    m_initX = x;
    m_initY = y;

    m_initWidth = width;
    m_initHeight = height;
}

void Tracker::propagate(ViolaJonesClassifier::VJDetection obs)
{
    if (!m_initialized)
        initialise();

    m_spatioTemporalDetections.append(obs);

    int totalFrames = m_spatioTemporalDetections.length();
    int sectionSize = 15;
    int overlapingSize = 5;
    int sectionCount = (totalFrames - overlapingSize) / (sectionSize - overlapingSize);
    sectionCount = std::max(0, sectionCount);

    for (int i = 0; i < sectionCount; i++)
    {
        if (i < m_sectionClusteringResult.length())
            continue;

        int startFrame = (sectionSize - overlapingSize) * i;
        int endFrame = startFrame + sectionSize - 1;
        runClusteringModelSelection(startFrame, endFrame);
        int estimatedCount = getEstimatedTargetAmount();    // TODO: remove
        m_sectionClusteringResult.append(estimatedCount);   // TODO: remove
    }

//    runClusteringModelSelection(0, m_spatioTemporalDetections.length() - 1);
//    runTrajectoryCounting();

    condensation_obtain_observations(obs);     /* Go make necessary measurements */
    condensation_predict_new_bases();       /* Push previous state through process model */
    condensation_calculate_base_weights();  /* Apply Bayesian measurement weighting */
    condensation_update_after_iterating(); /* Tidy up, display output, etc. */
}

QList<Tracker::Particle> Tracker::getParticles()
{
    QList<Particle> particles;
    for (int i = 0; i < global.nsamples; i++)
    {
        Particle p;
        p.pos = data.old_positions[i];
        p.weight = data.sample_weights[i];
        particles.append(p);
    }

    return particles;
}

QList<Tracker::Particle> Tracker::getDetections()
{
    QList<Particle> particles;
    for (int i = 0; i < data.meas.observed.length(); i++)
    {
        Particle p;
        p.pos = data.meas.observed[i];
        p.weight = 1;
        particles.append(p);
    }

    return particles;
}

QList<STCluster> Tracker::getSTClusters()
{
    return m_clusters;
}

QList<STLine> Tracker::getAllLineSegments()
{
    return m_lineSegments;
}

QList<STPoint> Tracker::getAllSTPoints()
{
    QList<STPoint> stPointList;
    int startFrame = 0;
    int endFrame = m_spatioTemporalDetections.length() - 1;
    std::vector<cv::Point3f> points = st_getPoints(startFrame, endFrame);
    int pointsCount = (int)points.size();

    for (int i = 0; i < pointsCount; i++)
    {
        cv::Point3f point = points.at(i);

        STPoint stPoint;
        stPoint.x = point.x;
        stPoint.y = point.y;
        stPoint.t = point.z;

        stPointList.append(stPoint);
    }

    return stPointList;
}

QList<STPoint> Tracker::getFilteredSTPoints()
{
    return m_filteredPoints;
}

QList<STCone> Tracker::getSTCones()
{
    return m_cones;
}

QList<STPoint> Tracker::getManualLabelings()
{
    return m_manualLabelings;
}

QList< QList<QColor> > Tracker::getColorLabels()
{
    return m_colorLabels;
}

QList<Tracker::Trajectory> Tracker::getTrajectories()
{
    return m_trajectories;
}

cv::Mat Tracker::getDetectionConfidenceMap()
{
    return m_confidenceMap;
}

int Tracker::getEstimatedTargetAmount()
{
    return m_estimatedTargetAmount;
}

QList<int> Tracker::getCountingResult()
{
    return m_sectionClusteringResult;
}

bool Tracker::isLost()
{
    return m_isLost;
}

bool Tracker::isInit()
{
    return m_initialized;
}

void Tracker::addManualLabeling(float x, float y, int frameNo)
{
    STPoint stPoint;
    stPoint.x = x;
    stPoint.y = y;
    stPoint.t = frameNo;

    m_manualLabelings.append(stPoint);
}

bool Tracker::initialise()
{
    if (!condensation_init())
    {
        qDebug() << "Failed to init Condensation algorithm";
        return false;
    }

    if (0 == m_frameWidth || 0 == m_frameHeight)
    {
        qDebug() << "Failed to init Condensation algorithm, the frame size is not initialized";
        return false;
    }

    m_spatioTemporalDetections.clear();
    m_initialized = true;

    return true;
}

QPointF Tracker::generateRandomPosition()
{
    // 2D uniform distribution
    double val_x = rand() % m_frameWidth;
    double val_y = rand() % m_frameHeight;
    return QPointF(val_x, val_y);
}

bool Tracker::insideFrame(QPointF sample)
{
    if (sample.x() >= 0 && sample.x() <= m_frameWidth && sample.y() >=0 && sample.y() <= m_frameHeight)
        return true;
    else
        return false;
}

void Tracker::runClusteringModelSelection(int startFrame, int endFrame)
{
    std::vector<cv::Point3f> points = st_getPoints(startFrame, endFrame);
    int pointsCount = (int)points.size();

    cv::Mat labels;

    // TODO: Space time cone filtering
    qDebug() << "Points before filtering:" << pointsCount;

    QList<STPoint> filteredPoints = runSpaceTimeConeFiltering(startFrame, endFrame);
    m_filteredPoints.append(filteredPoints);
    points = Utility::QListSTPointToStdVectorCvPoint3f(filteredPoints);
    pointsCount = (int)points.size();

    qDebug() << "Points after filtering:" << pointsCount;

    if (0 == pointsCount)
        return;

#ifdef PERFORMANCE_TUNING
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
#endif

    int maxCluster = 0;
//    int linesCount = clusterDetections_HoughTransform(startFrame, endFrame);
//    if (linesCount < 1)
//        maxCluster = pointsCount;
//    else
//        maxCluster = 2 * linesCount < pointsCount ? 2 * linesCount : pointsCount;

    maxCluster = std::min(pointsCount, 10);

    cv::Mat clusteringData;

    if (Tracker::SpectralClustering == m_clusteringAlgorithm)
    {
        /* Create adjacency and degree matrices */
        float sigma = 100.0f;
        cv::Mat adjacency = Utility::gaussianDistance(points, sigma);
        cv::Mat degree = degreeMatrix(adjacency);

        /* Create laplacian matrix */
        cv::Mat L = cv::Mat::diag(degree) - adjacency;
        cv::Mat degree_05;
        pow(degree, -0.5, degree_05);
        degree_05 = cv::Mat::diag(degree_05);
        L = (degree_05 * L) * degree_05;

        /* Perform eigen decompositions */
        cv::Mat eigenvalues, eigenvectors;
        cv::eigen(L, eigenvalues, eigenvectors);
        clusteringData = eigenvectors;
    } else if (Tracker::KMeansClustering == m_clusteringAlgorithm) {
        clusteringData = cv::Mat(points);
    } else if (Tracker::ARALClustering == m_clusteringAlgorithm) {
        // do nothing, we will use points
    }

    class ARALClustering aralClustering;
    std::vector<int> rhos = aralClustering.calculateLocalDensity(points);
//    std::cout << "rhos:" << std::endl;
    int avgRho = 0;
    for (size_t n = 0; n < rhos.size(); n++)
    {
        avgRho += rhos.at(n);
//        std::cout << rhos.at(n) << std::endl;
    }

    if (0 != rhos.size())
    {
        avgRho /= rhos.size();
//        std::cout << "Avg. rho:" << avgRho << std::endl;
    }

//    std::cout << "\n" << std::endl;

    std::vector<double> deltas = aralClustering.calculateDistanceToHigherDensity(points, rhos);
//    std::cout << "deltas:" << std::endl;
    double avgDelta = 0.0;
    for (size_t m = 0; m < deltas.size(); m++)
    {
        avgDelta += deltas.at(m);
//        std::cout << deltas.at(m) << std::endl;
    }

    if (0 != deltas.size())
    {
        avgDelta /= deltas.size();
//        std::cout << "Avg. delta:" << avgDelta << std::endl;
    }

    if (rhos.size() == deltas.size())
    {
        int rhoThreshold = avgRho/2;

        double maxDelta = aralClustering.getMaxDelta(deltas);
        double deltaThreshold = maxDelta - (maxDelta - avgDelta)/2;

        for (int ii = 0; ii < (int)rhos.size(); ii++)
        {
            int rho = rhos.at(ii);
            double delta = deltas.at(ii);

            if (rho < rhoThreshold && delta > deltaThreshold)
            {
                cv::Point3f p = points.at(ii);
                STPoint stP;
                stP.x = p.x;
                stP.y = p.y;
                stP.t = p.z;
//                std::cout << "Outlier candidate:" << "(" << p.x << "," << p.y << "," << p.z << ")" << std::endl;
            }
        }
    }

    double minMDL = 100000;
    int clusterCount = 1;
    cv::Mat candidateLabels;
    for (int k = 1; k <= maxCluster; k++)
    {
        cv::Mat dataPoints;

        if (Tracker::SpectralClustering == m_clusteringAlgorithm)
        {
            /* Since it's automatically sorted in descending order, take the last two entries of eigenvectors */
            dataPoints = clusteringData.rowRange(clusteringData.rows - k, clusteringData.rows).t();
        } else if (Tracker::KMeansClustering == m_clusteringAlgorithm) {
            dataPoints = clusteringData;
        } else if (Tracker::ARALClustering == m_clusteringAlgorithm) {
            class ARALClustering aralClustering;
            std::vector<int> rhos = aralClustering.calculateLocalDensity(points);
            std::cout << "rhos:" << std::endl;
            for (size_t n = 0; n < rhos.size(); n++)
            {
                std::cout << rhos.at(n) << std::endl;
            }

            std::cout << "\n" << std::endl;

            std::vector<double> deltas = aralClustering.calculateDistanceToHigherDensity(points, rhos);
            std::cout << "deltas:" << std::endl;
            for (size_t m = 0; m < deltas.size(); m++)
            {
                std::cout << deltas.at(m) << std::endl;
            }
        }

        cv::kmeans(dataPoints,
                   k,
                   candidateLabels,
                   cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),
                   3,
                   cv::KMEANS_PP_CENTERS);

        double mdl = calculateMDL(k, cv::Mat(points), candidateLabels);
        if (mdl < minMDL)
        {
            minMDL = mdl;
            clusterCount = k;
            labels = candidateLabels.clone();
        }
    }
    m_estimatedTargetAmount = clusterCount;

#ifdef PERFORMANCE_TUNING
    qDebug() << "Tracker::runClusteringModelSelection MDL elapsed time: " << elapsedTimer.elapsed();
#endif

    // Prepare cluster informations
    m_clusters.clear();
    double interval = 1.0 / double(clusterCount+1);
    for (int label = 0; label < clusterCount; label++)
    {
        double val = double(label+1) * interval;
        QColor color = Utility::getJetColor(val, 0.0, 1.0);

        STCluster cluster;
        cluster.label = label;
        cluster.color = color;

        m_clusters.append(cluster);
    }

    for (int i = 0; i < pointsCount; i++)
    {
        int label = labels.at<int>(i);
        cv::Point3f point = points.at(i);

        STPoint stPoint;
        stPoint.x = point.x;
        stPoint.y = point.y;
        stPoint.t = point.z;

        STCluster cluster = m_clusters.at(label);
        cluster.points.append(stPoint);

        m_clusters.replace(label, cluster);
    }

    // Cluster outlier removal
//    runClusterOutlierRemoval();

    // Calculate eigen vector(line) for each cluster
    runClusterEigenLineCalculation();

    // Update trajectories and detection colors(for each cluster) for each frame
//    m_trajectories.clear();
//    for (int cluster = 0; cluster < clusterCount; cluster++)
//    {
//        Trajectory track;
//        track.label = cluster;
//        m_trajectories.append(track);
//    }

//    m_colorLabels.clear();

//    int offset = 0;
//    for (int t = startFrame; t <= endFrame; t++)
//    {
//        QList<QColor> colors;
//        ViolaJonesClassifier::VJDetection obs = m_spatioTemporalDetections.at(t);

//        int num = obs.polygons.length();
//        for (int i = 0; i < num; i++)
//        {
//            int label = labels.at<int>(offset + i);
//            double val = double(label+1)*interval;

//            QColor color = Utility::getJetColor(val, 0.0, 1.0);
//            colors << color;

//            Trajectory track;
//            track.label = label;
//            track.color = color;
//            track.points = m_trajectories.at(label).points;

//            QPointF center = obs.polygons.at(i).boundingRect().center();
//            track.points.append(center);

//            m_trajectories.replace(label, track);
//        }

//        m_colorLabels.append(colors);
//        offset += num;
//    }
}

QList<STPoint> Tracker::runSpaceTimeConeFiltering(int startFrame, int endFrame)
{
#ifdef PERFORMANCE_TUNING
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
#endif

    QList<STPoint> filteredSTPoints;

    int maxVelocity = 20;
    int deltaTime = 1;

    QList<STCone> cones;
    // For each time/frame t
    for (int t = startFrame; t <= endFrame; t++)
    {
        ViolaJonesClassifier::VJDetection obs = m_spatioTemporalDetections.at(t);
        int num = obs.polygons.length();
        for (int i = 0; i < num; i++)
        {
            QPointF center = obs.polygons.at(i).boundingRect().center();
            STPoint point;
            point.x = center.x();
            point.y = center.y();
            point.t = t * 10;

            filteredSTPoints.append(point);

            // Cone growing for a. static object, or b. holonomically moving object
            if (0 == t)
            {
                STCone cone;
                cone.points.append(point);
                cones.append(cone);
            } else {
                bool assignment = false;
                for (int j = 0; j < cones.length(); j++)
                {
                    STCone cone = cones.at(j);
                    QList<STPoint> points = cone.points;

                    for (int n = points.length() - 1; n >= 0 ; n--)
                    {
                        STPoint p = points.at(n);
                        if (1 * 10 != (point.t - p.t))
                            break;

                        double distance = sqrt( pow((point.x - p.x), 2) + pow((point.y - p.y), 2) );
                        if (distance <= (maxVelocity * deltaTime))
                        {
                            cone.points.append(point);
                            cones.replace(j, cone);

                            assignment = true;
                        }
                    }
                }

                if (!assignment)
                {
                    STCone cone;
                    cone.points.append(point);
                    cones.append(cone);
                }
            }
        }
    }

    for (int n = 0; n < cones.length(); n++)
    {
        STCone cone = cones.at(n);
        QList<STPoint> stPoints = cone.points;

        if (stPoints.length() < 3)
        {
            foreach (const STPoint &pointToRemove, stPoints)
            {
                filteredSTPoints.removeOne(pointToRemove);
            }
        } else {
            m_cones.append(cone);
        }
    }

//    for (int n = 0; n < cones.length(); n++)
//    {
//        qDebug() << "Cone " << n+1;
//        STCone cone = cones.at(n);
//        QList<STPoint> stPoints = cone.points;
//        for (int m = 0; m < stPoints.length(); m++)
//        {
//            STPoint p = stPoints.at(m);
//            qDebug() << QString("(%1,%2,%3)").arg(p.x).arg(p.y).arg(p.t/10);
//        }
//    }

#ifdef PERFORMANCE_TUNING
    qDebug() << "Tracker::runSpaceTimeConeFiltering elapsed time: " << elapsedTimer.elapsed();
#endif

    return filteredSTPoints;
}

void Tracker::runClusterOutlierRemoval()
{
#ifdef PERFORMANCE_TUNING
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
#endif

    int clusterCount = m_estimatedTargetAmount;
    for (int label = 0; label < clusterCount; label++)
    {
        STCluster cluster = m_clusters.at(label);
        QList<STPoint> points = cluster.points;

        bool removalNeeded;
        do {
            removalNeeded = false;
            int clusterSize = points.length();

            if (clusterSize > 1)
            {
                QList<int> pointTimeDiffList;
                QList<float> pointSpaceDiffList;
                for (int i = 0; i < clusterSize; i++)
                {
                    if (i > 0)
                    {
                        int tDiff = points.at(i).t - points.at(i-1).t;
                        pointTimeDiffList.append(tDiff);

                        float xDiff = points.at(i).x - points.at(i-1).x;
                        float yDiff = points.at(i).y - points.at(i-1).y;

                        float sDiff = sqrt(pow(xDiff, 2) + pow(yDiff, 2));
                        pointSpaceDiffList.append(sDiff);
                    }
                }

                int maxTimeDiff = 5;
                int maxSpaceDiff = 100;
                for (int j = 0; j < pointTimeDiffList.length(); j++)
                {
                    int tDiff = pointTimeDiffList.at(j);
                    if (0 == tDiff)
                        continue;

                    float sDiff = pointSpaceDiffList.at(j);

                    if (tDiff > maxTimeDiff * 10 || sDiff > maxSpaceDiff)
                    {
                        points.removeAt(j);
                        removalNeeded = true;
                        break;
                    }
                }
            }
        } while (removalNeeded);

        cluster.points = points;
        m_clusters.replace(label, cluster);
    }

#ifdef PERFORMANCE_TUNING
    qDebug() << "Tracker::runClusterOutlierRemoval elapsed time: " << elapsedTimer.elapsed();
#endif
}

void Tracker::runClusterEigenLineCalculation()
{
#ifdef PERFORMANCE_TUNING
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
#endif

    int clusterCount = m_estimatedTargetAmount;
    for (int label = 0; label < clusterCount; label++)
    {
        STCluster cluster = m_clusters.at(label);
        QList<STPoint> stPoints = cluster.points;
        std::vector<cv::Point3f> cvPoints;
        for (int s = 0; s < stPoints.length(); s++)
        {
            STPoint stP = stPoints.at(s);
            cv::Point3f cvP(stP.x, stP.y, stP.t);
            cvPoints.push_back(cvP);
        }

        if (stPoints.length() > 1)
        {
            STLine line = Utility::fit3DLineToPoints(cvPoints);
            cluster.eigenLine = line;
            m_lineSegments.append(line);
        }

        m_clusters.replace(label, cluster);
    }

#ifdef PERFORMANCE_TUNING
    qDebug() << "Tracker::runClusterEigenLineCalculation elapsed time: " << elapsedTimer.elapsed();
#endif
}

void Tracker::runTrajectoryCounting()
{
    std::vector<cv::Point3f> points = st_getPoints(0, m_spatioTemporalDetections.length() - 1);
    cv::Mat clusteringData = cv::Mat(points);
    Utility::fastICA(clusteringData);
}

std::vector<cv::Point3f> Tracker::st_getPoints(int startFrame, int endFrame)
{
    std::vector<cv::Point3f> points;

    // For each time/frame t
    for (int t = startFrame; t <= endFrame; t++)
    {
        ViolaJonesClassifier::VJDetection obs = m_spatioTemporalDetections.at(t);
        int num = obs.polygons.length();
        for (int i = 0; i < num; i++)
        {
            QPointF center = obs.polygons.at(i).boundingRect().center();
            points.push_back(cv::Point3f(center.x(), center.y(), t*10));
        }
    }

    return points;
}

int Tracker::clusterDetections_HoughTransform(int startFrame, int endFrame)
{
#ifdef PERFORMANCE_TUNING
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
#endif

    int x_offset = 10 / 2;
    int y_offset = 20 / 2;

    int frameCount = endFrame - startFrame + 1;
    int space_width = 320 + x_offset * (frameCount-1);
    int space_height = 150 + y_offset * (frameCount-1);
    cv::Mat spaceImage = cv::Mat::zeros(space_height, space_width, CV_8UC3);

    for (int t = startFrame; t < endFrame; t++)
    {
        ViolaJonesClassifier::VJDetection obs = m_spatioTemporalDetections.at(t);
        int num = obs.polygons.length();
        for (int i = 0; i < num; i++)
        {
            QPointF center = obs.polygons.at(i).boundingRect().center();
            center /= 4;

            int x = center.x() + ((frameCount-1) - t) * x_offset;
            int y = center.y() + ((frameCount-1) - t) * y_offset;

            int thickness = -1;
            int lineType = 8;

            cv::Point p(x, y);
            cv::circle(spaceImage,
                       p,
                       1,
                       cv::Scalar(255, 255, 255),
                       thickness,
                       lineType);
        }
    }

    spaceImage.at<float>(75, 160) = 255;

    cv::Mat dst, color_dst;
    cv::Canny(spaceImage, dst, 50, 200, 3);
    cv::cvtColor(dst, color_dst, cv::COLOR_GRAY2BGR);

    std::vector<cv::Vec4i> lines;
    double rho = 1;
    double theta = CV_PI/180;
    int threshold = 6;
    double minLineLength = 80;
    double maxLineGap = 40;

    cv::HoughLinesP(dst, lines, rho, theta * 2, threshold, minLineLength, maxLineGap);
    for (size_t i = 0; i < lines.size(); i++)
    {
        cv::line(color_dst, cv::Point(lines[i][0], lines[i][1]),
                cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0,255,0), 2, 8);
    }

//    cv::namedWindow( "Detected Lines", 1 );
//    cv::imshow( "Detected Lines", color_dst );

#ifdef PERFORMANCE_TUNING
    qDebug() << "Tracker::clusterDetections_HoughTransform elapsed time: " << elapsedTimer.elapsed();
#endif

    return (int)lines.size();
}

double Tracker::calculateMDL(int k, cv::Mat points, cv::Mat labels)
{
    QHash<int, double> clusterHz;
    QHash<int, std::vector<cv::Point3f> > clusterPoints;

    for (int i = 0; i < k; i++)
    {
        clusterHz.insert(i,0);
    }

    for (int i = 0; i < points.rows; i++)
    {
        float x = points.at<float>(i, 0);
        float y = points.at<float>(i, 1);
        float z = points.at<float>(i, 2);
        cv::Point3f p(x, y, z);

        int label = labels.at<int>(i);
        clusterHz[label]++;
        clusterPoints[label].push_back(p);
    }

    double COST = 0.0;
    foreach (int key, clusterPoints.keys())
    {
        std::vector<cv::Point3f> points = clusterPoints.value(key);
        double volume = 0.0;
        if (points.size() > 1)
            volume = Utility::calClusterVolumn(points);

        COST += volume;
    }

    int dimension = 3;
    int numOfParameters = dimension * k;
    double N = points.rows;
    double COMP = 0.5 * numOfParameters * log(N);

    COST /= 10;
    COMP *= 3;
    double mdl = COST + COMP;

//    qDebug() << "Cluster K:" << k;
//    qDebug() << "Cluster MDL:" << mdl;
//    qDebug() << "        MDL COST:" << COST;
//    qDebug() << "        MDL COMP:" << COMP;
//    qDebug() << "";

    return mdl;
}

int Tracker::getTotalDetectionCount()
{
    int count = 0;
    for (int n = 0; n < m_spatioTemporalDetections.length(); n++)
    {
        ViolaJonesClassifier::VJDetection obs = m_spatioTemporalDetections.at(n);
        count += obs.polygons.length();
    }
    return count;
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

    /* The following routines are model-specific and should be replaced to
       implement an arbitrary process and observation model. */

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
        // 2D uniform distribution
        data.old_positions[n] = generateRandomPosition();

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
}

/* In a real implementation, this routine would go and actually make
   measurements and store them in the data.meas structure. This
   simulation consists of an `object' moving around obeying a
   first-order auto-regressive process, and being observed with its
   true positions corrupted by Gaussian measurement noise.
   Accordingly, this routine calculates the new simulated true and
   measured position of the object. */
void Tracker::condensation_obtain_observations(ViolaJonesClassifier::VJDetection obs)
{
    QList<QPointF> detectionCentroids;
    cv::Mat binaryImg = cv::Mat::ones(m_frameHeight, m_frameWidth, CV_8UC1);

    for (int i = 0; i < obs.polygons.size(); i++)
    {
        QRectF rect = obs.polygons.at(i).boundingRect();
        QPointF center = rect.center();
        detectionCentroids.append(center);
        binaryImg.at<uchar>(center.y(), center.x()) = 0;    // interest point, seed
    }

    data.meas.observed = detectionCentroids;

    // Apply distance transform
#if CV_VERSION_EPOCH >= 3
    cv::distanceTransform(binaryImg, m_distanceMap, m_labels, cv::DIST_L2, cv::DIST_MASK_5, cv::DIST_LABEL_PIXEL);
#else // if 2.4.9 or older
    cv::distanceTransform(binaryImg, m_distanceMap, m_labels, CV_DIST_L2, CV_DIST_MASK_5, cv::DIST_LABEL_PIXEL);
#endif

    for (int m = 0; m < detectionCentroids.length(); m++)
    {
        int dLabel = m_labels.at<int>(detectionCentroids.at(m).y(), detectionCentroids.at(m).x());
        m_confidenceHash.insert(dLabel, obs.weights.at(m));
    }

    m_confidenceMap = m_distanceMap.clone();
    for (int r = 0; r < m_distanceMap.rows; r++)
    {
        for (int c = 0; c < m_distanceMap.cols; c++)
        {
            float distance = m_distanceMap.at<float>(r, c);
            double eval = evaluate_gaussian(distance, global.obs.sigma);

            int pLabel = m_labels.at<int>(r, c);
            double confidence = m_confidenceHash.value(pLabel);

            eval *= confidence;
            m_confidenceMap.at<float>(r, c) = eval;
        }
    }
    cv::normalize(m_confidenceMap, m_confidenceMap, 0.0, 1.0, cv::NORM_MINMAX);
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
    QPointF pos = iterate_first_order_arp(data.old_positions[old_sample], global.process);
    data.new_positions[new_sample] = pos;
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
    double cumul_total = 0.0;

    for (int n = 0; n < global.nsamples; ++n)
    {
        QPointF samplePoint = data.new_positions[n];
        // TODO: log-likilihood
        float weight = insideFrame(samplePoint) ? condensation_evaluate_observation_density(samplePoint) : 0;
        data.sample_weights[n] = weight;
        data.cumul_prob_array[n] = cumul_total;
        cumul_total += data.sample_weights[n];
    }
    data.largest_cumulative_prob = cumul_total;
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
    } else {
        m_isLost = true;
        //        condensation_shut_down();
    }

    QPointF *temp;
    temp = data.new_positions;
    data.new_positions = data.old_positions;
    data.old_positions = temp;
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
    double eval = m_confidenceMap.at<float>(samplePoint.y(), samplePoint.x());
    return eval;
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
