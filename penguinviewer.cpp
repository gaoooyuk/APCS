#include "penguinviewer.h"
#include <QColor>
#include <QHash>
#include <QImageReader>
#include <QElapsedTimer>
#include <QDateTime>

#include "trainingsystem.h"
#include "colorfeatureextractor.h"
#include "lbpfeatureextractor.h"

#define CV_WINDOW_NAME "penguinVideoWindow"

cv::VideoCapture cap;


PenguinViewer::PenguinViewer(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    m_imagePath = "";
    m_applyColorFilter = true;
    m_saveDetectedPenguins = false;
    m_showConfidenceMap = false;
    m_showSMCProcess = false;
    m_showTrajectories = false;
    m_saveManualLabelings = false;

    cap.open("/Users/apple/Desktop/Courses/Penguin/tracking_videos/tracking2.mp4");
//    cap.open("/Users/apple/Desktop/Courses/Penguin/tracking_videos/08.mp4");
    if(!cap.isOpened())  // check if we succeeded
        qDebug() << "Camera failed.";

    cv::namedWindow(CV_WINDOW_NAME, 1);
    cv::resizeWindow(CV_WINDOW_NAME, 0, 0);

    m_currentFrameNo = 0;
    m_frameCount = 0;
    m_currentFPS = 0.0f;
    m_estimatedPenguinAmount = 0;
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateFPS()));

    m_vjClassifier.setParams(1.25, 0, QSize(40, 120));
    m_surveillanceStatus = PenguinViewer::Idel;
}

PenguinViewer::~PenguinViewer()
{
    qDebug() << "PenguinViewer::~PenguinViewer()";
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
    m_randomForest.train("cb_classifier_20x60/cb.classifier", 1226, 3600);

    m_surveillanceStatus = PenguinViewer::Running;
    emit statusChanged();

    m_detectedPenguinImageList.clear();

    //    #define ABC

#ifdef ABC
    QStringList imgs = TrainingSystem::getAllFilesOfDir("/Users/apple/Desktop/Courses/Penguin/training_images/positives/fake");
    //        QStringList imgs = TrainingSystem::getAllFilesOfDir("/Users/apple/Desktop/Courses/Penguin/training_images/negatives");

    qDebug() << "Num of imgs: " << imgs.length();
    for (int num = 0; num < imgs.length(); num++)
#else
    while (1)
#endif
    {
#ifdef ABC
        cv::Mat frame = cv::imread(imgs.at(num).toUtf8().data(), cv::IMREAD_UNCHANGED);   // Read the file
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

        m_currentFrameNo++;
        emit currentFrameNoChanged();

        frame.convertTo(frame, CV_8UC1);

        int w = frame.cols;
        int h = frame.rows;

        if (w > 1280 || h > 600)
            cv::resize(frame, frame, cv::Size(1280, 600));
        m_image = Utility::cvMatToQImage(frame);

        setWidth(m_image.width());
        setHeight(m_image.height());

        m_trackingSystem.setFrameSize(width(), height());

        m_detections = detectPenguins(frame);
        if (!m_detections.polygons.isEmpty())
        {
            if (saveDetectedPenguins())
            {
                QDateTime dateTime;
                for (int i = 0; i < m_detections.polygons.size(); i++)
                {
                    QRectF roi = m_detections.polygons.at(i).boundingRect();
                    QImage detectedPenguinImage = m_image.copy(roi.toRect());
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
        }

        if (m_trackingSystem.isInit())
        {
            m_trackingSystem.propagate(m_detections);

            m_estimatedPenguinAmount = m_trackingSystem.getEstimatedTargetAmount();
            emit estimatedPenguinAmountChanged();

            QList<Tracker::Particle> trackingParticles = m_trackingSystem.getParticles();
            QList<Tracker::Particle> trackingDetections = m_trackingSystem.getDetections();

            // Tracking points
            QList<STPoint> trackingPoints = m_trackingSystem.getAllSTPoints();
            QVariantList stPointsVL;
            foreach (STPoint stPoint, trackingPoints)
            {
                stPointsVL.append(QVariant::fromValue(stPoint));
            }
            QObject *stPointsObj = new QObject;
            stPointsObj->setProperty("stPoints", stPointsVL);
            emit pointsAvailable(stPointsObj);

            // Tracking filtered points
            QList<STPoint> trackingFilteredPoints = m_trackingSystem.getFilteredSTPoints();
            QVariantList stFilteredPointsVL;
            foreach (STPoint stPoint, trackingFilteredPoints)
            {
                stFilteredPointsVL.append(QVariant::fromValue(stPoint));
            }
            QObject *stFilteredPointsObj = new QObject;
            stFilteredPointsObj->setProperty("stFilteredPoints", stFilteredPointsVL);
            emit filteredPointsAvailable(stFilteredPointsObj);

            // Tracking cones
            QList<STCone> trackingCones = m_trackingSystem.getSTCones();
            QVariantList stConesVL;
            foreach (STCone stCone, trackingCones)
            {
                stConesVL.append(QVariant::fromValue(stCone));
            }
            QObject *stConesObj = new QObject;
            stConesObj->setProperty("stCones", stConesVL);
            emit conesAvailable(stConesObj);

            if (m_saveManualLabelings)
            {
                // Manual labeling points
                QList<STPoint> labelingPoints = m_trackingSystem.getManualLabelings();
                QVariantList lbPointsVL;
                foreach (STPoint stPoint, labelingPoints)
                {
                    lbPointsVL.append(QVariant::fromValue(stPoint));
                }
                QObject *lbPointsObj = new QObject;
                lbPointsObj->setProperty("lbPoints", lbPointsVL);
                emit labelingPointsAvailable(lbPointsObj);
            }

            // Tracking clusters
            QList<STCluster> trackingClusters = m_trackingSystem.getSTClusters();
            QVariantList clustersVL;
            foreach (STCluster cluster, trackingClusters)
            {
                clustersVL.append(QVariant::fromValue(cluster));
            }
            QObject *clusterInfoObj = new QObject;
            clusterInfoObj->setProperty("clusterInfo", clustersVL);
            emit clusterInfoAvailable3(clusterInfoObj);

            // Tracking line segments
            QList<STLine> trackingLineSegments = m_trackingSystem.getAllLineSegments();
            QVariantList lineSegmentsVL;
            foreach (STLine line, trackingLineSegments)
            {
                lineSegmentsVL.append(QVariant::fromValue(line));
            }
            QObject *lineSegmentsObj = new QObject;
            lineSegmentsObj->setProperty("lineSegments", lineSegmentsVL);
            emit lineSegmentsAvailable(lineSegmentsObj);

            QVariantList points;
            QVariantList weights;
            foreach (Tracker::Particle p, trackingParticles)
            {
                points.append(QVariant::fromValue(p.pos));
                weights.append(QVariant::fromValue(p.weight));
            }

            QVariantList dPoints;
            QVariantList dWeights;
            foreach (Tracker::Particle p, trackingDetections)
            {
                dPoints.append(QVariant::fromValue(p.pos));
                dWeights.append(QVariant::fromValue(p.weight));
            }

            QObject *o_p = new QObject;
            o_p->setProperty("points", points);

            QObject *o_w = new QObject;
            o_w->setProperty("weights", weights);

            QObject *o_dp = new QObject;
            o_dp->setProperty("dPoints", dPoints);

            QObject *o_dw = new QObject;
            o_dw->setProperty("dWeights", dWeights);

            emit newSpatioInfoAvailable(o_p, o_w, o_dp, o_dw);

            QList< QList<QColor> > colorLabelsForFrames = m_trackingSystem.getColorLabels();
            for (int i = 0; i < colorLabelsForFrames.length(); i++)
            {
                QList<QColor> colorLabels = colorLabelsForFrames.at(i);
                QVariantList dColors;
                for (int j = 0; j < colorLabels.length(); j++)
                {
                    dColors.append(colorLabels.at(j));
                }

                QObject *o_dc = new QObject;
                o_dc->setProperty("dColors", dColors);
                emit clusterInfoAvailable(o_dc, i);
            }
        }

        update();

#ifdef ABC
        cv::waitKey();
#else
        cv::waitKey();

        //        if(cv::waitKey(10) >= 0)
        //            break;
#endif
    }

    QList<int> sectionClusteringResult = m_trackingSystem.getCountingResult();
    qDebug() << "APCS Result:";
    for (int j = 0; j < sectionClusteringResult.length(); j++)
    {
        int estimatedCount = sectionClusteringResult.at(j);
        qDebug() << QString("For section %1, the estimated penguin amount is %2").arg(j).arg(estimatedCount);
    }

    m_surveillanceStatus = PenguinViewer::Idel;
    emit statusChanged();
}

void PenguinViewer::updateFPS()
{
    m_currentFPS =  m_frameCount;
    m_frameCount = 0;

    emit fpsChanged();
}

int PenguinViewer::currentFrameNo() const
{
    return m_currentFrameNo;
}

void PenguinViewer::setVJClassifierParams(double scaleFactor,
                                          int minNeighbours,
                                          int minWidth,
                                          int minHeight)
{
    m_vjClassifier.setParams(scaleFactor, minNeighbours, QSize(minWidth, minHeight));
}

void PenguinViewer::addTrackingManualLabeling(float x, float y, int frameNo)
{
    if (m_trackingSystem.isInit())
    {
        m_trackingSystem.addManualLabeling(x, y, frameNo);
    }
}

bool PenguinViewer::saveTrackingManualLabelings(QString savedFileName)
{
    bool sucess = false;

    if (m_trackingSystem.isInit())
    {
        QList<STPoint> labels = m_trackingSystem.getManualLabelings();

        QString savedPath = QDir::currentPath() + QDir::separator() + savedFileName;
        QFile file(savedPath);
        if (file.open(QFile::WriteOnly | QFile::Truncate))
        {
            QByteArray data;
            for (int i = 0; i < labels.length(); i++)
            {
                STPoint p = labels.at(i);
                QString dataLine = QString("%1 %2 %3").arg(p.x).arg(p.y).arg(p.t);
                data.append(dataLine);

                if (i != labels.length() - 1)
                    data.append("\n");
            }

            file.write(data);

            sucess = true;
        }

        file.close();
    }

    return sucess;
}

void PenguinViewer::paint(QPainter *painter)
{
    if (m_image.isNull())
        return;

    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->drawImage(boundingRect(), m_image);

    painter->save();

    if (m_detections.polygons.length())
    {
        for (int j = 0; j < m_detections.polygons.length(); j++)
        {
            QPolygonF polygon = m_detections.polygons.at(j);
            double weight = m_detections.weights.at(j);
            QColor color = Utility::getJetColor(weight, 0, 1);

            painter->setPen(color);
            painter->drawPolygon(polygon);
            painter->drawText(polygon.at(0) + QPoint(4, -4), QString::number(weight, 'g', 4));

            // calculate the penguin centre
            QPointF center = m_detections.polygons.at(j).boundingRect().center();
            int x = center.x();
            int y = center.y();

            int lineLen = 6;
            painter->drawLine(QPoint(x-lineLen, y), QPoint(x+lineLen, y));
            painter->drawLine(QPoint(x, y-lineLen), QPoint(x, y+lineLen));
        }
    }

    painter->restore();

    if (m_trackingSystem.isInit())
    {
        if (m_showConfidenceMap)
        {
            cv::Mat confidenceMap = m_trackingSystem.getDetectionConfidenceMap();
            double min;
            double max;
            cv::minMaxIdx(confidenceMap, &min, &max);
            cv::Mat colorMap;
            confidenceMap.convertTo(colorMap, CV_8UC1, 255 / (max-min), -min);
            cv::applyColorMap(colorMap, colorMap, cv::COLORMAP_JET);

            QImage confidenceColorMap = Utility::cvMatToQImage(colorMap);
            if (!confidenceColorMap.isNull())
            {
                painter->save();
                painter->setOpacity(0.5);
                painter->drawImage(boundingRect(), confidenceColorMap);
                painter->restore();
            }
        }

        if (m_showSMCProcess)
        {
            QList<Tracker::Particle> trackingParticles = m_trackingSystem.getParticles();
            QList<int> hightProbParticlesIdx;
            painter->save();
            QPen pen;
            pen.setWidth(2);
            for (int j = 0; j < trackingParticles.size(); j++)
            {
                Tracker::Particle particle = trackingParticles.at(j);
                QPointF pos = particle.pos;
                float weight = particle.weight;
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
                Tracker::Particle particle = trackingParticles.at(idx);
                QPointF pos = particle.pos;
                pen.setColor(Qt::red);
                painter->setPen(pen);
                painter->drawPoint(pos);
            }

            painter->restore();
        }

        if (m_showTrajectories)
        {
            painter->save();
            QPen pen;
            pen.setWidth(2);

            // draw trajectories
            QList<Tracker::Trajectory> trajectories = m_trackingSystem.getTrajectories();
            foreach (Tracker::Trajectory track, trajectories)
            {
                QColor trackColor = track.color;
                int trackLabel = track.label;

                pen.setColor(trackColor);
                painter->setPen(pen);

                foreach (QPointF pos, track.points)
                {
                    painter->drawPoint(pos);
                }
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

ViolaJonesClassifier::VJDetection PenguinViewer::detectPenguins(cv::Mat videoFrame)
{
    ViolaJonesClassifier::VJDetection detections = m_vjClassifier.detectPenguins(videoFrame);

    // TODO: update weights accordingly
    if (applyColorFilter())
    {
        QList<QPolygonF> polygons = detections.polygons;
        QList<QPolygonF> filteredPolygons;
        QList<double> filteredWeights;

        for (int i = 0; i < polygons.size(); i++)
        {
            QRectF roi = polygons.at(i).boundingRect();
            QImage image = m_image.copy(roi.toRect());
            //            image = image.scaled(20, 60);
            cv::Mat cvImage = Utility::QImageToCvMat(image);

            QVector<int> concatenatedFV;

            ColorFeatureExtractor colorFeatureExtractor;
            LbpFeatureExtractor lbpFeatureExtractor;

            colorFeatureExtractor.compute(cvImage);
            QVector<int> cbFv = colorFeatureExtractor.featureVector();

            //            lbpFeatureExtractor.compute(cvImage);
            //            QVector<int> lbpFv = lbpFeatureExtractor.featureVector();

            concatenatedFV += cbFv;
            //            concatenatedFV += lbpFv;

            int numOfFeatures = 3600;
            cv::Mat sample = cv::Mat(1, numOfFeatures, CV_32FC1);
            for (int attribute = 0; attribute < numOfFeatures; attribute++)
            {
                if (attribute < numOfFeatures)
                {
                    sample.at<float>(attribute) = concatenatedFV.at(attribute);
                }
            }

            float ret = m_randomForest.predict_prob(sample);
            double weight = detections.weights.at(i);
            double newWeight = weight * ret;

            if (0 != newWeight)
            {
                filteredWeights.append(newWeight);
                filteredPolygons.append(roi);
            }
        }

        detections.polygons = filteredPolygons;
        detections.weights = filteredWeights;
    }

    return detections;
}

QString PenguinViewer::currentFPS()
{
    return QString::number(m_currentFPS);
}

QString PenguinViewer::estimatedPenguinAmount()
{
    return QString::number(m_estimatedPenguinAmount);
}

QStringList PenguinViewer::detectedPenguinImageList() const
{
    return m_detectedPenguinImageList;
}

PenguinViewer::SurveillanceStatus PenguinViewer::status() const
{
    return m_surveillanceStatus;
}

bool PenguinViewer::applyColorFilter() const
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

bool PenguinViewer::saveDetectedPenguins() const
{
    return m_saveDetectedPenguins;
}

void PenguinViewer::setSaveDetectedPenguins(bool is)
{
    if (m_saveDetectedPenguins != is)
    {
        m_saveDetectedPenguins = is;
        emit saveDetectedPenguinsChanged();
    }
}

bool PenguinViewer::showConfidenceMap() const
{
    return m_showConfidenceMap;
}

void PenguinViewer::setShowConfidenceMap(bool is)
{
    if (m_showConfidenceMap != is)
    {
        m_showConfidenceMap = is;
        emit showConfidenceMapChanged();
        update();
    }
}

bool PenguinViewer::showSMCProcess() const
{
    return m_showSMCProcess;
}

void PenguinViewer::setShowSMCProcess(bool is)
{
    if (m_showSMCProcess != is)
    {
        m_showSMCProcess = is;
        emit showSMCProcessChanged();
        update();
    }
}

bool PenguinViewer::showTrajectories() const
{
    return m_showTrajectories;
}

void PenguinViewer::setShowTrajectories(bool is)
{
    if (m_showTrajectories != is)
    {
        m_showTrajectories = is;
        emit showTrajectoriesChanged();
        update();
    }
}
