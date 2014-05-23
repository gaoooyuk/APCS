#ifndef LBPITEM_H
#define LBPITEM_H

#include <QtQuick/QQuickPaintedItem>
#include <QPainter>

class LbpItem : public QQuickPaintedItem
{
    Q_OBJECT

public:
    LbpItem(QQuickItem *parent = 0);
    LbpItem(const LbpItem &other);
    LbpItem& operator =(const LbpItem &other);

    void paint(QPainter *painter);

private:
    QImage m_image;
};

#endif // LBPITEM_H
