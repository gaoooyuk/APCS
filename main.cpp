#include <QtGui/QGuiApplication>
#include <QFile>
#include <QQmlEngine>
#include <QQmlContext>

#include "qtquick2applicationviewer.h"
#include "cascadeparser.h"
#include "cascadewriter.h"
#include "cascadeObject.h"
#include "haarfeatureitem.h"
#include "colormapitem.h"

#include "penguinviewer.h"
#include "lbpitem.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<HaarFeatureItem>("Cascade", 1, 0, "HaarFeatureItem");
    qmlRegisterType<ColorMapItem>("Analysis", 1, 0, "ColorMapItem");
    qmlRegisterType<PenguinViewer>("Core", 1, 0, "PenguinViewer");
    qmlRegisterType<LbpItem>("Analysis", 1, 0, "LbpItem");

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/CascadeVisualizer/main.qml"));
    viewer.showExpanded();

    QString fileName("/Users/apple/Desktop/Courses/Penguin/prototype/cascade_45.xml");
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return -1;
    }

    CascadeParser parser;
    if (!parser.read(&file))
    {
    }

    CascadeObject cascadeObj = parser.getCascadeObject();
//    cascadeObj.prune(5);

    HaarFeatureItem *haarItem = viewer.rootObject()->findChild<HaarFeatureItem*>("haarItem");
    if (NULL == haarItem)
        qDebug() << "Can't find haarItem";

    for (int i = 0; i < cascadeObj.featureNum(); i++)
    {
        haarItem->appendFeature(cascadeObj.getFeature(i));
    }

//    QFile wFile("/Users/apple/Desktop/Courses/Penguin/prototype/cascade_45_pruned.xml");
//    if (!wFile.open(QFile::WriteOnly | QFile::Truncate))
//    {
//        return -1;
//    }
//    CascadeWriter writer;
//    writer.write(&wFile, cascadeObj);

    return app.exec();
}
