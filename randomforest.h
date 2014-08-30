#ifndef RANDOMFOREST_H
#define RANDOMFOREST_H

#include <QObject>
#include <opencv2/ml/ml.hpp>

class RandomForest : public QObject
{
    Q_OBJECT
public:
    explicit RandomForest(QObject *parent = 0);
    void train(QString filename, int numOfSamples, int numOfFeatures);
    int predict(const cv::Mat& sample);
    float predict_prob(const cv::Mat& sample);

private:
    void test(QString filename, int numOfSamples, int numOfFeatures);
    int loadData(const char* filename,
                 cv::Mat samples,
                 cv::Mat lables,
                 int numOfSamples,
                 int numOfFeatures);

private:
    CvRTrees m_randomForest;
};

#endif // RANDOMFOREST_H
