#include "violajonesclassifier.h"
//#include "utility.h"

#include <QDebug>

ViolaJonesClassifier::ViolaJonesClassifier(QObject *parent) :
    QObject(parent)
{
    // 1. Add cascade descriptors
//    m_cascade_xmls.push_back(penguin_cascade_frontal);
    //    m_cascade_xmls.push_back(penguin_cascade_90);
    //    m_cascade_xmls.push_back(penguin_cascade_90_upper);
    //    m_cascade_xmls.push_back(penguin_cascade_90_lower);
        m_cascade_xmls.push_back(penguin_cascade_45);
}

QList< QVector<QPointF> > ViolaJonesClassifier::detectPenguins(cv::Mat videoFrame)
{
    QList< QVector<QPointF> > penguinRects;

//    m_colorRects.clear();

    cv::Mat inputImage = videoFrame; // cv::imread(m_imagePath.toUtf8().data(), CV_LOAD_IMAGE_UNCHANGED);   // Read the file
    if (!inputImage.data)
    {
        qDebug() <<  "Could not open or find the image.";
        return penguinRects;
    }

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
            for (std::vector<std::string>::iterator it = m_cascade_xmls.begin(); it != m_cascade_xmls.end(); ++it)
            {
                std::string cascade_xml = *it;
                if (!m_classifier.load(cascade_xml))
                {
                    printf("Error loading descriptor: %s\n", cascade_xml.c_str());
                    return penguinRects;;
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

                std::vector<cv::Rect> rects = detect(frame_rotated);
                QList< QVector<QPointF> > new_rects;
                for (size_t m = 0; m < rects.size(); m++)
                {
                    cv::Rect rect = rects[m];
                    cv::RotatedRect rRect = cv::RotatedRect(cv::Point2f(rect.x + rect.width/2, rect.y + rect.height/2), rect.size(), 0);

                    cv::Point2f vertices[4];
                    rRect.points(vertices);

                    QVector<QPointF> rect_points;
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

                        rect_points.append(QPointF(vertices[n].x, vertices[n].y));
                    }

                    new_rects.append(rect_points);
                }

//                if (m_colorRects.contains(rectColor))
//                {
//                    QList< QVector<QPointF> > old_rects = m_colorRects.take(rectColor);
//                    new_rects.append(old_rects);
//                }
//                m_colorRects.insert(rectColor, new_rects);

                penguinRects.append(new_rects);
            }
        }
    }

    return penguinRects;
}

std::vector<cv::Rect> ViolaJonesClassifier::detect(cv::Mat frame)
{
    std::vector<cv::Rect> rects;    // rects detected by cascade classifier
    m_classifier.detectMultiScale(frame, rects, 1.05, 3, 0|CV_HAAR_SCALE_IMAGE, cv::Size(20,60));
    return rects;
}

//std::vector<cv::Rect> PenguinViewer::detect(cv::Mat frame, int rotation)
//{
//    std::vector<cv::Rect> filteredRects;    // rects after apply color classification
//    std::vector<cv::Rect> rects;    // rects detected by cascade classifier
//    cv::Mat frame_rotated;

//    cv::equalizeHist(frame, frame);
//    rotate(frame, rotation, frame_rotated);

//    classifier.detectMultiScale(frame_rotated, rects, 1.05, 3, 0|CV_HAAR_SCALE_IMAGE, cv::Size(20,60));
//    filteredRects = rects;

//    if (m_applyColorFilter)
//    {
//        filteredRects.clear();
//        for (size_t m = 0; m < rects.size(); m++)
//        {
//            cv::Rect rect = rects[m];

//            QImageReader imgReader;
//            imgReader.setFileName(m_imagePath);
//            imgReader.setClipRect(QRect(rect.x, rect.y, rect.width, rect.height));
//            QImage img = imgReader.read();

//            if (!img.isNull())
//            {
//                int bin_width = rect.width / 4;
//                int bin_height = rect.height / 12;
//                QVector<int> output_r;
//                QVector<int> output_g;
//                QVector<int> output_b;
//                for (int i = 0; i < 48; i++)
//                {
//                    int x = (i % 4) * bin_width;
//                    int y = (i / 4) * bin_height;

//                    int r = 0;
//                    int g = 0;
//                    int b = 0;
//                    for (int j = x; j < x + bin_width; j++)
//                    {
//                        for (int k = y; k < y + bin_height; k++)
//                        {
//                            QRgb rgb = img.pixel(j, k);
//                            r += qRed(rgb);
//                            g += qGreen(rgb);
//                            b += qBlue(rgb);
//                        }
//                    }
//                    r = r / (bin_width * bin_height);
//                    g = g / (bin_width * bin_height);
//                    b = b / (bin_width * bin_height);

//                    output_r.append(r);
//                    output_g.append(g);
//                    output_b.append(b);
//                }

//                QVector<int> output;
//                output += output_r;
//                output += output_g;
//                output += output_b;

//                int numOfFeatures = 144;
//                cv::Mat sample = cv::Mat(1, numOfFeatures, CV_32FC1);
//                for (int attribute = 0; attribute < numOfFeatures; attribute++)
//                {
//                    if (attribute < numOfFeatures)
//                    {
//                        sample.at<float>(attribute) = output.at(attribute);
//                    }
//                }

//                int ret = m_randomForest.predict(sample);
//                if (ret)
//                {
//                    filteredRects.push_back(rect);
//                }
//            }
//        }
//    }

//    return filteredRects;
//}

void ViolaJonesClassifier::rotate(cv::Mat& src, double angle, cv::Mat& dst)
{
    int w = src.cols;
    int h = src.rows;
    cv::Point center(w/2, h/2);
    cv::Mat r = cv::getRotationMatrix2D(center, angle, 1.0);

    cv::warpAffine(src, dst, r, cv::Size(w-1, h));
}
