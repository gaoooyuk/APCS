#include "lbpitem.h"
#include "utility.h"
#include "lbp/lbp.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <QDebug>

LbpItem::LbpItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    QString str = QString("/Users/apple/Desktop/Courses/Penguin/prototype/training/training/positives/frontal/ratio_1_3/pos-003.jpg");
    cv::Mat inputImage = cv::imread(str.toUtf8().data(), IMREAD_UNCHANGED);   // Read the file
    if (!inputImage.data)
    {
        qDebug() <<  "Could not open or find the image.";
        return;
    }

    cv::Mat frame_gray;
    cv::Mat frame_lbp;
    cv::cvtColor(inputImage, frame_gray, CV_BGR2GRAY);

//    libfacerec::olbp(frame_gray, frame_lbp);
    libfacerec::elbp(frame_gray, frame_lbp, 1, 8);

    frame_lbp.convertTo(frame_lbp, CV_8UC1);
    m_image = Utility::cvMatToQImage(inputImage);
    m_image = m_image.scaled(200, 600);

    cv::Mat lbp_hists = libfacerec::spatial_histogram(frame_lbp, 256, 5, 15);
    // size of lbp_hists = # of patterns * grid_x * grid_y
//    std::cout << "M = "<< std::endl << " "  << lbp_hists << std::endl << std::endl;
}

LbpItem::LbpItem(const LbpItem &other)
{
}

LbpItem& LbpItem::operator =(const LbpItem &other)
{
    if(this == &other)
        return *this;

    return *this;
}

void LbpItem::paint(QPainter *painter)
{
    if (m_image.isNull())
        return;

    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->drawImage(boundingRect(), m_image);

    // TODO: draw grid
    QRect histRect = boundingRect().toRect();
    painter->save();
    painter->setPen(Qt::red);
    int num_grid_x = 5;
    int num_grid_y = 15;
    for (int i = 0; i < histRect.height(); i+=(histRect.height()/num_grid_y))
    {
        painter->drawLine(QPoint(histRect.x(), histRect.y() + i), QPoint(histRect.x() + histRect.width(), histRect.y() + i));
    }

    for (int j = 0; j < histRect.width(); j+=(histRect.width()/num_grid_x))
    {
        painter->drawLine(QPoint(histRect.x() + j, histRect.y()), QPoint(histRect.x() + j, histRect.y() + histRect.height()));
    }
    painter->restore();
}

