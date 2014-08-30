#ifndef EDGEFEATUREEXTRACTOR_H
#define EDGEFEATUREEXTRACTOR_H

#include "featureextractor.h"

class EdgeFeatureExtractor : public FeatureExtractor
{
    Q_OBJECT
public:
    explicit EdgeFeatureExtractor(QObject *parent = 0);
    cv::Mat compute(QString imgPath);
    cv::Mat compute(cv::Mat inputImage);
};

#endif // EDGEFEATUREEXTRACTOR_H
