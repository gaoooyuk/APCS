import QtQuick 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0

import Core 1.0

Rectangle {
    id: surveillanceWindowImp
    width: 1280
    height: 800
    color: "black"

    Item {
        id: surveillanceArea
        width: parent.width
        height: 600
        anchors.horizontalCenter: parent.horizontalCenter

        property bool isHovered: false

        PenguinViewer {
            id: penguinViewer
            objectName: "penguinViewer"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                surveillanceArea.isHovered = true;
            }

            onExited: {
                surveillanceArea.isHovered = false;
            }
        }

        // playBtn
        Image {
            id: playBtn
            width: 100
            height: 100
            anchors.centerIn: parent
            source: "qrc:///qml/icons/playBtn.png"
            smooth: true
            visible: (PenguinViewer.Idel == penguinViewer.status) && surveillanceArea.isHovered

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    penguinViewer.start();
                }
            }
        }

        // stopBtn
        Image {
            id: stopBtn
            width: 100
            height: 100
            anchors.centerIn: parent
            source: "qrc:///qml/icons/stopBtn.png"
            smooth: true
            visible: (PenguinViewer.Running == penguinViewer.status) && surveillanceArea.isHovered

            MouseArea {
                anchors.fill: parent
                onClicked: {
                }
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

    SurveillanceParamsPanel {
        width: 800
        height: 200
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
    }

    Item {
        id: btn_training
        width: 32
        height: 32
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10

        Image {
            id: img
            width: parent.width
            height: parent.height
            smooth: true
            source: "qrc:///qml/icons/module_training.png"
        }

        Glow {
            id: glow
            anchors.fill: img
            visible: false
            radius: 12
            samples: 24
            spread: 0.5
            transparentBorder: true
            color: "yellow"
            source: img
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                glow.visible = true;
            }
            onExited: {
                glow.visible = false;
            }
            onClicked: {
                mainWindow.state = "trainingMode"
            }
        }
    }
}
