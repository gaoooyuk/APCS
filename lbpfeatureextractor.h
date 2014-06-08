#ifndef LBPFEATUREEXTRACTOR_H
#define LBPFEATUREEXTRACTOR_H

#include "featureextractor.h"

class LbpFeatureExtractor : public FeatureExtractor
{
    Q_OBJECT
public:
    explicit LbpFeatureExtractor(QObject *parent = 0);
    cv::Mat compute(QString imgPath);
};

#endif // LBPFEATUREEXTRACTOR_H