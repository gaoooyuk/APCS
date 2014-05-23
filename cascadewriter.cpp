#include "cascadewriter.h"
#include <QDebug>

CascadeWriter::CascadeWriter(QObject *parent) :
    QObject(parent)
{
    xml.setAutoFormatting(true);
}

bool CascadeWriter::write(QIODevice *device, CascadeObject cascadeObj)
{
    xml.setDevice(device);
    m_cascadeObj = cascadeObj;

    xml.writeStartDocument();
    xml.writeStartElement("opencv_storage");
    writeCascade();
    xml.writeEndElement();
    xml.writeEndDocument();
    return true;
}

void CascadeWriter::writeCascade()
{
    xml.writeStartElement("cascade");
    xml.writeTextElement("stageType", m_cascadeObj.stageType());
    xml.writeTextElement("featureType", m_cascadeObj.featureType());
    xml.writeTextElement("height", QString::number(m_cascadeObj.height()));
    xml.writeTextElement("width", QString::number(m_cascadeObj.width()));
    writeStageParams();
    writeFeatureParams();
    xml.writeTextElement("stageNum", QString::number(m_cascadeObj.stageNum()));
    writeStages();
    writeFeatures();
    xml.writeEndElement();
}

void CascadeWriter::writeStageParams()
{
    xml.writeStartElement("stageParams");
    xml.writeTextElement("boostType", m_cascadeObj.boostType());
    xml.writeTextElement("minHitRate", QString::number(m_cascadeObj.minHitRate()));
    xml.writeTextElement("maxFalseAlarm",QString::number( m_cascadeObj.maxFalseAlarm()));
    xml.writeTextElement("weightTrimRate", QString::number(m_cascadeObj.weightTrimRate()));
    xml.writeTextElement("maxDepth", QString::number(m_cascadeObj.maxDepth()));
    xml.writeTextElement("maxWeakCount", QString::number(m_cascadeObj.maxWeakCount()));
    xml.writeEndElement();
}

void CascadeWriter::writeFeatureParams()
{
    xml.writeStartElement("featureParams");
    xml.writeTextElement("maxCatCount", QString::number(m_cascadeObj.maxCatCount()));
    xml.writeTextElement("featSize", QString::number(m_cascadeObj.featSize()));
    xml.writeTextElement("mode", m_cascadeObj.mode());
    xml.writeEndElement();
}

void CascadeWriter::writeStages()
{
    xml.writeStartElement("stages");

    for (int i = 0; i < m_cascadeObj.stageNum(); i++)
    {
        QString stageStr = QString(" stage %1 ").arg(i);
        xml.writeComment(stageStr);

        Stage stage = m_cascadeObj.getStage(i);
        xml.writeStartElement("_");
        xml.writeTextElement("maxWeakCount", QString::number(stage.maxWeakCount()));
        xml.writeTextElement("stageThreshold", QString::number(stage.stageThreshold()));

        // write weak classifiers
        xml.writeStartElement("weakClassifiers");
        for (int j = 0; j < stage.maxWeakCount(); j++)
        {
            WeakClassifier wc = stage.getWeakClassifier(j);
            xml.writeStartElement("_");
            xml.writeTextElement("internalNodes", wc.internalNodes());
            xml.writeTextElement("leafValues", wc.leafValues());
            xml.writeEndElement();
        }
        xml.writeEndElement();

        xml.writeEndElement();
    }
    xml.writeEndElement();
}

void CascadeWriter::writeFeatures()
{
    xml.writeStartElement("features");
    for (int i = 0; i < m_cascadeObj.featureNum(); i++)
    {
        xml.writeStartElement("_");
        xml.writeStartElement("rects");
        Feature feature = m_cascadeObj.getFeature(i);
        for (int j = 0; j < feature.rectangleCount(); j++)
        {
            QRect rect = feature.getRectangle(j);
            int factor = feature.getRectangleFactor(j);
            QString str = QString("%1 %2 %3 %4 %5.").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height()).arg(factor);
            xml.writeTextElement("_", str);
        }
        xml.writeEndElement();
        xml.writeTextElement("titled", QString::number(feature.tilted()));
        xml.writeEndElement();
    }
    xml.writeEndElement();
}
