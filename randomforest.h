#ifndef RANDOMFOREST_H
#define RANDOMFOREST_H

#include <QObject>
#include <QSize>
#include <opencv2/ml/ml.hpp>

class RandomForest : public QObject
{
    Q_OBJECT
public:
    explicit RandomForest(QObject *parent = 0);
    void train(QString filename, int numOfSamples, int numOfFeatures);
    void test(QString filename, int numOfSamples, int numOfFeatures);
    float predict(const cv::Mat& sample);
    float predict_prob(const cv::Mat& sample);

    QSize workSize() const;
    void setWorkSize(QSize size);

    int numOfFeatures() const;
    void setNumOfFeatures(int numOfFeatures);

    cv::Mat computeFeatureVectors(QString imgPath, int w, int h);
    cv::Mat computeFeatureVectors(cv::Mat inputImage, int w, int h);

private:
    int loadData(const char* filename,
                 cv::Mat samples,
                 cv::Mat lables,
                 int numOfSamples,
                 int numOfFeatures);

private:
    CvRTrees m_randomForest;
    QSize m_workSize;
    int m_numOfFeatures;
};

#endif // RANDOMFOREST_H
