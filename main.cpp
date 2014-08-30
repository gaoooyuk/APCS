#include <QApplication>
#include <QFile>
#include <QQmlEngine>
#include <QQmlContext>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>


#include "qtquick2applicationviewer.h"
#include "cascadeparser.h"
#include "cascadewriter.h"
#include "cascadeObject.h"
#include "haarfeatureitem.h"
#include "colormapitem.h"

#include "penguinviewer.h"
#include "featureitem.h"
#include "trainingsystem.h"
#include "spatioframe.h"
#include "spatiotemporalbox.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QPixmap pixmap(":/qml/icons/APCS_Logo.png");
    QSplashScreen splash(pixmap);
    splash.showMessage(QString("Initialize African Penguin Counter System"), Qt::AlignLeft | Qt::AlignBottom, Qt::white);

    QTimer splashTimer;
    QObject::connect(&splashTimer, SIGNAL(timeout()), &splash, SLOT(close()));

    splash.show();
    splashTimer.start(1000);

    qmlRegisterType<HaarFeatureItem>("Cascade", 1, 0, "HaarFeatureItem");
    qmlRegisterType<ColorMapItem>("Analysis", 1, 0, "ColorMapItem");
    qmlRegisterType<PenguinViewer>("Core", 1, 0, "PenguinViewer");
    qmlRegisterType<FeatureItem>("Analysis", 1, 0, "FeatureItem");
    qmlRegisterType<TrainingSystem>("Core", 1, 0, "TrainingSystem");
    qmlRegisterType<SpatioFrame>("Analysis", 1, 0, "SpatioFrame");
    qmlRegisterType<SpatioTemporalBox>("Analysis", 1, 0, "SpatioTemporalBox");

    QtQuick2ApplicationViewer viewer;

    viewer.setMainQmlFile(QStringLiteral("qml/APCS/main.qml"));
    viewer.showExpanded();

//    HaarFeatureItem *haarItem = viewer.rootObject()->findChild<HaarFeatureItem*>("haarItem");
//    if (NULL == haarItem) {
//        qDebug() << "Can't find haarItem";
//    } else {
//        for (int i = 0; i < cascadeObj.featureNum(); i++)
//        {
//            haarItem->appendFeature(cascadeObj.getFeature(i));
//        }
//    }

//    QFile wFile("/Users/apple/Desktop/Courses/Penguin/prototype/cascade_45_pruned.xml");
//    if (!wFile.open(QFile::WriteOnly | QFile::Truncate))
//    {
//        return -1;
//    }
//    CascadeWriter writer;
//    writer.write(&wFile, cascadeObj);

    return app.exec();
}
