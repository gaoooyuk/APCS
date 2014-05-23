#include "feature.h"

Feature::Feature(QObject *parent) :
    QObject(parent)
{
    m_tilted = 0;
    m_rectanglesList.clear();
    m_rectangleFactorsList.clear();
}

Feature::Feature(const Feature &other)
{
    m_tilted = other.tilted();

    for (int i = 0; i < other.rectangleCount(); i++)
    {
        m_rectanglesList.append(other.getRectangle(i));
        m_rectangleFactorsList.append(other.getRectangleFactor(i));
    }
}

Feature& Feature::operator =(const Feature &other)
{
    if(this == &other)
        return *this;

    m_tilted = other.tilted();

    for (int i = 0; i < other.rectangleCount(); i++)
    {
        m_rectanglesList.append(other.getRectangle(i));
        m_rectangleFactorsList.append(other.getRectangleFactor(i));
    }

    return *this;
}

int Feature::tilted() const
{
    return m_tilted;
}

void Feature::setTilted(const int tilted)
{
    if (m_tilted != tilted)
    {
        m_tilted = tilted;
    }
}

QRect Feature::getRectangle(int index) const
{
    if (index < m_rectanglesList.size())
        return m_rectanglesList.at(index);
    else
        return QRect();
}

void Feature::appendRectangle(const QRect &rect)
{
    m_rectanglesList.append(rect);
}

int Feature::rectangleCount() const
{
    return m_rectanglesList.size();
}

void Feature::appendRectangleFactor(int factor)
{
    m_rectangleFactorsList.append(factor);
}

int Feature::getRectangleFactor(int index) const
{
    if (index < m_rectangleFactorsList.size())
        return m_rectangleFactorsList.at(index);
    else
        return 0;
}
