#ifndef SPATIOFRAME_H
#define SPATIOFRAME_H

#include <QtQuick/QQuickPaintedItem>
#include <QPainter>
#include <QVariantList>

class SpatioFrame : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QObject* trackingPoints READ trackingPoints WRITE importParticlePositions)
    Q_PROPERTY(QObject* trackingWeights READ trackingWeights WRITE importParticleWeights)

    Q_PROPERTY(QObject* detectionPoints READ detectionPoints WRITE importDetectionPositions)
    Q_PROPERTY(QObject* detectionWeights READ detectionWeights WRITE importDetectionWeights)

    Q_PROPERTY(QObject* detectionLabelColors READ detectionLabelColors WRITE importDetectionLabelColors)

    Q_ENUMS(PlotType)

public:
    SpatioFrame(QQuickItem *parent = 0);

    enum PlotType {
        PlotType_Particle,
        PlotType_Detection
    };

    void paint(QPainter *painter);

public slots:
    // trackingPoints are particles
    QObject* trackingPoints();
    QObject* trackingWeights();

    void importParticlePositions(QObject *trackingPoints);
    void importParticleWeights(QObject *trackingWeights);

    // detectionPoints are centre of detections
    QObject* detectionPoints();
    QObject* detectionWeights();

    void importDetectionPositions(QObject *detectionPoints);
    void importDetectionWeights(QObject *detectionWeights);

    QObject* detectionLabelColors();
    void importDetectionLabelColors(QObject *detectionLabelColors);

    void changePlotType(PlotType type);

private:
    QList<QPointF> m_trackingPoints;
    QList<double> m_trackingWeights;

    QList<QPointF> m_detectionPoints;
    QList<double> m_detectionWeights;

    QList<QColor> m_detectionLabelColors;

    PlotType m_plotType;
};

#endif // SPATIOFRAME_H
