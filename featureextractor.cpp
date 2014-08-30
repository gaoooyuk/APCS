#include "featureextractor.h"

FeatureExtractor::FeatureExtractor(QObject *parent) :
    QObject(parent)
{
}

QVector<int> FeatureExtractor::featureVector() const
{
    return m_featureVector;
}
