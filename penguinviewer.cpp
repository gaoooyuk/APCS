#include "penguinviewer.h"
#include <QColor>
#include <QHash>
#include <QImageReader>
#include <QElapsedTimer>
#include <QDateTime>

#include "trainingsystem.h"
#include "colorfeatureextractor.h"

#define CV_WINDOW_NAME "penguinVideoWindow"

cv::VideoCapture cap;


PenguinViewer::PenguinViewer(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    m_imagePath = "";
    m_applyColorFilter = false;

//    cap.open("./videos/non_penguin/2.mp4");
    cap.open("./videos/penguin/penguin1.mp4");
    if(!cap.isOpened())  // check if we succeeded
        qDebug() << "Camera failed.";

    cv::namedWindow(CV_WINDOW_NAME, 1);
    cv::resizeWindow(CV_WINDOW_NAME, 0, 0);

    m_frameCount = 0;
    m_currentFPS = 0.0f;

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateFPS()));

    m_trackers.clear();

    m_vjClassifier.setParams(1.01, 1, QSize(20, 60));
    m_surveillanceStatus = PenguinViewer::Idel;
}

PenguinViewer::~PenguinViewer()
{
    // TODO: this deconstructor doesn't call
    cv::destroyWindow(CV_WINDOW_NAME);
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
    // Load and training random forest classifier
    m_randomForest.train("cb_classifier_8x24/cb.classifier", 5900, 576);

    m_surveillanceStatus = PenguinViewer::Running;
    emit statusChanged();

    m_trackers.clear();
    m_detectedPenguinImageList.clear();
    int num_detected = 0;

    //#define ABC

#ifdef ABC
    QStringList imgs = TrainingSystem::getAllFilesOfDir("/MyApps/build-APCS-Desktop/APCS.app/Contents/MacOS/test");
    //        QStringList imgs = TrainingSystem::getAllFilesOfDir("/Users/apple/Desktop/Courses/Penguin/training_images/negatives");

    qDebug() << "Num of imgs: " << imgs.length();
    for (int num = 0; num < imgs.length(); num++)
#else
    while (1)
#endif
    {
#ifdef ABC
        cv::Mat frame = cv::imread(imgs.at(num).toUtf8().data(), CV_LOAD_IMAGE_UNCHANGED);   // Read the file
        if (!frame.data)
        {
            qDebug() <<  "Could not open or find the image.";
        }
#else
        cv::Mat frame;
        bool ret = cap.read(frame);    // get a new frame from camera or video
        if (!ret)
            break;
#endif

        frame.convertTo(frame, CV_8UC1);
        m_image = Utility::cvMatToQImage(frame);

        setWidth(m_image.width());
        setHeight(m_image.height());

        m_colorRects.clear();

        QList< QVector<QPointF> > detections = detectPenguins(frame);
        if (!detections.isEmpty())
        {
            num_detected += detections.length();

            m_colorRects.insert("Q_COLOR_FRONTAL", detections);

            QDateTime dateTime;
            QList<QPointF> detectionCentroids;
            for (int i = 0; i < detections.size(); i++)
            {
                QVector<QPointF> rect = detections.at(i);

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

                detectionCentroids.append(QPointF(cx, cy));

                //                int p_width = abs(rect.at(0).x() - rect.at(2).x());
                //                int p_height = abs(rect.at(1).y() - rect.at(3).y());

                bool saveDetectedPenguins = false;
                if (saveDetectedPenguins)
                {
                    QRect roi(rect.at(1).toPoint(), rect.at(3).toPoint());
                    QImage detectedPenguinImage = m_image.copy(roi);
                    QString timeFormat = QString("yyyy-MM-dd-hh-mm-ss-zzz-%1.jpg").arg(QString::number(i+1));
                    QString savedImgName = dateTime.currentDateTime().toString(timeFormat);
                    QString savedPath = QDir::currentPath() + QDir::separator() + QString("detected") + QDir::separator() + savedImgName;
                    bool saved = detectedPenguinImage.save(savedPath, "JPEG");
                    if (!saved)
                    {
                        qDebug() << "Can't save image " << savedPath;
                    } else {
                        m_detectedPenguinImageList.append(savedPath);
                        emit detectedPenguinImageListChanged();
                    }
                }
            }

            if (m_trackers.isEmpty())
            {
                Tracker *tracker = new Tracker;
                tracker->setFrameSize(width(), height());
                //                tracker->setInitialState(cx, cy, p_width, p_height);
                m_trackers.append(tracker);

            } else {
                Tracker *tracker = m_trackers.first();
                tracker->propagate(detectionCentroids);

                //            if (tracker->isLost())
                //            {
                //                m_trackers.removeLast();
                //            }
            }
        }

        update();

        if(cv::waitKey(30) >= 0)
            break;
    }

    m_surveillanceStatus = PenguinViewer::Idel;
    emit statusChanged();
    qDebug() << "Num of detections: " << num_detected;
}

void PenguinViewer::updateFPS()
{
    m_currentFPS =  m_frameCount;
    m_frameCount = 0;

    emit fpsChanged();
}

void PenguinViewer::setVJClassifierParams(double scaleFactor,
                                          int minNeighbours,
                                          int minWidth,
                                          int minHeight)
{
    m_vjClassifier.setParams(scaleFactor, minNeighbours, QSize(minWidth, minHeight));
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
        Tracker *tracker = m_trackers.first();
        if (1)
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
            //            qDebug() << "trajectory: " << trajectoryPoints;
            for (int i = 0; i < trajectoryPoints.size(); i++)
            {
                //                painter->drawRect(trajectoryRects.at(i));
                painter->drawEllipse(trajectoryPoints.at(i), 2, 2);
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
    QList< QVector<QPointF> > filterdDections;
    QList< QVector<QPointF> > detections = m_vjClassifier.detectPenguins(videoFrame);
    filterdDections = detections;

    if (applyColorFilter())
    {
        filterdDections.clear();
        ColorFeatureExtractor colorFeatureExtractor;
        for (int i = 0; i < detections.size(); i++)
        {
            QVector<QPointF> rect = detections.at(i);

            QRect roi(rect.at(1).toPoint(), rect.at(3).toPoint());
            QImage detectedPenguinImage = m_image.copy(roi);
            cv::Mat patch = Utility::QImageToCvMat(detectedPenguinImage);
            colorFeatureExtractor.compute(patch);

            QVector<int> fv = colorFeatureExtractor.featureVector();

            int numOfFeatures = 576;
            cv::Mat sample = cv::Mat(1, numOfFeatures, CV_32FC1);
            for (int attribute = 0; attribute < numOfFeatures; attribute++)
            {
                if (attribute < numOfFeatures)
                {
                    sample.at<float>(attribute) = fv.at(attribute);
                }
            }

            int ret = m_randomForest.predict(sample);
            if (ret)
            {
                filterdDections.append(rect);
            }
        }
    }

    return filterdDections;
}

QString PenguinViewer::currentFPS()
{
    return QString::number(m_currentFPS);
}

QStringList PenguinViewer::detectedPenguinImageList() const
{
    return m_detectedPenguinImageList;
}

PenguinViewer::SurveillanceStatus PenguinViewer::status() const
{
    return m_surveillanceStatus;
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
        emit applyColorFilterChanged();
    }
}
