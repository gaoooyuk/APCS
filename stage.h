#ifndef STAGE_H
#define STAGE_H

#include <QObject>
#include <QList>
#include "weakclassifier.h"

class Stage : public QObject
{
    Q_OBJECT
public:
    explicit Stage(QObject *parent = 0);
    Stage(const Stage &other);
    Stage& operator =(const Stage &other);

    int maxWeakCount() const;
    void setMaxWeakCount(const int maxWeakCount);

    double stageThreshold() const;
    void setStageThreshold(const double stageThreshold);

    WeakClassifier getWeakClassifier(int index) const;
    void appendWeakClassifier(const WeakClassifier &wc);

private:
    int m_maxWeakCount;
    double m_stageThreshold;
    QList<WeakClassifier> m_weakClassifiersList;
};

#endif // STAGE_H
