#ifndef FEATURE_H
#define FEATURE_H

#include <QObject>
#include <QRect>
#include <QList>

class Feature : public QObject
{
    Q_OBJECT

public:
    explicit Feature(QObject *parent = 0);
    Feature(const Feature &other);
    Feature& operator =(const Feature &other);

    int tilted() const;
    void setTilted(const int tilted);

    QRect getRectangle(int index) const;
    void appendRectangle(const QRect &rect);
    int rectangleCount() const;

    void appendRectangleFactor(int factor);
    int getRectangleFactor(int index) const;

private:
    int m_tilted;
    QList<QRect> m_rectanglesList;
    QList<int> m_rectangleFactorsList;
};

#endif // FEATURE_H
