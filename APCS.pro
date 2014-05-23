QT += core qml quick
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
    tracker.cpp

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
    tracker.h

INCLUDEPATH += /usr/local/include/
LIBS += -L/opt/local/lib \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_features2d \
    -lopencv_highgui \
    -lopencv_objdetect \
    -lopencv_ml \
    -lm

QMAKE_CXXFLAGS += -openmp
QMAKE_LFLAGS += -openmp
#LIBS += -fopenmp

RESOURCES += \
    qml/icons/icons.qrc
