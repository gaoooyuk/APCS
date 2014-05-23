#include "penguinviewer.h"
#include <QColor>
#include <QHash>
#include <QImageReader>

cv::VideoCapture cap;

PenguinViewer::PenguinViewer(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    m_imagePath = "";
    m_applyColorFilter = false;
    m_randomForest.train("penguin_color.train", 100, 144);

    cap.open("penguin.wmv");
    if(!cap.isOpened())  // check if we succeeded
        qDebug() << "Camera failed.";

    m_frameCount = 0;
    m_currentFPS = 0.0f;

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateFPS()));

    m_trackers.clear();
}

QString PenguinViewer::imagePath()
{
    return m_imagePath;
}

void PenguinViewer::setImage(QString imagePath)
{
    if (m_imagePath == imagePath)
        return;

    m_imagePath = imagePath;

    m_image.load(imagePath, "JPEG");
    setWidth(m_image.width());
    setHeight(m_image.height());
    update();
}

void PenguinViewer::start()
{
    m_trackers.clear();
    cv::namedWindow("edges", 1);

    for(;;)
    {
        cv::Mat frame;
        cap >> frame; // get a new frame from camera or video

        frame.convertTo(frame, CV_8UC1);
        m_image = Utility::cvMatToQImage(frame);

        setWidth(m_image.width());
        setHeight(m_image.height());

        if (m_trackers.isEmpty())
        {
            QList< QVector<QPointF> > rects = detectPenguins(frame);
            foreach(QVector<QPointF> rect, rects)
            {
                // calculate the penguin centre
                double cx = 0.0f;
                double cy = 0.0f;
                for (int i = 0; i < rect.size(); i++)
                {
                    cx += rect.at(i).x();
                    cy += rect.at(i).y();
                }

                cx = cx / rect.size();
                cy = cy / rect.size();

//                qDebug() << "Penguin centre: (" << cx << "," << cy << ")";

                int width = abs(rect.at(0).x() - rect.at(2).x());
                int height = abs(rect.at(1).y() - rect.at(3).y());

                Tracker *tracker = new Tracker;
                tracker->setInitialState(cx, cy, width, height);
                m_trackers.append(tracker);
            }

            m_colorRects.insert("Q_COLOR_FRONTAL", rects);
        } else {
            Tracker *tracker = m_trackers.last();
            tracker->propagate(frame);
            if (tracker->isLost())
            {
                m_trackers.removeLast();
            }
        }

        update();

        if(cv::waitKey(10) >= 0)
            break;
    }
}

void PenguinViewer::updateFPS()
{
    m_currentFPS =  m_frameCount;
    m_frameCount = 0;

    emit fpsChanged();
}

void PenguinViewer::paint(QPainter *painter)
{
    if (m_image.isNull())
        return;

    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->drawImage(boundingRect(), m_image);

    for (int i = 0; i < m_colorRects.keys().size(); i++)
    {
        QString key = m_colorRects.keys().at(i);
        QList< QVector<QPointF> > rects = m_colorRects.value(key);

#if 0
        if ("Q_COLOR_FRONTAL" == key)
            qDebug() << "FRONTAL DETECTION: " << rects.size();
        else if ("Q_COLOR_90" == key)
            qDebug() << "DEGREE 90 DETECTION: " << rects.size();
        else if ("Q_COLOR_90_UPPER" == key)
            qDebug() << "DEGREE 90 UPPER DETECTION: " << rects.size();
        else if ("Q_COLOR_90_LOWER" == key)
            qDebug() << "DEGREE 90 LOWER DETECTION: " << rects.size();
        else if ("Q_COLOR_45" == key)
            qDebug() << "DEGREE 45 DETECTION: " << rects.size();
#endif

        painter->save();
        if ("Q_COLOR_FRONTAL" == key)
            painter->setPen(Q_COLOR_FRONTAL);
        else if ("Q_COLOR_90" == key)
            painter->setPen(Q_COLOR_90);
        else if ("Q_COLOR_90_UPPER" == key)
            painter->setPen(Q_COLOR_90_UPPER);
        else if ("Q_COLOR_90_LOWER" == key)
            painter->setPen(Q_COLOR_90_LOWER);
        else if ("Q_COLOR_45" == key)
            painter->setPen(Q_COLOR_45);

        foreach(QVector<QPointF> rect, rects)
        {
            painter->drawPolygon(rect.data(), 4);

            // calculate the penguin centre
            int x = 0;
            int y = 0;
            for (int i = 0; i < rect.size(); i++)
            {
                x += rect.at(i).x();
                y += rect.at(i).y();
            }

            x = x / rect.size();
            y = y / rect.size();

            int lineLen = 6;
            painter->drawLine(QPoint(x-lineLen, y), QPoint(x+lineLen, y));
            painter->drawLine(QPoint(x, y-lineLen), QPoint(x, y+lineLen));
        }
        painter->restore();
    }

//    foreach (Tracking* tracker, m_trackers)
    if (!m_trackers.isEmpty())
    {
        Tracker *tracker = m_trackers.last();
        if (!tracker->isLost())
        {
            QList<QPointF> trackingCandidates = tracker->getCandidates();
            painter->save();
            for (int j = 0; j < trackingCandidates.size(); j++)
            {
                QPointF p = trackingCandidates.at(j);
                painter->setPen(Qt::red);
                painter->drawPoint(p);
            }
            painter->restore();

            painter->save();
            painter->setPen(Qt::yellow);
            painter->setBrush(Qt::yellow);
            painter->setOpacity(0.2);
            QList<QPointF> trajectoryPoints = tracker->getTrajectory();
//            QList<QRect> trajectoryRects = tracker->getTrajectoryRects();
            qDebug() << "trajectory: " << trajectoryPoints;
            for (int i = 0; i < trajectoryPoints.size(); i++)
            {
//                painter->drawRect(trajectoryRects.at(i));
                painter->drawEllipse(trajectoryPoints.at(i), 20, 20);
//                painter->drawRect(tracker->getTrackedRectangle());
            }
            painter->restore();
        }
    }

    if (m_frameCount == 0)
    {
        m_timer.start(1000);
    }
    m_frameCount++;
}

QList< QVector<QPointF> > PenguinViewer::detectPenguins(cv::Mat videoFrame)
{
    return m_vjClassifier.detectPenguins(videoFrame);
}

QString PenguinViewer::currentFPS()
{
    return QString::number(m_currentFPS);
}

bool PenguinViewer::applyColorFilter()
{
    return m_applyColorFilter;
}

void PenguinViewer::setApplyColorFilter(bool is)
{
    if (m_applyColorFilter != is)
    {
        m_applyColorFilter = is;
        //        detectPenguins();
    }
}