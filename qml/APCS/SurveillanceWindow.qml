import QtQuick 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0

import Core 1.0

import "ArrayJS.js" as ArrayJS

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
        property bool showTrajectoryAnalysis: false

        PenguinViewer {
            id: penguinViewer
            objectName: "penguinViewer"
            anchors.horizontalCenter: parent.horizontalCenter

            onCurrentFrameNoChanged: {
//                penguinViewer.saveTrackingManualLabelings("track_02.lb")
                ArrayJS.clear();
            }
        }

        // fpsPanel
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

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                surveillanceArea.isHovered = true;
            }

            onExited: {
                surveillanceArea.isHovered = false;
            }
            onClicked: {
                var frameNo = penguinViewer.currentFrameNo;
                var x = mouse.x;
                var y = mouse.y;
                console.log("Clicked: " + "(" + x + "," + y + ")")
                var obj = targetComp.createObject(surveillanceArea, {"x": x - 16, "y": y - 16});
                ArrayJS.addItem(obj)

                penguinViewer.addTrackingManualLabeling(x, y, frameNo);
            }
        }

        Component {
            id: targetComp

            Item {
                id: targetItem
                width: targetImg.width
                height: targetImg.height
                Image {
                    id: targetImg
                    width: sourceSize.width
                    height: sourceSize.height
                    source: "qrc:///qml/icons/target.png"
                    smooth: true
                }

                MouseArea {
                    anchors.fill: parent
                    drag.target: targetItem
                    drag.axis: Drag.XAndYAxis
                    onClicked: {

                    }
                }
            }
        }

        Ruler {
            id: sizeRuler
            x: surveillanceArea.width - sizeRuler.width - 10
            y: fpsPanel.y + fpsPanel.height - sizeRuler.height
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

        SpatioTemporalPlot {
            id: stPlot
            visible: surveillanceArea.showTrajectoryAnalysis
        }

        Image {
            id: analysisPanelBtn
            source: surveillanceArea.showTrajectoryAnalysis ?
                        "qrc:///qml/icons/trajectoryAnalysisOn.png"
                      : "qrc:///qml/icons/trajectoryAnalysisOff.png"

            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.bottom: confidenceMapBtn.top
            anchors.bottomMargin: 10

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    surveillanceArea.showTrajectoryAnalysis = !surveillanceArea.showTrajectoryAnalysis;
                }
            }
        }

        Image {
            id: showTrajectoriesBtn
            source: penguinViewer.showTrajectories ?
                        "qrc:///qml/icons/trajectoryAnalysisOn.png"
                      : "qrc:///qml/icons/trajectoryAnalysisOff.png"

            anchors.left: smcProcessBtn.right
            anchors.leftMargin: 10
            anchors.bottom: colorClassifierBtn.top
            anchors.bottomMargin: 10

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    penguinViewer.showTrajectories = !penguinViewer.showTrajectories;
                }
            }
        }

        Image {
            id: smcProcessBtn
            source: penguinViewer.showSMCProcess ?
                        "qrc:///qml/icons/trajectoryAnalysisOn.png"
                      : "qrc:///qml/icons/trajectoryAnalysisOff.png"

            anchors.left: confidenceMapBtn.right
            anchors.leftMargin: 10
            anchors.bottom: colorClassifierBtn.top
            anchors.bottomMargin: 10

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    penguinViewer.showSMCProcess = !penguinViewer.showSMCProcess;
                }
            }
        }

        Image {
            id: confidenceMapBtn
            source: penguinViewer.showConfidenceMap ?
                        "qrc:///qml/icons/trajectoryAnalysisOn.png"
                      : "qrc:///qml/icons/trajectoryAnalysisOff.png"

            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.bottom: colorClassifierBtn.top
            anchors.bottomMargin: 10

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    penguinViewer.showConfidenceMap = !penguinViewer.showConfidenceMap;
                }
            }
        }

        Image {
            id: colorClassifierBtn
            source: penguinViewer.applyColorFilter ?
                        "qrc:///qml/icons/color_classifier_switch.png"
                      : "qrc:///qml/icons/color_classifier_switch_off.png"

            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.bottom: fpsPanel.top
            anchors.bottomMargin: 10

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    penguinViewer.applyColorFilter = !penguinViewer.applyColorFilter;
                }
            }
        }

        // infoPanel
        Rectangle {
            id: infoPanel
            width: 100
            height: 200
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.top: parent.top
            anchors.topMargin: 10
            color: "transparent"

            Column {
                anchors.fill: parent
                Text {
                    width: paintedWidth
                    height: 30
                    anchors.left: parent.left
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "white"
                    text: qsTr("Current Frame: ") + penguinViewer.currentFrameNo
                }
                Text {
                    width: paintedWidth
                    height: 30
                    anchors.left: parent.left
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "white"
                    text: qsTr("Estimated No.: ") + penguinViewer.estimatedPenguinAmount
                }
            }
        }
    }

    DetectedObjectPanel {
        id: detectedObjPanel
        width: 400
        height: 200
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        detectedModel: penguinViewer.detectedPenguinImageList
    }

    SurveillanceParamsPanel {
        width: 800
        height: 200
        anchors.left: detectedObjPanel.right
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
