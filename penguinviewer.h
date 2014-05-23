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

public:
    PenguinViewer(QQuickItem *parent = 0);
    void paint(QPainter *painter);
    QList< QVector<QPointF> > detectPenguins(cv::Mat videoFrame);
    QString currentFPS();

public slots:
    bool applyColorFilter();
    void setApplyColorFilter(bool is);

    QString imagePath();
    void setImage(QString imagePath);

    void start();
    void updateFPS();

signals:
    void fpsChanged();

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

};

#endif // PENGUINVIEWER_H
