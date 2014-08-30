#include "violajonesclassifier.h"
#include <QElapsedTimer>
#include <cascadeparser.h>

#include <QDebug>

#include <iostream>

ViolaJonesClassifier::ViolaJonesClassifier(QObject *parent) :
    QObject(parent)
{
    // 1. Add cascade descriptors
    //    m_cascade_xmls.push_back(penguin_cascade_frontal);
    //    m_cascade_xmls.push_back(penguin_cascade_90);
    //    m_cascade_xmls.push_back(penguin_cascade_90_upper);
    //    m_cascade_xmls.push_back(penguin_cascade_90_lower);
    //    m_cascade_xmls.push_back(penguin_cascade_45);
    m_cascade_xmls.push_back(penguin_cascade);

    // Default parameters
    m_scaleFactor = 1.05;
    m_minNeighbours = 3;
    m_minSize = QSize(20, 60);

    m_usingOpenCL = false;
}

void ViolaJonesClassifier::setParams(double scaleFactor, int minNeighbours, QSize minSize)
{
    m_scaleFactor = scaleFactor;
    m_minNeighbours = minNeighbours;
    m_minSize = minSize;
}

ViolaJonesClassifier::VJDetection ViolaJonesClassifier::detectPenguins(cv::Mat videoFrame)
{
#ifdef PERFORMANCE_TUNING
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
#endif

    VJDetection vjDetection;

    cv::Mat inputImage = videoFrame;
    if (!inputImage.data)
    {
        qDebug() <<  "Could not open or find the image.";
        return vjDetection;
    }

    QList<QPolygonF> penguinPolygons;
    QList<QRectF> penguinRects;
    QList<QPointF> PenguinCenters;
    QList<double> penguinWeights;

    int flips = 1;
    // int rorations[5] = {0, -30, -60, 30, 60};
    //        int rorations[5] = {0, -15, -30, 15, 30};
    //    int rorations[3] = {0, -15, 15};
    int rorations[1] = {0};
    int flipCode = 1; // flipping around y-axis

    cv::Mat frame_gray;
    cv::Mat frame;
    QString rectColor;
    cv::cvtColor(inputImage, frame_gray, CV_BGR2GRAY);
    int imgWidth = inputImage.size().width;
    int imgHeight = inputImage.size().height;
    cv::Point center(imgWidth/2, imgHeight/2);

    for (int i = 0; i < flips; i++)
    {
        if (0 != i)
            cv::flip(frame_gray, frame, flipCode);
        else
            frame = frame_gray;

        for (size_t j = 0; j < sizeof(rorations) / sizeof(int); j++)
        {
            int rotation = rorations[j];

            // TODO: cv::parallel_for_
            for (std::vector<std::string>::iterator it = m_cascade_xmls.begin(); it != m_cascade_xmls.end(); ++it)
            {
                std::string cascade_xml = *it;

                // TODO: the classifier is load multiple times
                if (m_usingOpenCL ? !m_oclClassifier.load(cascade_xml) : !m_classifier.load(cascade_xml))
                {
                    printf("Error loading descriptor: %s\n", cascade_xml.c_str());
                    return vjDetection;
                }

                QString fileName = QDir::currentPath() + QDir::separator() + QString(cascade_xml.c_str());
                QFile file(fileName);
                if (!file.open(QFile::ReadOnly | QFile::Text))
                {
                    printf("Error parsing descriptor: %s\n", cascade_xml.c_str());
                    return vjDetection;
                }

                CascadeParser parser;
                double lastStageThreshold = 0.0;
                int lastStageWeakTreesCount = 0;
                if (parser.read(&file))
                {
                    CascadeObject cascade = parser.getCascadeObject();
                    int stageCount = cascade.stageNum();
                    Stage stage = cascade.getStage(stageCount-1);
                    lastStageThreshold = stage.stageThreshold();
                    lastStageWeakTreesCount = stage.maxWeakCount();

                    //                    qDebug() << "fileName: " << fileName;
                    //                    qDebug() << "stageCount: " << stageCount;
                    //                    qDebug() << "lastStageThreshold: " << lastStageThreshold;
                    //                    qDebug() << "lastStageWeakTreesCount: " << lastStageWeakTreesCount;
                }

                if (penguin_cascade_frontal == cascade_xml)
                    rectColor = "Q_COLOR_FRONTAL";
                else if (penguin_cascade_90 == cascade_xml)
                    rectColor = "Q_COLOR_90";
                else if (penguin_cascade_90_upper == cascade_xml)
                    rectColor = "Q_COLOR_90_UPPER";
                else if (penguin_cascade_90_lower == cascade_xml)
                    rectColor = "Q_COLOR_90_LOWER";
                else if (penguin_cascade_45 == cascade_xml)
                    rectColor = "Q_COLOR_45";

                cv::Mat frame_rotated;
                cv::equalizeHist(frame, frame);
                rotate(frame, rotation, frame_rotated);

                VJDetectionPrivate vjDetectionPrivate = detect(frame_rotated);
                if (vjDetectionPrivate.rects.size() != vjDetectionPrivate.weights.size())
                {
                    qFatal("Error: ViolaJonesClassifier::detectPenguins rects.size() != weights.size()");
                }

                for (size_t m = 0; m < vjDetectionPrivate.rects.size(); m++)
                {
                    cv::Rect rect = vjDetectionPrivate.rects[m];
                    cv::RotatedRect rRect = cv::RotatedRect(cv::Point2f(rect.x + rect.width/2, rect.y + rect.height/2), rect.size(), 0);

                    cv::Point2f vertices[4];
                    rRect.points(vertices);

                    QPolygonF polygon;
                    for (int n = 0; n < 4; n++)
                    {
                        // Rotate the rRect around image center
                        int v_x = vertices[n].x - center.x;
                        int v_y = vertices[n].y - center.y;

                        double alpha = rotation * PI / 180;
                        int x =  v_x * cos(alpha) - v_y * sin(alpha);
                        int y = v_x * sin(alpha) + v_y * cos(alpha);

                        vertices[n].x = x + center.x;
                        vertices[n].y = y + center.y;

                        if (0 != i) // If we flipped the image
                        {
                            vertices[n].x = imgWidth - vertices[n].x;
                        }

                        polygon << QPointF(vertices[n].x, vertices[n].y);
                    }

                    penguinPolygons.append(polygon);

                    double weight = vjDetectionPrivate.weights.at(m) - lastStageThreshold;
                    weight = ((weight/lastStageWeakTreesCount) + 1) / 2;
                    penguinWeights.append(weight);
                }
            }
        }
    }

#ifdef PERFORMANCE_TUNING
    qDebug() << "ViolaJonesClassifier::detectPenguins elapsed time: " << elapsedTimer.elapsed();
#endif

    vjDetection.polygons = penguinPolygons;
    vjDetection.weights = penguinWeights;

    return vjDetection;
}

ViolaJonesClassifier::VJDetectionPrivate ViolaJonesClassifier::detect(cv::Mat frame)
{
    VJDetectionPrivate vjDetectionPrivate;

    std::vector<cv::Rect> rects;    // rects detected by cascade classifier
    std::vector<int> rejectLevels;
    std::vector<double> levelWeights;

#ifdef PERFORMANCE_TUNING
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
#endif

    if (m_usingOpenCL)
    {
        cv::ocl::oclMat oclFrame(frame);
        CvMemStorage *storage = cvCreateMemStorage(0);
        CvSeq *objects;

        objects = m_oclClassifier.oclHaarDetectObjects(oclFrame,
                                                       storage,
                                                       m_scaleFactor,
                                                       m_minNeighbours,
                                                       0|cv::CASCADE_SCALE_IMAGE,
                                                       cv::Size(m_minSize.width(), m_minSize.height()),
                                                       cv::Size(120,360));

        std::vector<CvAvgComp> vecAvgComp;
        cv::Seq<CvAvgComp>(objects).copyTo(vecAvgComp);
        rects.resize(vecAvgComp.size());

        for (std::vector<CvAvgComp>::iterator it = vecAvgComp.begin(); it != vecAvgComp.end(); ++it)
        {
            CvAvgComp cp = *it;
            rects.push_back(cp.rect);
        }
    } else {
        m_classifier.detectMultiScale(frame,
                                      rects,
                                      rejectLevels,
                                      levelWeights,
                                      m_scaleFactor,
                                      m_minNeighbours,
                                      0|cv::CASCADE_SCALE_IMAGE,
                                      cv::Size(m_minSize.width(), m_minSize.height()),
                                      cv::Size(120,360),
                                      true);
    }

#ifdef PERFORMANCE_TUNING
    qDebug() << "ViolaJonesClassifier::detect detectMultiScale elapsed time:" << elapsedTimer.elapsed();
#endif

    vjDetectionPrivate.rects = rects;
    vjDetectionPrivate.weights = levelWeights;

    return vjDetectionPrivate;
}

void ViolaJonesClassifier::rotate(cv::Mat& src, double angle, cv::Mat& dst)
{
    int w = src.cols;
    int h = src.rows;
    cv::Point center(w/2, h/2);
    cv::Mat r = cv::getRotationMatrix2D(center, angle, 1.0);

    cv::warpAffine(src, dst, r, cv::Size(w-1, h));
}
