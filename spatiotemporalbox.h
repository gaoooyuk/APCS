#ifndef SPATIOTEMPORALBOX_H
#define SPATIOTEMPORALBOX_H

#include <QtQuick/QQuickPaintedItem>
#include <QPainter>

#include "utility.h"
#include "spatiotemporaldef.h"

class SpatioTemporalBox : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(float rotationX READ rotationX WRITE setRotationX NOTIFY rotationXChanged)
    Q_PROPERTY(float rotationY READ rotationY WRITE setRotationY NOTIFY rotationYChanged)

    Q_PROPERTY(bool isDrawLBPoints READ isDrawLBPoints WRITE setIsDrawLBPoints NOTIFY isDrawLBPointsChanged)
    Q_PROPERTY(bool isDrawSTPoints READ isDrawSTPoints WRITE setIsDrawSTPoints NOTIFY isDrawSTPointsChanged)
    Q_PROPERTY(bool isHighlightClusters READ isHighlightClusters WRITE setIsHighlightClusters NOTIFY isHighlightClustersChanged)
    Q_PROPERTY(bool isDrawFilteringSTPoints READ isDrawFilteringSTPoints WRITE setIsDrawFilteringSTPoints NOTIFY isDrawFilteringSTPointsChanged)
    Q_PROPERTY(bool isDrawClusterEigenLine READ isDrawClusterEigenLine WRITE setIsDrawClusterEigenLine NOTIFY isDrawClusterEigenLineChanged)

    Q_ENUMS(DisplayMode)

public:
    SpatioTemporalBox(QQuickItem *parent = 0);
    void paint(QPainter *painter);

    enum DisplayMode {
        DarkMode,
        BrightMode
    };

public slots:
    float rotationX();
    void setRotationX(const float angel);

    float rotationY();
    void setRotationY(const float angel);

    void zoomIn();
    void zoomOut();

    void timeScaleUp();
    void timeScaleDown();

    bool isDrawLBPoints() const;
    void setIsDrawLBPoints(const bool is);

    bool isDrawSTPoints() const;
    void setIsDrawSTPoints(const bool is);

    bool isHighlightClusters() const;
    void setIsHighlightClusters(const bool is);

    bool isDrawFilteringSTPoints() const;
    void setIsDrawFilteringSTPoints(const bool is);

    bool isDrawClusterEigenLine() const;
    void setIsDrawClusterEigenLine(const bool is);

    void updateSpatioTemporalPoints(QObject *stPoints);
    void updateSpatioTemporalFilteredPoints(QObject *stFilteredPoints);
    void updateSpatioTemporalCones(QObject *stCones);

    void updateSpatioTemporalLabelingPoints(QObject *lbPoints);
    void updateSpatioTemporalCluster(QObject *clusterInfo);
    void updateSpatioTemporalLineSegments(QObject *lineSegments);

    void loadSpatioTemporalLabelingPoints(const QString fileName);

    void changeDisplayMode(DisplayMode mode);

signals:
    void rotationXChanged();
    void rotationYChanged();
    void isDrawLBPointsChanged();
    void isDrawSTPointsChanged();
    void isHighlightClustersChanged();
    void isDrawFilteringSTPointsChanged();
    void isDrawClusterEigenLineChanged();

private:
    void init();
    void drawSTBox();
    void drawSTPoints();
    void drawSTClusters();
    void drawSTLineSegments();
    void drawSTCones();
    void drawClusterEigenLine();

private:
    float m_rotationX;
    float m_rotationY;
    float m_zoomValue;
    int m_timeScale;

    QList<STPoint> m_stPoints;
    QList<STPoint> m_stFilteredPoints;
    QList<STCone> m_stCones;
    QList<STPoint> m_lbPoints;  // Points of manual labeling

    QList<QColor> m_clusterColorList;
    QList<STCluster> m_clusters;
    QList<STLine> m_lineSegments;

    bool m_isDrawLBPoints;
    bool m_isDrawSTPoints;
    bool m_isHighlightClusters;
    bool m_isDrawFilteringSTPoints;
    bool m_isDrawClusterEigenLine;

    DisplayMode m_displayMode;
};

#endif // SPATIOTEMPORALBOX_H
