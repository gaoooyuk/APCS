#include "randomforest.h"
#include <QDebug>

RandomForest::RandomForest(QObject *parent) :
    QObject(parent)
{
}

void RandomForest::train(QString filename, int numOfSamples, int numOfFeatures)
{
    cv::Mat samples = cv::Mat(numOfSamples, numOfFeatures, CV_32FC1);
    cv::Mat labels = cv::Mat(numOfSamples, 1, CV_32FC1);

    cv::Mat var_type = cv::Mat(numOfFeatures + 1, 1, CV_8U);
    var_type.setTo(cv::Scalar(CV_VAR_NUMERICAL) );
    var_type.at<uchar>(numOfFeatures, 0) = CV_VAR_CATEGORICAL;

    int ret = loadData(filename.toUtf8().data(), samples, labels, numOfSamples, numOfFeatures);
    if (!ret)
        return;

    float priors[] = {1,1};  // weights of each classification for classes
    CvRTParams params = CvRTParams(50, // max depth
                                   numOfSamples * 0.01, // min sample count
                                   0, // regression accuracy: N/A here
                                   false, // compute surrogate split, no missing data
                                   2, // max number of categories (use sub-optimal algorithm for larger numbers)
                                   priors, // the array of priors
                                   false,  // calculate variable importance
                                   0,       // number of variables randomly selected at node and used to find the best split(s).
                                   1,  // max number of trees in the forest
                                   0.001f,               // forrest accuracy
                                   CV_TERMCRIT_ITER |   CV_TERMCRIT_EPS // termination cirteria
                                   );

    m_randomForest.train(samples, CV_ROW_SAMPLE, labels, cv::Mat(), cv::Mat(), var_type, cv::Mat(), params);

    test(filename, numOfSamples, numOfFeatures);
}

int RandomForest::predict(const cv::Mat& sample)
{
    int res = (int)m_randomForest.predict(sample, cv::Mat());
    return res;
}

void RandomForest::test(QString filename, int numOfSamples, int numOfFeatures)
{
    cv::Mat samples = cv::Mat(numOfSamples, numOfFeatures, CV_32FC1);
    cv::Mat labels = cv::Mat(numOfSamples, 1, CV_32FC1);

    cv::Mat var_type = cv::Mat(numOfFeatures + 1, 1, CV_8U);
    var_type.setTo(cv::Scalar(CV_VAR_NUMERICAL) );
    var_type.at<uchar>(numOfFeatures, 0) = CV_VAR_CATEGORICAL;

    // TODO: read and parser data
    int ret = loadData(filename.toUtf8().data(), samples, labels, numOfSamples, numOfFeatures);
    if (!ret)
        return;

    int correct_class = 0;
    int wrong_class = 0;
    int false_positives [2] = {0,0};
    cv::Mat test_sample;
    double result;

    for (int tsample = 0; tsample < numOfSamples; tsample++)
    {
        // extract a row from the testing matrix
        test_sample = samples.row(tsample);
        result = m_randomForest.predict(test_sample, cv::Mat());

        qDebug() << "Testing Sample " << tsample << "-> class result (digit  " << (int) result << ")";

        // if the prediction and the (true) testing classification are the same
        // (N.B. openCV uses a floating point decision tree implementation!)
        if (fabs(result - labels.at<float>(tsample, 0)) >= FLT_EPSILON)
        {
            // if they differ more than floating point error => wrong class
            wrong_class++;
            false_positives[(int) result]++;
        } else {
            // otherwise correct
            correct_class++;
        }
    }

    qDebug() << "Correct classification: " << correct_class << "(" << (double)correct_class*100/numOfSamples << "%)";
    qDebug() << "Wrong classification: " << wrong_class << "(" << (double)wrong_class*100/numOfSamples << "%)";
}

int RandomForest::loadData(const char* filename, cv::Mat samples, cv::Mat lables, int numOfSamples, int numOfFeatures)
{
    float tmp;

    // if we can't read the input file then return 0
    FILE* f = fopen(filename, "r");
    if (!f)
    {
        qDebug() << "ERROR: cannot read file: " << filename;
        return 0; // all not OK
    }

    // for each sample in the file
    for (int line = 0; line < numOfSamples; line++)
    {
        // for each attribute on the line in the file
        for (int attribute = 0; attribute < (numOfFeatures + 1); attribute++)
        {
            if (attribute < numOfFeatures)
            {
                // first numOfFeatures(0~numOfFeatures-1) elements in each line are the attributes
                fscanf(f, "%f,", &tmp);
                samples.at<float>(line, attribute) = tmp;
            } else if (attribute == numOfFeatures) {
                // attribute numOfFeatures is the class label
                fscanf(f, "%f,", &tmp);
                lables.at<float>(line, 0) = tmp;
            }
        }
    }

    fclose(f);
    return 1; // all OK
}
