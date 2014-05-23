#ifndef WEAKCLASSIFIER_H
#define WEAKCLASSIFIER_H

#include <QObject>

class WeakClassifier : public QObject
{
    Q_OBJECT
public:
    explicit WeakClassifier(QObject *parent = 0);
    WeakClassifier(const WeakClassifier &other);
    WeakClassifier& operator =(const WeakClassifier &other);

    QString internalNodes() const;
    void setInternalNodes(const QString internalNodes);

    QString leafValues() const;
    void setLeafValues(const QString leafValues);

private:
    QString m_internalNodes;
    QString m_leafValues;
};

#endif // WEAKCLASSIFIER_H
