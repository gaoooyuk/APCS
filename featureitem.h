#ifndef FEATUREITEM_H
#define FEATUREITEM_H

#include <QtQuick/QQuickPaintedItem>
#include <QPainter>

class FeatureItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString imagePath READ imagePath WRITE setImagePath NOTIFY imagePathChanged)
    Q_ENUMS(FeatureExtractorType)
    Q_PROPERTY(FeatureExtractorType featureExtractorType READ featureExtractorType WRITE setFeatureExtractorType NOTIFY featureExtractorTypeChanged)

public:
    FeatureItem(QQuickItem *parent = 0);
    FeatureItem(const FeatureItem &other);
    FeatureItem& operator =(const FeatureItem &other);

    enum FeatureExtractorType {
        HaarLike,
        LBP,
        CensusFeature,
        ColorBins,
        Laplacian
    };

    void paint(QPainter *painter);

public slots:
    QString imagePath() const;
    void setImagePath(QString &imgPath);

    FeatureItem::FeatureExtractorType featureExtractorType() const;
    void setFeatureExtractorType(FeatureExtractorType type);

signals:
    void imagePathChanged();
    void featureExtractorTypeChanged();

private:
    void compute();

private:
    FeatureExtractorType m_extractorType;
    QImage m_image;
    QString m_imagePath;
};

#endif // FEATUREITEM_H
