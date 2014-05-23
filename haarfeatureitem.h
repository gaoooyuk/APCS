#ifndef HAARFEATUREITEM_H
#define HAARFEATUREITEM_H

#include <QtQuick/QQuickPaintedItem>
#include <QPainter>
#include "feature.h"

class HaarFeatureItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int currentFeatureIndex READ currentFeatureIndex WRITE setCurrentFeatureIndex)

public:
    HaarFeatureItem(QQuickItem *parent = 0);
    HaarFeatureItem(const HaarFeatureItem &other);
    HaarFeatureItem& operator =(const HaarFeatureItem &other);

    void appendFeature(const Feature &feature);
    int featureNum() const;
    Feature getFeature(int index) const;

    int currentFeatureIndex() const;
    void setCurrentFeatureIndex(const int index);

    void paint(QPainter *painter);

public slots:
    void showPrevFeature();
    void showNextFeature();

private:
    QList<Feature> m_featuresList;
    int m_currentFeatureIndex;
};

#endif // HAARFEATUREITEM_H
