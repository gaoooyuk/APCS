import QtQuick 2.0
import QtQuick.Dialogs 1.0
import QtQuick.Controls 1.1
import Cascade 1.0
import Analysis 1.0
import Core 1.0

Rectangle {
    id: mainWindow
    width: 1280
    height: 800
    color: "black"

    property int currentIndex: 0

    ScrollView {
        anchors.fill: parent
        frameVisible: true

        contentItem: PenguinViewer {
            id: penguinViewer
            objectName: "penguinViewer"
            imagePath: imageModel.get(currentIndex).path
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    penguinViewer.start();
                }
            }
        }
    }

    Indicator {
        id: indicator
        x: 10
        y: 10
        visible: !analysisPanel.visible

        MouseArea {
            anchors.fill: parent
            drag.target: indicator
            drag.axis: Drag.XandYAxis
            drag.minimumX: 0
            drag.maximumX: mainWindow.width - indicator.width
            drag.minimumY: 0
            drag.maximumY: mainWindow.height - indicator.height
        }
    }

    Image {
        id: prevImgBtn
        source: "qrc:///left_round.png"

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.horizontalCenter
        anchors.rightMargin: 10
        visible: !analysisPanel.visible

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (0 == currentIndex)
                    currentIndex = imageModel.count - 1
                else
                    currentIndex = currentIndex - 1

                penguinViewer.imagePath = imageModel.get(currentIndex).path
            }
        }
    }

    Image {
        id: nextImgBtn
        source: "qrc:///right_round.png"

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.left: parent.horizontalCenter
        anchors.leftMargin: 10
        visible: !analysisPanel.visible

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (imageModel.count - 1 == currentIndex)
                    currentIndex = 0
                else
                    currentIndex = currentIndex + 1

                penguinViewer.imagePath = imageModel.get(currentIndex).path
            }
        }
    }

    ListModel {
        id: imageModel
        ListElement { path: "/Users/apple/Desktop/Courses/Penguin/prototype/test0.jpg" }
        ListElement { path: "/Users/apple/Desktop/Courses/Penguin/prototype/test2.jpg" }
        ListElement { path: "/Users/apple/Desktop/Courses/Penguin/prototype/t2.jpg" }
        ListElement { path: "/Users/apple/Desktop/Courses/Penguin/prototype/t1.jpg" }
    }

    AnalysisPanel {
        id: analysisPanel
    }

    Image {
        id: colorClassifierBtn
        source: "qrc:///color_classifier_switch.png"

        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 20

        MouseArea {
            anchors.fill: parent
            onClicked: {
                penguinViewer.applyColorFilter = !penguinViewer.applyColorFilter;
            }
        }
    }

    Image {
        id: analysisPanelBtn
        source: "qrc:///analysis_panel_btn.png"

        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.right: colorClassifierBtn.left
        anchors.rightMargin: 10

        MouseArea {
            anchors.fill: parent
            onClicked: {
                analysisPanel.visible = !analysisPanel.visible
            }
        }
    }

    Rectangle {
        id: fpsPanel
        width: 60
        height: 30
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        color: "black"

        Text {
            anchors.centerIn: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "white"
            text: qsTr("FPS: ") + penguinViewer.currentFPS
        }
    }
}
