QT += core qml quick widgets opengl
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
# Add more folders to ship with the application, here
folder_01.source = qml/APCS
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    cascadeparser.cpp \
    haarfeatureitem.cpp \
    cascadewriter.cpp \
    cascadeobject.cpp \
    stage.cpp \
    weakclassifier.cpp \
    feature.cpp \
    colormapitem.cpp \
    penguinviewer.cpp \
    randomforest.cpp \
    lbp/helper.cpp \
    lbp/lbp.cpp \
    lbpitem.cpp \
    utility.cpp \
    violajonesclassifier.cpp \
    tracker.cpp \
    hog/laplacian.cpp \
    featureitem.cpp \
    lbpfeatureextractor.cpp \
    colorfeatureextractor.cpp \
    featureextractor.cpp \
    censusfeatureextractor.cpp \
    edgefeatureextractor.cpp \
    trainingsystem.cpp \
    spatioframe.cpp \
    spatiotemporalbox.cpp \
    evaluationsystem.cpp \
    aralclustering.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    cascadeparser.h \
    haarfeatureitem.h \
    cascadewriter.h \
    cascadeobject.h \
    stage.h \
    weakclassifier.h \
    feature.h \
    colormapitem.h \
    penguinviewer.h \
    randomforest.h \
    lbp/helper.hpp \
    lbp/lbp.hpp \
    lbpitem.h \
    utility.h \
    condensation/condensation.h \
    condensation/data_types.h \
    condensation/model_parameters.h \
    violajonesclassifier.h \
    violajonesdefs.h \
    tracker.h \
    hog/laplacian.h \
    featureitem.h \
    lbpfeatureextractor.h \
    colorfeatureextractor.h \
    featureextractor.h \
    censusfeatureextractor.h \
    edgefeatureextractor.h \
    trainingsystem.h \
    spatioframe.h \
    spatiotemporalbox.h \
    spatiotemporaldef.h \
    evaluationsystem.h \
    aralclustering.h

INCLUDEPATH += /usr/local/include/
LIBS += -L/usr/local/lib \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_features2d \
    -lopencv_highgui \
    -lopencv_objdetect \
    -lopencv_ocl \
    -lopencv_ml \
    -lopencv_contrib \
    -lm \
    -litpp

RESOURCES += \
    icons.qrc
