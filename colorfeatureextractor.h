#ifndef COLORFEATUREEXTRACTOR_H
#define COLORFEATUREEXTRACTOR_H

#include "featureextractor.h"

class ColorFeatureExtractor : public FeatureExtractor
{
    Q_OBJECT
public:
    explicit ColorFeatureExtractor(QObject *parent = 0);
    cv::Mat compute(QString imgPath);
};

#endif // COLORFEATUREEXTRACTOR_H
