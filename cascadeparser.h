#ifndef CASCADEPARSER_H
#define CASCADEPARSER_H

#include <QObject>
#include <QXmlStreamReader>
#include "cascadeobject.h"

class CascadeParser : public QObject
{
    Q_OBJECT
public:
    explicit CascadeParser(QObject *parent = 0);
    bool read(QIODevice *device);
    CascadeObject getCascadeObject();

private:
    void readCascade();
    void readStageParams();
    void readStages();
    void readFeatureParams();
    void readFeatures();
    void readSingleStage();
    void readSingleFeature();

    QString errorString() const;

private:
    QXmlStreamReader xml;
    CascadeObject m_cascadeObj;
};

#endif // CASCADEPARSER_H
