#ifndef COLORFEATUREEXTRACTOR_H
#define COLORFEATUREEXTRACTOR_H

#include "featureextractor.h"

class ColorFeatureExtractor : public FeatureExtractor
{
    Q_OBJECT
public:
    explicit ColorFeatureExtractor(QObject *parent = 0);

    // Reture image
    cv::Mat compute(QString imgPath, int horizontalBins, int verticalBins);
    cv::Mat compute(cv::Mat inputImage, int horizontalBins, int verticalBins);
};

#endif // COLORFEATUREEXTRACTOR_H
