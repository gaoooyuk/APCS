#include "stage.h"

Stage::Stage(QObject *parent) :
    QObject(parent)
{
    m_maxWeakCount = 0;
    m_stageThreshold = 0;
}

Stage::Stage(const Stage &other)
{
    m_maxWeakCount = other.maxWeakCount();
    m_stageThreshold = other.stageThreshold();

    for (int i = 0; i < other.maxWeakCount(); i++)
        m_weakClassifiersList.append(other.getWeakClassifier(i));
}

Stage& Stage::operator =(const Stage &other)
{
    if(this == &other)
        return *this;

    m_maxWeakCount = other.maxWeakCount();
    m_stageThreshold = other.stageThreshold();

    for (int i = 0; i < other.maxWeakCount(); i++)
        m_weakClassifiersList.append(other.getWeakClassifier(i));

    return *this;
}

int Stage::maxWeakCount() const
{
    return m_maxWeakCount;
}

void Stage::setMaxWeakCount(const int maxWeakCount)
{
    if (m_maxWeakCount != maxWeakCount)
    {
        m_maxWeakCount = maxWeakCount;
    }
}

double Stage::stageThreshold() const
{
    return m_stageThreshold;
}

void Stage::setStageThreshold(const double stageThreshold)
{
    if (m_stageThreshold != stageThreshold)
    {
        m_stageThreshold = stageThreshold;
    }
}

WeakClassifier Stage::getWeakClassifier(int index) const
{
    if (index < m_weakClassifiersList.size())
        return m_weakClassifiersList.at(index);
    else
        return WeakClassifier();
}

void Stage::appendWeakClassifier(const WeakClassifier &wc)
{
    m_weakClassifiersList.append(wc);
}
