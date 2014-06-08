#ifndef PENGUINVIEWER_H
#define PENGUINVIEWER_H

#include <QtQuick/QQuickPaintedItem>
#include <QPainter>
#include <QImage>
#include <QList>
#include <QTimer>

#include "violajonesclassifier.h"
#include "randomforest.h"
#include "tracker.h"

class PenguinViewer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(bool applyColorFilter READ applyColorFilter WRITE setApplyColorFilter)
    Q_PROPERTY(QString imagePath READ imagePath WRITE setImage)
    Q_PROPERTY(QString currentFPS READ currentFPS NOTIFY fpsChanged)

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
    QList< QVector<QPointF> > detectPenguins(cv::Mat videoFrame);
    QString currentFPS();

    PenguinViewer::SurveillanceStatus status() const;

public slots:
    bool applyColorFilter();
    void setApplyColorFilter(bool is);

    QString imagePath();
    void setImage(QString imagePath);

    void start();
    void updateFPS();

signals:
    void fpsChanged();
    void statusChanged();

private:
    QImage m_image;
    QString m_imagePath;
    QHash< QString, QList< QVector<QPointF> > > m_colorRects;
    bool m_applyColorFilter;

    ViolaJonesClassifier m_vjClassifier;
    RandomForest m_randomForest;
    QList<Tracker*> m_trackers;

    // For performance analysis
    QTimer m_timer;
    int m_frameCount;
    float m_currentFPS;

    SurveillanceStatus m_surveillanceStatus;
};

#endif // PENGUINVIEWER_H
