#include "cascadeObject.h"

#define STAGE_TYPE_BOOST "BOOST"
#define FEATURE_TYPE_HAAR "HAAR"
#define BOOST_TYPE_GAB "GAB"
#define FEATURE_MODE_ALL "ALL"

CascadeObject::CascadeObject(QObject *parent) :
    QObject(parent)
{
    m_stageType = STAGE_TYPE_BOOST;
    m_featureType = FEATURE_TYPE_HAAR;
    m_height = 24;
    m_width = 24;
    m_stageNum = 0;

    m_boostType = BOOST_TYPE_GAB;
    m_minHitRate = 0;
    m_maxFalseAlarm = 0;
    m_weightTrimRate = 0;
    m_maxDepth = 0;
    m_maxWeakCount = 0;

    m_maxCatCount = 0;
    m_featSize = 0;
    m_mode = FEATURE_MODE_ALL;
}

CascadeObject::CascadeObject(const CascadeObject &other)
{
    m_stageType = other.stageType();
    m_featureType = other.featureType();
    m_height = other.height();
    m_width = other.width();
    m_stageNum = other.stageNum();

    m_boostType = other.boostType();
    m_minHitRate = other.minHitRate();
    m_maxFalseAlarm = other.maxFalseAlarm();
    m_weightTrimRate = other.weightTrimRate();
    m_maxDepth = other.maxDepth();
    m_maxWeakCount = other.maxWeakCount();

    m_maxCatCount = other.maxCatCount();
    m_featSize = other.featSize();
    m_mode = other.mode();

    for (int i = 0; i < other.stageNum(); i++)
        m_stagesList.append(other.getStage(i));

    for (int i = 0; i < other.featureNum(); i++)
        m_featuresList.append(other.getFeature(i));
}

CascadeObject& CascadeObject::operator =(const CascadeObject &other)
{
    if(this == &other)
        return *this;

    m_stageType = other.stageType();
    m_featureType = other.featureType();
    m_height = other.height();
    m_width = other.width();
    m_stageNum = other.stageNum();

    m_boostType = other.boostType();
    m_minHitRate = other.minHitRate();
    m_maxFalseAlarm = other.maxFalseAlarm();
    m_weightTrimRate = other.weightTrimRate();
    m_maxDepth = other.maxDepth();
    m_maxWeakCount = other.maxWeakCount();

    m_maxCatCount = other.maxCatCount();
    m_featSize = other.featSize();
    m_mode = other.mode();

    for (int i = 0; i < other.stageNum(); i++)
        m_stagesList.append(other.getStage(i));

    for (int i = 0; i < other.featureNum(); i++)
        m_featuresList.append(other.getFeature(i));

    return *this;
}

QString CascadeObject::stageType() const
{
    return m_stageType;
}

void CascadeObject::setStageType(const QString &type)
{
    if (m_stageType != type)
    {
        m_stageType = type;
    }
}

QString CascadeObject::featureType() const
{
    return m_featureType;
}

void CascadeObject::setFeatureType(const QString &type)
{
    if (m_featureType != type)
    {
        m_featureType = type;
    }
}

int CascadeObject::height() const
{
    return m_height;
}

void CascadeObject::setHeight(const int height)
{
    if (m_height != height)
    {
        m_height = height;
    }
}

int CascadeObject::width() const
{
    return m_width;
}

void CascadeObject::setWidth(const int width)
{
    if (m_width != width)
    {
        m_width = width;
    }
}

int CascadeObject::stageNum() const
{
    return m_stageNum;
}

void CascadeObject::setStageNum(const int stageNum)
{
    if (m_stageNum != stageNum)
    {
        m_stageNum = stageNum;
    }
}

QString CascadeObject::boostType() const
{
    return m_boostType;
}

void CascadeObject::setBoostType(const QString &type)
{
    if (m_boostType != type)
    {
        m_boostType = type;
    }
}

double CascadeObject::minHitRate() const
{
    return m_minHitRate;
}

void CascadeObject::setMinHitRate(const double minHitRate)
{
    if (m_minHitRate != minHitRate)
    {
        m_minHitRate = minHitRate;
    }
}

double CascadeObject::maxFalseAlarm() const
{
    return m_maxFalseAlarm;
}

void CascadeObject::setMaxFalseAlarm(const double maxFalseAlarm)
{
    if (m_maxFalseAlarm != maxFalseAlarm)
    {
        m_maxFalseAlarm = maxFalseAlarm;
    }
}

double CascadeObject::weightTrimRate() const
{
    return m_weightTrimRate;
}

void CascadeObject::setWeightTrimRate(const double weightTrimRate)
{
    if (m_weightTrimRate != weightTrimRate)
    {
        m_weightTrimRate = weightTrimRate;
    }
}

int CascadeObject::maxDepth() const
{
    return m_maxDepth;
}

void CascadeObject::setMaxDepth(const int maxDepth)
{
    if (m_maxDepth != maxDepth)
    {
        m_maxDepth = maxDepth;
    }
}

int CascadeObject::maxWeakCount() const
{
    return m_maxWeakCount;
}

void CascadeObject::setMaxWeakCount(const int maxWeakCount)
{
    if (m_maxWeakCount != maxWeakCount)
    {
        m_maxWeakCount = maxWeakCount;
    }
}

int CascadeObject::maxCatCount() const
{
    return m_maxCatCount;
}

void CascadeObject::setMaxCatCount(const int maxCatCount)
{
    if (m_maxCatCount != maxCatCount)
    {
        m_maxCatCount = maxCatCount;
    }
}

int CascadeObject::featSize() const
{
    return m_featSize;
}

void CascadeObject::setFeatSize(const int featSize)
{
    if (m_featSize != featSize)
    {
        m_featSize = featSize;
    }
}

QString CascadeObject::mode() const
{
    return m_mode;
}

void CascadeObject::setMode(const QString &mode)
{
    if (m_mode != mode)
    {
        m_mode = mode;
    }
}

Stage CascadeObject::getStage(int index) const
{
    if (index < m_stagesList.size())
        return m_stagesList.at(index);
    else
        return Stage();
}

void CascadeObject::setStage(const Stage &stage, const int index)
{
    if (index < m_stagesList.size())
        m_stagesList.replace(index, stage);
}

void CascadeObject::appendStage(const Stage &stage)
{
    m_stagesList.append(stage);
}

Feature CascadeObject::getFeature(int index) const
{
    if (index < m_featuresList.size())
        return m_featuresList.at(index);
    else
        return Feature();
}

void CascadeObject::setFeature(const Feature &feature, const int index)
{
    if (index < m_featuresList.size())
        m_featuresList.replace(index, feature);
}

void CascadeObject::appendFeature(const Feature &feature)
{
    m_featuresList.append(feature);
}

int CascadeObject::featureNum() const
{
    return m_featuresList.size();
}

void CascadeObject::prune(int percentage)
{
    if (percentage >= 100)
        return;

    for (int i = 0; i < stageNum(); i++)
    {
        Stage stage = m_stagesList.at(i);
        double stageThreshold = stage.stageThreshold();
        if (stageThreshold > 0)
            stageThreshold = stageThreshold * (100 - percentage) / 100;
        else
            stageThreshold = stageThreshold + stageThreshold * percentage / 100;

        stage.setStageThreshold(stageThreshold);
        m_stagesList.replace(i, stage);
    }
}
