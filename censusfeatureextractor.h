#ifndef CENSUSFEATUREEXTRACTOR_H
#define CENSUSFEATUREEXTRACTOR_H

#include "featureextractor.h"

class CensusFeatureExtractor : public FeatureExtractor
{
    Q_OBJECT
public:
    explicit CensusFeatureExtractor(QObject *parent = 0);
    cv::Mat compute(QString imgPath);

private:
    void census3x3(unsigned char* in, unsigned char* out, int w, int h);
};

#endif // CENSUSFEATUREEXTRACTOR_H
