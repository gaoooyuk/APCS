#ifndef COLORFEATUREEXTRACTOR_H
#define COLORFEATUREEXTRACTOR_H

#include "featureextractor.h"

class ColorFeatureExtractor : public FeatureExtractor
{
    Q_OBJECT
public:
    explicit ColorFeatureExtractor(QObject *parent = 0);
    cv::Mat compute(QString imgPath);
    cv::Mat compute(cv::Mat inputImage);
    QVector<int> featureVector() const;

private:
    QVector<int> m_featureVector;
};

#endif // COLORFEATUREEXTRACTOR_H
