#include "colormapitem.h"
#include <QRgb>
#include <QHash>

ColorMapItem::ColorMapItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    m_bin_width = 50;
    m_bin_height = 50;

    QString str = QString("/Users/apple/Desktop/Courses/Penguin/prototype/training/training/positives/frontal/ratio_1_3/pos-001.jpg");
    m_image.load(str, "JPG");
    m_image = m_image.scaled(200, 600);

//    for (int i = 34; i <= 40; i++)
//    {
//        QString str = QString("/Users/apple/Desktop/Courses/Penguin/prototype/training/training/color_positives/pos-0%1.jpg").arg(i);
//        m_image.load(str, "JPG");
//        m_image = m_image.scaled(200, 600);

//        QByteArray output_r;
//        QByteArray output_g;
//        QByteArray output_b;
//        for (int i = 0; i < 48; i++)
//        {
//            int x = (i % 4) * m_bin_width;
//            int y = (i / 4) * m_bin_height;

//            if (!m_image.isNull())
//            {
//                int r = 0;
//                int g = 0;
//                int b = 0;
//                for (int j = x; j < x + m_bin_width; j++)
//                {
//                    for (int k = y; k < y + m_bin_height; k++)
//                    {
//                        QRgb rgb = m_image.pixel(j, k);
//                        r += qRed(rgb);
//                        g += qGreen(rgb);
//                        b += qBlue(rgb);
//                    }
//                }
//                r = r / (m_bin_width * m_bin_height);
//                g = g / (m_bin_width * m_bin_height);
//                b = b / (m_bin_width * m_bin_height);

//                output_r.append(QString::number(r));
//                output_r.append(",");

//                output_g.append(QString::number(g));
//                output_g.append(",");

//                output_b.append(QString::number(b));
//                output_b.append(",");
//            }
//        }

//        QByteArray output;
//        output.append(output_r);
//        output.append(output_g);
//        output.append(output_b);
//        output.append("1");

//        qDebug() << output;
//    }
}

ColorMapItem::ColorMapItem(const ColorMapItem &other)
{
    Q_UNUSED(other)
    m_bin_width = 50;
    m_bin_height = 50;
}

ColorMapItem& ColorMapItem::operator =(const ColorMapItem &other)
{
    if(this == &other)
        return *this;

    m_bin_width = 50;
    m_bin_height = 50;

    return *this;
}

void ColorMapItem::paint(QPainter *painter)
{
    painter->setOpacity(1);
    painter->setRenderHints(QPainter::Antialiasing, true);

    QHash<int, int> red_bins;
    QHash<int, int> green_bins;
    QHash<int, int> blue_bins;
    QColor color = Qt::black;
    for (int i = 0; i < 48; i++)
    {
        int x = (i % 4) * m_bin_width;
        int y = (i / 4) * m_bin_height;

        if (!m_image.isNull())
        {
            int r = 0;
            int g = 0;
            int b = 0;
            for (int j = x; j < x + m_bin_width; j++)
            {
                for (int k = y; k < y + m_bin_height; k++)
                {
                    QRgb rgb = m_image.pixel(j, k);
                    r += qRed(rgb);
                    g += qGreen(rgb);
                    b += qBlue(rgb);
                }
            }
            r = r / (m_bin_width * m_bin_height);
            g = g / (m_bin_width * m_bin_height);
            b = b / (m_bin_width * m_bin_height);
            color.setRgb(r, g, b);

            intensityMapping(r, red_bins);
            intensityMapping(g, green_bins);
            intensityMapping(b, blue_bins);
        }

        QRect rect(x, y, m_bin_width, m_bin_height);
        painter->fillRect(rect, color);
        painter->drawRect(rect);
    }

    // Draw histograms
    QString levelStr;
    QColor binColor;
    QRect histRect;
    int histWidth = 200;
    int histHeight = 200;
    int margin = 60;

    // draw RGB histogram
    levelStr = "RGB Level";
    binColor = Qt::transparent;
    histRect = QRect(m_bin_width * 4 + margin, margin, histWidth, histHeight);
    drawHistogram(painter, histRect, levelStr, binColor, blue_bins);

    // draw Red histogram
    levelStr = "Red Level";
    binColor = Qt::red;
    histRect = QRect(m_bin_width * 4 + margin, histHeight + margin * 2, histWidth, histHeight);
    drawHistogram(painter, histRect, levelStr, binColor, red_bins);

    // draw Green histogram
    levelStr = "Green Level";
    binColor = Qt::green;
    histRect = QRect(m_bin_width * 4 + margin * 2 + histWidth, margin, histWidth, histHeight);
    drawHistogram(painter, histRect, levelStr, binColor, green_bins);

    // draw Blue histogram
    levelStr = "Blue Level";
    binColor = Qt::blue;
    histRect = QRect(m_bin_width * 4 + margin * 2 + histWidth, histHeight + margin * 2, histWidth, histHeight);
    drawHistogram(painter, histRect, levelStr, binColor, blue_bins);

    painter->drawRect(boundingRect());
}

void ColorMapItem::intensityMapping(int intensity, QHash<int, int> &bins)
{
    // TODO:
    if (intensity >= 0 && intensity < 51)
        bins[1]++;
    else if (intensity >= 51 && intensity < 102)
        bins[2]++;
    else if (intensity >= 102 && intensity < 153)
        bins[3]++;
    else if (intensity >= 153 && intensity < 204)
        bins[4]++;
    else if (intensity >= 204 && intensity <= 255)
        bins[5]++;
}

void ColorMapItem::drawHistogram(QPainter *painter,
                                 QRect histRect,
                                 QString levelStr,
                                 QColor binColor,
                                 QHash<int, int> &bins)
{
    painter->setOpacity(1);

    // TODO: draw grid
    painter->save();
    painter->setPen(Qt::lightGray);
    int num_grid = 20;
    for (int i = 0; i < histRect.height(); i+=(histRect.height()/num_grid))
    {
        painter->drawLine(QPoint(histRect.x(), histRect.y() + i), QPoint(histRect.x() + histRect.width(), histRect.y() + i));
    }

    for (int j = 0; j < histRect.width(); j+=(histRect.width()/num_grid))
    {
        painter->drawLine(QPoint(histRect.x() + j, histRect.y()), QPoint(histRect.x() + j, histRect.y() + histRect.height()));
    }
    painter->restore();

    int bin_w_interval = histRect.width() / bins.keys().size();
    int bin_h_interval = histRect.height() / 48;

    QString countStr("Bin Count");

    QPoint bl = histRect.bottomLeft();
    QPoint br = histRect.bottomRight();
    bl += QPoint(0, 14);
    br += QPoint(-20, 14);
    QPoint bc(bl.x() + (br.x() - bl.x()) / 2, br.y());
    bc += QPoint(-16, 0);
    painter->drawText(bl, "0");
    painter->drawText(br, "255");
    painter->drawText(bc, levelStr);

    QPoint lc(histRect.width() / 2 - br.y() - 20, bl.x() - 4);
    painter->save();
    painter->rotate(-90);
    painter->drawText(lc, countStr);
    painter->restore();

    for (int j = 0; j < bins.keys().size(); j++)
    {
        QRect binCountRect(histRect.bottomLeft().x() + j * bin_w_interval, histRect.bottomLeft().y() - bins[j+1] * bin_h_interval, bin_w_interval, bins[j+1] * bin_h_interval);
        painter->fillRect(binCountRect, binColor);
    }

    // draw bounding box
    painter->drawRect(histRect);
}
