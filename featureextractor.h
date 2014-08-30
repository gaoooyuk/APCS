#ifndef FEATUREEXTRACTOR_H
#define FEATUREEXTRACTOR_H

#include <QObject>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

#include <QDebug>

class FeatureExtractor : public QObject
{
    Q_OBJECT
public:
    explicit FeatureExtractor(QObject *parent = 0);
    virtual cv::Mat compute(QString imgPath) = 0;
    virtual cv::Mat compute(cv::Mat inputImage) = 0;

    QVector<int> featureVector() const;

protected:
    QVector<int> m_featureVector;
};

#endif // FEATUREEXTRACTOR_H
