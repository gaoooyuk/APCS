#ifndef PENGUINVIEWER_H
#define PENGUINVIEWER_H

#include <QtQuick/QQuickPaintedItem>
#include <QPainter>
#include <QImage>
#include <QList>
#include <QTimer>
#include <QVariantList>

#include "violajonesclassifier.h"
#include "randomforest.h"
#include "tracker.h"

class PenguinViewer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(bool applyColorFilter READ applyColorFilter WRITE setApplyColorFilter NOTIFY applyColorFilterChanged)
    Q_PROPERTY(bool saveDetectedPenguins READ saveDetectedPenguins WRITE setSaveDetectedPenguins NOTIFY saveDetectedPenguinsChanged)
    Q_PROPERTY(bool showConfidenceMap READ showConfidenceMap WRITE setShowConfidenceMap NOTIFY showConfidenceMapChanged)
    Q_PROPERTY(bool showSMCProcess READ showSMCProcess WRITE setShowSMCProcess NOTIFY showSMCProcessChanged)
    Q_PROPERTY(bool showTrajectories READ showTrajectories WRITE setShowTrajectories NOTIFY showTrajectoriesChanged)
    Q_PROPERTY(bool enableInspection READ enableInspection WRITE setEnableInspection NOTIFY enableInspectionChanged)

    Q_PROPERTY(QString imagePath READ imagePath WRITE setImage)
    Q_PROPERTY(QString currentFPS READ currentFPS NOTIFY fpsChanged)
    Q_PROPERTY(QString estimatedPenguinAmount READ estimatedPenguinAmount NOTIFY estimatedPenguinAmountChanged)
    Q_PROPERTY(int currentFrameNo READ currentFrameNo NOTIFY currentFrameNoChanged)

    Q_ENUMS(SurveillanceStatus)
    Q_PROPERTY(SurveillanceStatus status READ status NOTIFY statusChanged)

public:
    PenguinViewer(QQuickItem *parent = 0);
    ~PenguinViewer();

    enum SurveillanceStatus {
        Idel,
        Buffering,
        Running,
        Error
    };

    void paint(QPainter *painter);
    ViolaJonesClassifier::VJDetection detectPenguins(cv::Mat videoFrame);
    QString currentFPS();
    QString estimatedPenguinAmount();
    PenguinViewer::SurveillanceStatus status() const;

public slots:
    bool applyColorFilter() const;
    void setApplyColorFilter(bool is);

    bool saveDetectedPenguins() const;
    void setSaveDetectedPenguins(bool is);

    bool showConfidenceMap() const;
    void setShowConfidenceMap(bool is);

    bool showSMCProcess() const;
    void setShowSMCProcess(bool is);

    bool showTrajectories() const;
    void setShowTrajectories(bool is);

    bool enableInspection() const;
    void setEnableInspection(bool is);

    QString imagePath();
    void setImage(QString imagePath);

    void start(QString videoFile);
    void updateFPS();
    int currentFrameNo() const;

    void setVJClassifierParams(double scaleFactor,
                               int minNeighbours,
                               int minWidth,
                               int minHeight);

    void addTrackingManualLabeling(float x, float y, int frameNo);
    bool saveTrackingManualLabelings(QString savedFileName);

signals:
    void applyColorFilterChanged();
    void saveDetectedPenguinsChanged();
    void showConfidenceMapChanged();
    void showSMCProcessChanged();
    void showTrajectoriesChanged();
    void enableInspectionChanged();

    void detectedPenguinImageSaved(QString savedPath);
    void fpsChanged();
    void estimatedPenguinAmountChanged();
    void currentFrameNoChanged();
    void statusChanged();
    void newSpatioInfoAvailable(QObject *points,
                                QObject *weights,
                                QObject *dPoints,
                                QObject *dWeights);

    void pointsAvailable(QObject *stPoints);
    void filteredPointsAvailable(QObject *stFilteredPoints);
    void conesAvailable(QObject *stCones);
    void labelingPointsAvailable(QObject *lbPoints);
    void clusterInfoAvailable(QObject *clusterInfo);
    void lineSegmentsAvailable(QObject *lineSegments);

private:
    QImage m_image;
    QString m_imagePath;
    bool m_applyColorFilter;
    bool m_saveDetectedPenguins;
    bool m_showConfidenceMap;
    bool m_showSMCProcess;
    bool m_showTrajectories;
    bool m_enableInspection;
    bool m_saveManualLabelings;

    ViolaJonesClassifier m_vjClassifier;
    RandomForest m_randomForest;
    Tracker m_trackingSystem;

    ViolaJonesClassifier::VJDetection m_detections;

    // For performance analysis
    QTimer m_timer;
    int m_currentFrameNo;
    int m_frameCount;
    float m_currentFPS;
    int m_estimatedPenguinAmount;
    int m_skipAmount;

    SurveillanceStatus m_surveillanceStatus;
};

#endif // PENGUINVIEWER_H
