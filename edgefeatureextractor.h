#ifndef EDGEFEATUREEXTRACTOR_H
#define EDGEFEATUREEXTRACTOR_H

#include "featureextractor.h"

class EdgeFeatureExtractor : public FeatureExtractor
{
    Q_OBJECT
public:
    explicit EdgeFeatureExtractor(QObject *parent = 0);
    cv::Mat compute(QString imgPath, int horizontalBins, int verticalBins);
    cv::Mat compute(cv::Mat inputImage, int horizontalBins, int verticalBins);
};

#endif // EDGEFEATUREEXTRACTOR_H
