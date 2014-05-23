#ifndef CASCADEOBJECT_H
#define CASCADEOBJECT_H

#include <QObject>
#include <QList>
#include "stage.h"
#include "feature.h"

class CascadeObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString stageType READ stageType WRITE setStageType)
    Q_PROPERTY(QString featureType READ featureType WRITE setFeatureType)
    Q_PROPERTY(int height READ height WRITE setHeight)
    Q_PROPERTY(int width READ width WRITE setWidth)
    Q_PROPERTY(int stageNum READ stageNum WRITE setStageNum)

    Q_PROPERTY(QString boostType READ boostType WRITE setBoostType)

public:
    explicit CascadeObject(QObject *parent = 0);
    CascadeObject(const CascadeObject &other);
    CascadeObject& operator =(const CascadeObject &other);

    QString stageType() const;
    void setStageType(const QString &type);

    QString featureType() const;
    void setFeatureType(const QString &type);

    int height() const;
    void setHeight(const int height);

    int width() const;
    void setWidth(const int width);

    int stageNum() const;
    void setStageNum(const int stageNum);

    // stageParams start
    QString boostType() const;
    void setBoostType(const QString &type);

    double minHitRate() const;
    void setMinHitRate(const double minHitRate);

    double maxFalseAlarm() const;
    void setMaxFalseAlarm(const double maxFalseAlarm);

    double weightTrimRate() const;
    void setWeightTrimRate(const double weightTrimRate);

    int maxDepth() const;
    void setMaxDepth(const int maxDepth);

    int maxWeakCount() const;
    void setMaxWeakCount(const int maxWeakCount);
    // stageParams end

    // featureParams start
    int maxCatCount() const;
    void setMaxCatCount(const int maxCatCount);

    int featSize() const;
    void setFeatSize(const int featSize);

    QString mode() const;
    void setMode(const QString &mode);
    // featureParams end

    Stage getStage(int index) const;
    void setStage(const Stage &stage, const int index);
    void appendStage(const Stage &stage);

    Feature getFeature(int index) const;
    void setFeature(const Feature &feature, const int index);
    void appendFeature(const Feature &feature);
    int featureNum() const;

    void prune(int percentage);

private:
    QString m_stageType;
    QString m_featureType;
    int m_height;
    int m_width;
    int m_stageNum;

    // stageParams
    QString m_boostType;
    double  m_minHitRate;
    double  m_maxFalseAlarm;
    double  m_weightTrimRate;
    int m_maxDepth;
    int m_maxWeakCount;

    // featureParams
    int m_maxCatCount;
    int m_featSize;
    QString m_mode;

    // stage object
    QList<Stage> m_stagesList;

    // feature object
    QList<Feature> m_featuresList;
};

#endif // CASCADEOBJECT_H
