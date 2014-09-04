#ifndef LBPFEATUREEXTRACTOR_H
#define LBPFEATUREEXTRACTOR_H

#include "featureextractor.h"

class LbpFeatureExtractor : public FeatureExtractor
{
    Q_OBJECT
public:
    explicit LbpFeatureExtractor(QObject *parent = 0);
    cv::Mat compute(QString imgPath, int horizontalBins, int verticalBins);
    cv::Mat compute(cv::Mat inputImage, int horizontalBins, int verticalBins);
};

#endif // LBPFEATUREEXTRACTOR_H
