#include "weakclassifier.h"

WeakClassifier::WeakClassifier(QObject *parent) :
    QObject(parent)
{
}

WeakClassifier::WeakClassifier(const WeakClassifier &other)
{
    m_internalNodes = other.internalNodes();
    m_leafValues = other.leafValues();
}

WeakClassifier& WeakClassifier::operator =(const WeakClassifier &other)
{
    if(this == &other)
        return *this;

    m_internalNodes = other.internalNodes();
    m_leafValues = other.leafValues();

    return *this;
}

QString WeakClassifier::internalNodes() const
{
    return m_internalNodes;
}

void WeakClassifier::setInternalNodes(const QString internalNodes)
{
    if (m_internalNodes != internalNodes)
    {
        m_internalNodes = internalNodes;
    }
}

QString WeakClassifier::leafValues() const
{
    return m_leafValues;
}

void WeakClassifier::setLeafValues(const QString leafValues)
{
    if (m_leafValues != leafValues)
    {
        m_leafValues = leafValues;
    }
}
