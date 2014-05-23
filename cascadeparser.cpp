#include "cascadeparser.h"
#include <QDebug>

CascadeParser::CascadeParser(QObject *parent) :
    QObject(parent)
{
}

bool CascadeParser::read(QIODevice *device)
{
//    qDebug() << "Start parsing cascade xml file -->";
    xml.setDevice(device);

    while (xml.readNextStartElement())
    {
        if (xml.name() == "cascade")
            readCascade();
    }

//    qDebug() << "<-- End parsing cascade xml file";
    return !xml.error();
}

CascadeObject CascadeParser::getCascadeObject()
{
    return m_cascadeObj;
}

void CascadeParser::readCascade()
{
//    qDebug() << "readCascade -->";
    while (xml.readNextStartElement())
    {
        if (xml.name() == "stageType")
            m_cascadeObj.setStageType(xml.readElementText());
        else if (xml.name() == "featureType")
            m_cascadeObj.setFeatureType(xml.readElementText());
        else if (xml.name() == "width")
            m_cascadeObj.setWidth(xml.readElementText().toInt());
        else if (xml.name() == "height")
            m_cascadeObj.setHeight(xml.readElementText().toInt());
        else if (xml.name() == "stageNum")
            m_cascadeObj.setStageNum(xml.readElementText().toInt());
        else if (xml.name() == "stageParams")
            readStageParams();
        else if (xml.name() == "featureParams")
            readFeatureParams();
        else if (xml.name() == "stages")
            readStages();
        else if (xml.name() == "features")
            readFeatures();
        else
            xml.skipCurrentElement();
    }
//    qDebug() << "<-- readCascade";
}

void CascadeParser::readStageParams()
{
//    qDebug() << "<stageParams>";
    Q_ASSERT(xml.isStartElement() && xml.name() == "stageParams");

    while (xml.readNextStartElement())
    {
        if (xml.name() == "boostType")
            m_cascadeObj.setBoostType(xml.readElementText());
        else if (xml.name() == "minHitRate")
            m_cascadeObj.setMinHitRate(xml.readElementText().toDouble());
        else if (xml.name() == "maxFalseAlarm")
            m_cascadeObj.setMaxFalseAlarm(xml.readElementText().toDouble());
        else if (xml.name() == "weightTrimRate")
            m_cascadeObj.setWeightTrimRate(xml.readElementText().toDouble());
        else if (xml.name() == "maxDepth")
            m_cascadeObj.setMaxDepth(xml.readElementText().toInt());
        else if (xml.name() == "maxWeakCount")
            m_cascadeObj.setMaxWeakCount(xml.readElementText().toInt());
        else
            xml.skipCurrentElement();
    }
//    qDebug() << "</stageParams>";
}

void CascadeParser::readStages()
{
//    qDebug() << "<stages>";
    Q_ASSERT(xml.isStartElement() && xml.name() == "stages");

    while (xml.readNextStartElement())
    {
        if (xml.name() == "_")
            readSingleStage();
        else
            xml.skipCurrentElement();
    }
//    qDebug() << "</stages>";
}

void CascadeParser::readFeatureParams()
{
//    qDebug() << "<featureParams>";
    Q_ASSERT(xml.isStartElement() && xml.name() == "featureParams");

    while (xml.readNextStartElement())
    {
        if (xml.name() == "maxCatCount")
            m_cascadeObj.setMaxCatCount(xml.readElementText().toInt());
        else if (xml.name() == "featSize")
            m_cascadeObj.setFeatSize(xml.readElementText().toInt());
        else if (xml.name() == "mode")
            m_cascadeObj.setMode(xml.readElementText());
        else
            xml.skipCurrentElement();
    }
//    qDebug() << "</featureParams>";
}

void CascadeParser::readFeatures()
{
//    qDebug() << "<features>";
    Q_ASSERT(xml.isStartElement() && xml.name() == "features");

    while (xml.readNextStartElement())
    {
        if (xml.name() == "_")
            readSingleFeature();
        else
            xml.skipCurrentElement();
    }
//    qDebug() << "</features>";
}

void CascadeParser::readSingleStage()
{
//    qDebug() << "<stage>";
    Q_ASSERT(xml.isStartElement() && xml.name() == "_");

    Stage stage;
    while (xml.readNextStartElement())
    {
        if (xml.name() == "maxWeakCount")
            stage.setMaxWeakCount(xml.readElementText().toInt());
        else if (xml.name() == "stageThreshold")
            stage.setStageThreshold(xml.readElementText().toDouble());
        else if (xml.name() == "weakClassifiers")
        {
            while (xml.readNextStartElement())
            {
                if (xml.name() == "_")
                {
                    WeakClassifier wc;
                    while (xml.readNextStartElement())
                    {
                        if (xml.name() == "internalNodes")
                            wc.setInternalNodes(xml.readElementText().trimmed());
                        else if (xml.name() == "leafValues")
                            wc.setLeafValues(xml.readElementText().trimmed());
                        else
                            xml.skipCurrentElement();
                    }
                    stage.appendWeakClassifier(wc);
                }
                else
                    xml.skipCurrentElement();
            }
        }
        else
            xml.skipCurrentElement();
    }
    m_cascadeObj.appendStage(stage);
//    qDebug() << "</stage>";
}

void CascadeParser::readSingleFeature()
{
//    qDebug() << "<feature>";
    Q_ASSERT(xml.isStartElement() && xml.name() == "_");

    Feature feature;
    while (xml.readNextStartElement())
    {
        if (xml.name() == "rects")
        {
            while (xml.readNextStartElement())
            {
                if (xml.name() == "_")
                {
                    QString rectStr = xml.readElementText().remove(".");
                    QStringList rectParams = rectStr.split(" ", QString::SkipEmptyParts);
                    rectParams.removeFirst();

                    int x = rectParams.takeFirst().toInt();
                    int y = rectParams.takeFirst().toInt();
                    int width = rectParams.takeFirst().toInt();
                    int height = rectParams.takeFirst().toInt();
                    int factor = rectParams.takeFirst().toInt();
                    QRect rect;
                    rect.setTopLeft(QPoint(x,y));
                    rect.setWidth(width);
                    rect.setHeight(height);
                    feature.appendRectangle(rect);
                    feature.appendRectangleFactor(factor);
                }
                else
                    xml.skipCurrentElement();
            }
        }
        else if (xml.name() == "tilted")
            feature.setTilted(xml.readElementText().toInt());
        else
            xml.skipCurrentElement();
    }

    m_cascadeObj.appendFeature(feature);
//    qDebug() << "</feature>";
}

QString CascadeParser::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}
