#include "haarfeatureitem.h"

HaarFeatureItem::HaarFeatureItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    m_currentFeatureIndex = 0;
}

HaarFeatureItem::HaarFeatureItem(const HaarFeatureItem &other)
{
    for (int i = 0; i < other.featureNum(); i++)
        m_featuresList.append(other.getFeature(i));

    m_currentFeatureIndex = other.currentFeatureIndex();
}

HaarFeatureItem& HaarFeatureItem::operator =(const HaarFeatureItem &other)
{
    if(this == &other)
        return *this;

    for (int i = 0; i < other.featureNum(); i++)
        m_featuresList.append(other.getFeature(i));

    m_currentFeatureIndex = other.currentFeatureIndex();

    return *this;
}

void HaarFeatureItem::appendFeature(const Feature &feature)
{
    m_featuresList.append(feature);
    update();
}

int HaarFeatureItem::featureNum() const
{
    return m_featuresList.size();
}

Feature HaarFeatureItem::getFeature(int index) const
{
    if (index < m_featuresList.size())
        return m_featuresList.at(index);
    else
        return Feature();
}

int HaarFeatureItem::currentFeatureIndex() const
{
    return m_currentFeatureIndex;
}

void HaarFeatureItem::setCurrentFeatureIndex(const int index)
{
    if (m_currentFeatureIndex != index)
    {
        int idx;
        if (index < 0)
            idx = featureNum() - 1;
        else if (index > featureNum() - 1)
            idx = 0;
        else
            idx = index;

        m_currentFeatureIndex = idx;
    }
}

void HaarFeatureItem::showPrevFeature()
{
    setCurrentFeatureIndex(currentFeatureIndex() - 1);
    update();
}

void HaarFeatureItem::showNextFeature()
{
    setCurrentFeatureIndex(currentFeatureIndex() + 1);
    update();
}

void HaarFeatureItem::paint(QPainter *painter)
{
    if (0 == m_featuresList.size())
        return;

//    painter->setOpacity(0.8);
    painter->setRenderHints(QPainter::Antialiasing, true);

    QColor color;
    Feature feature = m_featuresList.at(m_currentFeatureIndex);
    for (int j = 0; j < feature.rectangleCount(); j++)
    {
        QRect rect = feature.getRectangle(j);
        QRect rectScaled(rect.x() * 10, rect.y() * 10, rect.width() * 10, rect.height() * 10);
        if (0 == j)
            color = Qt::white;
        else
            color = Qt::black;
        painter->fillRect(rectScaled, color);
        painter->drawRect(rectScaled);
    }

    painter->drawRect(boundingRect());
}
