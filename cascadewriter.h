#ifndef CASCADEWRITER_H
#define CASCADEWRITER_H

#include <QObject>
#include <QXmlStreamWriter>
#include "cascadeobject.h"

class CascadeWriter : public QObject
{
    Q_OBJECT
public:
    explicit CascadeWriter(QObject *parent = 0);
    bool write(QIODevice *device, CascadeObject cascadeObj);
    QString errorString() const;

private:
    void writeCascade();
    void writeStageParams();
    void writeFeatureParams();
    void writeStages();
    void writeFeatures();

private:
    QXmlStreamWriter xml;
    CascadeObject m_cascadeObj;
};

#endif // CASCADEWRITER_H
