#include "featureitem.h"
#include "lbpitem.h"
#include "lbpfeatureextractor.h"
#include "censusfeatureextractor.h"
#include "colorfeatureextractor.h"
#include "edgefeatureextractor.h"
#include "utility.h"

FeatureItem::FeatureItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    m_extractorType = FeatureItem::HaarLike;   // default feature extractor
    m_image.fill(Qt::black);
    m_imagePath = "";
}

FeatureItem::FeatureItem(const FeatureItem &other)
{
    m_extractorType = other.featureExtractorType();
    m_imagePath = other.imagePath();
}

FeatureItem& FeatureItem::operator =(const FeatureItem &other)
{
    if(this == &other)
        return *this;

    m_extractorType = other.featureExtractorType();
    m_imagePath = other.imagePath();

    return *this;
}

void FeatureItem::paint(QPainter *painter)
{
    if (m_image.isNull())
        return;

    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->drawImage(boundingRect(), m_image);
}

QString FeatureItem::imagePath() const
{
    return m_imagePath;
}

void FeatureItem::setImagePath(QString &imgPath)
{
    if (m_imagePath != imgPath)
    {
        m_imagePath = imgPath;
        compute();
        emit imagePathChanged();
    }
}

FeatureItem::FeatureExtractorType FeatureItem::featureExtractorType() const
{
    return m_extractorType;
}

void FeatureItem::setFeatureExtractorType(FeatureItem::FeatureExtractorType type)
{
    if (m_extractorType != type)
    {
        m_extractorType = type;
        compute();
        emit featureExtractorTypeChanged();
    }
}

void FeatureItem::compute()
{
    if (m_imagePath.isEmpty())
        return;

    cv::Mat cvResult;

    switch (m_extractorType)
    {
    case FeatureItem::HaarLike:
    {
        cv::Mat inputImage = cv::imread(m_imagePath.toUtf8().data(), cv::IMREAD_UNCHANGED);
        if (!inputImage.data)
        {
            qDebug() <<  "Could not open or find the image.";
        }
        cv::Mat frame_gray;
        cv::cvtColor(inputImage, frame_gray, CV_BGR2GRAY);
        cvResult = frame_gray;
        break;
    }

    case FeatureItem::LBP:
    {
        LbpFeatureExtractor lbpFeatureExtractor;
        cvResult = lbpFeatureExtractor.compute(m_imagePath);
        break;
    }

    case FeatureItem::CensusFeature:
    {
        CensusFeatureExtractor censusFeatureExtractor;
        cvResult = censusFeatureExtractor.compute(m_imagePath);
        break;
    }

    case FeatureItem::ColorBins:
    {
        ColorFeatureExtractor colorFeatureExtractor;
        cvResult = colorFeatureExtractor.compute(m_imagePath, 8, 24);
        break;
    }

    case FeatureItem::Laplacian:
    {
        EdgeFeatureExtractor edgeFeatureExtractor;
        cvResult = edgeFeatureExtractor.compute(m_imagePath);
        break;
    }

    default:
        break;
    }

    QImage img = Utility::cvMatToQImage(cvResult);
    if (img.isNull())
    {
        qDebug() << "FeatureItem::compute() Null image returned";
        return;
    }

    m_image = img.scaled(QSize(width(), height()));

    update();
}
