#include "spatioframe.h"

SpatioFrame::SpatioFrame(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    m_plotType = SpatioFrame::PlotType_Particle;
}

void SpatioFrame::paint(QPainter *painter)
{
    if (m_trackingPoints.length() != m_trackingWeights.length())
        return;

    if (m_detectionPoints.length() != m_detectionWeights.length())
        return;

    if (m_detectionPoints.length() != m_detectionLabelColors.length())
        return;

    painter->setRenderHints(QPainter::Antialiasing, true);

    painter->save();

    QList<int> hightProbParticlesIdx;
    QPen pen;
    pen.setWidth(2);

    if (SpatioFrame::PlotType_Particle == m_plotType)
    {
        for (int j = 0; j < m_trackingPoints.size(); j++)
        {
            QPointF pos = m_trackingPoints.at(j);
            pos /= 4;
            double weight = m_trackingWeights.at(j);
            if (weight > 0.6)
            {
                hightProbParticlesIdx.append(j);
            } else {
                pen.setColor(Qt::blue);
                painter->setPen(pen);
                painter->drawPoint(pos);
            }
        }

        // draw particles with high probability
        foreach (int idx, hightProbParticlesIdx)
        {
            QPointF pos = m_trackingPoints.at(idx);
            pos /= 4;
            pen.setColor(Qt::red);
            painter->setPen(pen);
            painter->drawPoint(pos);
        }
    } else if (SpatioFrame::PlotType_Detection == m_plotType) {

        for (int j = 0; j < m_detectionPoints.size(); j++)
        {
            QPointF pos = m_detectionPoints.at(j);
            pos /= 4;

            QColor color = m_detectionLabelColors.at(j);
            pen.setColor(color);
            painter->setPen(pen);
            painter->drawPoint(pos);
        }
    }

    painter->restore();
}

QObject* SpatioFrame::trackingPoints()
{
    QVariantList vl;
    foreach (QPointF p, m_trackingPoints)
    {
        vl.append(QVariant::fromValue(p));
    }

    QObject *o = new QObject;
    o->setProperty("points", vl);
    return o;
}

QObject* SpatioFrame::trackingWeights()
{
    QVariantList vl;
    foreach (double w, m_trackingWeights)
    {
        vl.append(QVariant::fromValue(w));
    }

    QObject *o = new QObject;
    o->setProperty("weights", vl);
    return o;
}

void SpatioFrame::importParticlePositions(QObject *trackingPoints)
{
    m_trackingPoints.clear();
    QVariantList vl = trackingPoints->property("points").toList();

    foreach (QVariant var, vl)
    {
        m_trackingPoints.append(var.toPointF());
    }

    update();
}

void SpatioFrame::importParticleWeights(QObject *trackingWeights)
{
    m_trackingWeights.clear();
    QVariantList vl = trackingWeights->property("weights").toList();

    foreach (QVariant var, vl)
    {
        m_trackingWeights.append(var.toDouble());
    }

    update();
}

QObject* SpatioFrame::detectionPoints()
{
    QVariantList vl;
    foreach (QPointF p, m_detectionPoints)
    {
        vl.append(QVariant::fromValue(p));
    }

    QObject *o = new QObject;
    o->setProperty("dPoints", vl);
    return o;
}

QObject* SpatioFrame::detectionWeights()
{
    QVariantList vl;
    foreach (double w, m_detectionWeights)
    {
        vl.append(QVariant::fromValue(w));
    }

    QObject *o = new QObject;
    o->setProperty("dWeights", vl);
    return o;
}

void SpatioFrame::importDetectionPositions(QObject *detectionPoints)
{
    m_detectionPoints.clear();
    QVariantList vl = detectionPoints->property("dPoints").toList();

    foreach (QVariant var, vl)
    {
        m_detectionPoints.append(var.toPointF());
    }

    update();
}

void SpatioFrame::importDetectionWeights(QObject *detectionWeights)
{
    m_detectionWeights.clear();
    QVariantList vl = detectionWeights->property("dWeights").toList();

    foreach (QVariant var, vl)
    {
        m_detectionWeights.append(var.toDouble());
    }

    update();
}

QObject* SpatioFrame::detectionLabelColors()
{
    QVariantList vl;
    foreach (QColor c, m_detectionLabelColors)
    {
        vl.append(c);
    }

    QObject *o = new QObject;
    o->setProperty("dColors", vl);
    return o;
}

void SpatioFrame::importDetectionLabelColors(QObject *detectionLabelColors)
{
    m_detectionLabelColors.clear();
    QVariantList vl = detectionLabelColors->property("dColors").toList();

    foreach (QVariant var, vl)
    {
        m_detectionLabelColors.append(var.value<QColor>());
    }

    update();
}

void SpatioFrame::changePlotType(PlotType type)
{
    if (type != m_plotType)
    {
        m_plotType = type;
        update();
    }
}
