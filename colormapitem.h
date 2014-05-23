#ifndef COLORMAPITEM_H
#define COLORMAPITEM_H

#include <QtQuick/QQuickPaintedItem>
#include <QPainter>
#include <QImage>

class ColorMapItem : public QQuickPaintedItem
{
    Q_OBJECT

public:
    ColorMapItem(QQuickItem *parent = 0);
    ColorMapItem(const ColorMapItem &other);
    ColorMapItem& operator =(const ColorMapItem &other);

    void paint(QPainter *painter);
    void intensityMapping(int intensity, QHash<int, int> &bins);

private:
    void drawHistogram(QPainter *painter,
                       QRect histRect,
                       QString levelStr,
                       QColor binColor,
                       QHash<int, int> &bins);

private:
    int m_bin_width;
    int m_bin_height;
    QImage m_image;
};

#endif // COLORMAPITEM_H
