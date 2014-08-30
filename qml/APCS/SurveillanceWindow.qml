import QtQuick 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0

import Core 1.0

import "ArrayJS.js" as ArrayJS

Rectangle {
    id: surveillanceWindowImp
    color: "black"

    property alias stPlotItem: stPlot

    Item {
        id: surveillanceArea
        width: parent.width
        height: 600
        anchors.horizontalCenter: parent.horizontalCenter

        property bool isHovered: false
        property bool showTrajectoryAnalysis: false
        property bool enableManualLabeling: false

        property bool isDrawSTPoints: true
        property bool isDrawLBPoints: false
        property bool isHighlightClusters: false
        property bool isDrawFilteringSTPoints: false
        property bool isDrawClusterEigenLine: false

        PenguinViewer {
            id: penguinViewer
            objectName: "penguinViewer"
            anchors.horizontalCenter: parent.horizontalCenter

            onCurrentFrameNoChanged: {
//                penguinViewer.saveTrackingManualLabelings("track_02.lb")
                ArrayJS.clear();
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
                if (!surveillanceArea.enableManualLabeling)
                    return;

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

            function init() {
                x = surveillanceArea.width - sizeRuler.width - 10;
                y = surveillanceArea.height - sizeRuler.height - 10
            }

            Component.onCompleted: {
                init();
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
            anchors.leftMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    surveillanceArea.showTrajectoryAnalysis = !surveillanceArea.showTrajectoryAnalysis;
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
                Text {
                    width: paintedWidth
                    height: 30
                    anchors.left: parent.left
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "white"
                    text: qsTr("FPS: ") + penguinViewer.currentFPS
                }
            }
        }
    }

    Rectangle {
        width: parent.width
        height: 240
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        visible: !surveillanceArea.showTrajectoryAnalysis

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#bbbbbb" }
            GradientStop { position: 0.33; color: "#cccccc"}
            GradientStop { position: 1.0; color: "#bbbbbb" }
        }

        DetectedObjectPanel {
            id: detectedObjPanel
            width: 400
            height: parent.height
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            detectedModel: penguinViewer.detectedPenguinImageList
        }

        SurveillanceParamsPanel {
            width: parent.width - detectedObjPanel.width
            height: parent.height
            anchors.left: detectedObjPanel.right
            anchors.bottom: parent.bottom
        }
    }

    SpatioTemporalAnalysisParamsPanel {
        id: stAnalysisParamsPanel
        width: parent.width
        height: 240
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        visible: surveillanceArea.showTrajectoryAnalysis
    }

    Item {
        id: btn_training
        width: 32
        height: 32
        anchors.top: parent.top
        anchors.topMargin: 4
        anchors.right: parent.right
        anchors.rightMargin: 10

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
