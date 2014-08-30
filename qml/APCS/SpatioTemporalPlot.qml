import QtQuick 2.2
import Analysis 1.0
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Rectangle {
    id: stPlotImp
    width: parent.width
    height: parent.height
    color: "black"
    clip: true
    antialiasing: true
    smooth: true

    property alias lbFileDialog: fileDialog

//    ListModel {
//        id: stModel
//    }

    Connections {
        target: penguinViewer

//        onNewSpatioInfoAvailable: {
//            stModel.append({"t_points": points,
//                               "t_weights": weights,
//                               "t_dPoints": dPoints,
//                               "t_dWeights": dWeights,
//                               "t_colors": undefined})
//        }

//        onClusterInfoAvailable: {
//            stModel.set(frameNo, {"t_colors": colorLabels})
//        }

        onPointsAvailable: {
            stBox.updateSpatioTemporalPoints(stPoints);
        }

        onFilteredPointsAvailable: {
            stBox.updateSpatioTemporalFilteredPoints(stFilteredPoints);
        }

        onConesAvailable: {
            stBox.updateSpatioTemporalCones(stCones);
        }

        onLabelingPointsAvailable: {
            stBox.updateSpatioTemporalLabelingPoints(lbPoints);
        }

        onClusterInfoAvailable3: {
            stBox.updateSpatioTemporalCluster(clusterInfo);
        }

        onLineSegmentsAvailable: {
            stBox.updateSpatioTemporalLineSegments(lineSegments);
        }
    }

//    ListView {
//        id: stView
//        width: 320
//        height: parent.height
//        anchors.right: parent.right
//        anchors.rightMargin: 20
//        anchors.bottom: parent.bottom
//        model: stModel
//        spacing: "plane" == state ? -200 : -180
//        verticalLayoutDirection: ListView.BottomToTop
//        focus: true
//        state: "plane"

//        states: [
//            State {
//                name: "plane"
//            },
//            State {
//                name: "normal"
//            },
//            State {
//                name: "3d"
//            }
//        ]

//        delegate: Item {
//            width: 320
//            height: 200
//            antialiasing: true
//            smooth: true

//            Rectangle {
//                id: space
//                width: 320
//                height: 150
//                color: "transparent"
//                border.width: 2
//                border.color: stView.currentIndex == index ? "cyan" : "white"
//                antialiasing: true
//                smooth: true

//                property bool selected: false

//                state: stView.state

//                states: [
//                    State {
//                        name: "normal"
//                        PropertyChanges {
//                            target: space
//                            x: -index * 10
//                            border.width: 2
//                            selected: false
//                        }
//                    },
//                    State {
//                        name: "plane"
//                        PropertyChanges {
//                            target: space
//                            x: 0
//                            border.width: 0
//                            selected: false
//                        }
//                    },
//                    State {
//                        name: "3d"
//                        PropertyChanges {
//                            target: space
//                            x: -index * 10
//                            border.width: 0
//                            selected: false
//                        }
//                    }
//                ]

//                PropertyAnimation {
//                    id: animation
//                    target: space
//                    properties: "x";
//                    easing.type: Easing.InOutQuad
//                }

//                transitions: [
//                    Transition {
//                        from: "plane"; to: "normal"
//                        PropertyAnimation { properties: "x"; easing.type: Easing.InOutQuad }
//                    }
//                ]

//                transform: Rotation {
//                    origin.x: space.width/2;
//                    origin.y: space.height/2;
//                    axis { x: 0; y: 1; z: 0 }
//                    angle: 0
//                }

//                SpatioFrame {
//                    id: sframe
//                    width: parent.width
//                    height: parent.height
//                    trackingPoints: t_points
//                    trackingWeights: t_weights
//                    detectionPoints: t_dPoints
//                    detectionWeights: t_dWeights
////                    detectionLabelColors: t_colors

//                    Connections {
//                        target: trajAController
//                        onCallChangePlotType: {
//                            sframe.changePlotType(type)
//                        }
//                    }

//                    Component.onCompleted: {
//                        sframe.changePlotType(trajAController.currentPlotType)
//                    }
//                }

//                Rectangle {
//                    id: pptEffect
//                    anchors.fill: parent
//                    color: "gray"
//                    visible: "normal" == state
//                    opacity: stView.currentIndex == index ? 0.4 : 0
//                }

//                Text {
//                    id: frameNoText
//                    anchors.right: parent.left
//                    anchors.rightMargin: 10
//                    anchors.verticalCenter: parent.top
//                    verticalAlignment: Text.AlignVCenter
//                    color: stView.currentIndex == index ? "cyan" : "white"
//                    visible: "normal" == space.state && space.selected
//                    text: qsTr("Frame: ") + index
//                }

//                Text {
//                    id: pointsNoText
//                    anchors.left: parent.left
//                    anchors.leftMargin: 2
//                    anchors.top: parent.bottom
//                    anchors.topMargin: 2
//                    color: stView.currentIndex == index ? "cyan" : "white"
//                    visible: "normal" == space.state && space.selected
//                    text: qsTr("#Detections/Clusters")
//                }

//                MouseArea {
//                    anchors.fill: parent
//                    onClicked: {
//                        if ("plane" == space.state || "3d" == space.state)
//                            return;

//                        if (stView.currentIndex != index) {
//                            stView.currentIndex = index
//                        } else {
//                            if ("normal" == space.state) {
//                                space.selected = !space.selected

//                                if (space.selected) {
//                                    animation.to = -(space.width + index * 10) * space.scale
//                                } else {
//                                    animation.to = -index * 10
//                                }

//                                animation.start()
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }

//    // Converts from radians to degrees.
//    function degrees(radians) {
//      return radians * 180 / Math.PI;
//    }

//    property string boxColor: "white"
//    property double boxOpacity: 0.2

//    Rectangle {
//        id: frontalRect
//        width: 320
//        height: 150
//        border.width: 2
//        border.color: boxColor
//        anchors.right: stView.right
//        anchors.bottom: stView.bottom
//        anchors.bottomMargin: 50
//        color: "transparent"
//        visible: "3d" == stView.state || "plane" == stView.state
//    }

//    Rectangle {
//        id: backRect
//        width: 320
//        height: 150
//        border.width: 2
//        border.color: boxColor
//        anchors.right: stView.right
//        anchors.rightMargin: 10 * (stModel.count - 1)
//        anchors.bottom: stView.bottom
//        anchors.bottomMargin: 50 + 20 * (stModel.count - 1)
//        color: "transparent"
//        opacity: boxOpacity
//        visible: "3d" == stView.state
//    }

//    Rectangle {
//        id: lineBottomLeft
//        width: 2
//        height: Math.sqrt(Math.pow(Math.abs(frontalRect.y - backRect.y), 2) + Math.pow(Math.abs(frontalRect.x - backRect.x), 2))
//        anchors.left: backRect.left
//        anchors.top: backRect.bottom
//        color: boxColor
//        smooth: true
//        antialiasing: true
//        transformOrigin: Item.TopLeft
//        rotation: -degrees(Math.atan(Math.abs(frontalRect.x - backRect.x) / Math.abs(frontalRect.y - backRect.y)))
//        opacity: boxOpacity
//        visible: "3d" == stView.state
//    }

//    Rectangle {
//        width: 2
//        height: lineBottomLeft.height
//        anchors.left: backRect.left
//        anchors.top: backRect.top
//        color: boxColor
//        smooth: true
//        antialiasing: true
//        transformOrigin: Item.TopLeft
//        rotation: lineBottomLeft.rotation
//        opacity: boxOpacity
//        visible: "3d" == stView.state
//    }

//    Rectangle {
//        width: 2
//        height: lineBottomLeft.height
//        anchors.right: backRect.right
//        anchors.top: backRect.top
//        color: boxColor
//        smooth: true
//        antialiasing: true
//        transformOrigin: Item.TopLeft
//        rotation: lineBottomLeft.rotation
//        opacity: boxOpacity
//        visible: "3d" == stView.state
//    }

//    Rectangle {
//        width: 2
//        height: lineBottomLeft.height
//        anchors.right: backRect.right
//        anchors.top: backRect.bottom
//        color: boxColor
//        smooth: true
//        antialiasing: true
//        transformOrigin: Item.TopLeft
//        rotation: lineBottomLeft.rotation
//        opacity: boxOpacity
//        visible: "3d" == stView.state
//    }

//    TrajectoryAnalysisContorller {
//        id: trajAController
//        anchors.left: stView.left
//        anchors.bottom: parent.bottom
//        anchors.bottomMargin: 6
//    }

    SpatioTemporalBox {
        id: stBox
        width: parent.width
        height: parent.height
        anchors.centerIn: parent

        isDrawSTPoints: surveillanceArea.isDrawSTPoints
        isDrawLBPoints: surveillanceArea.isDrawLBPoints
        isHighlightClusters: surveillanceArea.isHighlightClusters
        isDrawFilteringSTPoints: surveillanceArea.isDrawFilteringSTPoints
        isDrawClusterEigenLine: surveillanceArea.isDrawClusterEigenLine

        MouseArea {
            anchors.fill: parent
            onWheel: {
                if (wheel.angleDelta.y > 0)
                    stBox.zoomOut();
                else
                    stBox.zoomIn();
            }
        }
    }

    ScaleBar {
        id: rotateBarY
        anchors.bottom: stBox.bottom
        anchors.bottomMargin: 20
        anchors.horizontalCenter: stBox.horizontalCenter
        onOffsetChanged: {
            stBox.rotationY = offset * 100
        }
    }

    ScaleBar {
        id: rotateBarX
        rotation: -90
        anchors.horizontalCenter: stBox.right
        anchors.horizontalCenterOffset: -100
        anchors.verticalCenter: stBox.verticalCenter
        onOffsetChanged: {
            stBox.rotationX = offset * 100
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a .lb file"
        nameFilters: [ "LB files (*.lb)" ]
        onAccepted: {
            stBox.loadSpatioTemporalLabelingPoints(fileUrl)
        }
    }

    Item {
        id: timeScaleControl
        width: 48
        height: 48
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        Image {
            id: timeScaleControlImg
            width: parent.width
            height: parent.height
            smooth: true
            source: "qrc:///qml/icons/plus_minus_btn.png"
        }

        Glow {
            id: timeScaleGlow
            anchors.fill: timeScaleControlImg
            visible: true
            radius: 12
            samples: 24
            spread: 0.5
            transparentBorder: true
            color: "white"
            source: timeScaleControlImg
        }

        Rectangle {
            id: upBtn
            width: 20
            height: 20
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 8
            color: "transparent"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    stBox.timeScaleUp()
                }
                onPressed: {
                    timeScaleGlow.color = "red"
                }
                onReleased: {
                    timeScaleGlow.color = "white"
                }
            }
        }

        Rectangle {
            id: downBtn
            width: 18
            height: 18
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 3
            anchors.right: parent.right
            anchors.rightMargin: 3
            color: "transparent"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    stBox.timeScaleDown()
                }
                onPressed: {
                    timeScaleGlow.color = "green"
                }
                onReleased: {
                    timeScaleGlow.color = "white"
                }
            }
        }
    }

//    Item {
//        id: eyeBtn
//        width: 24
//        height: 24
//        anchors.right: parent.right
//        anchors.rightMargin: 20
//        anchors.bottom: parent.bottom
//        anchors.bottomMargin: 10
//        smooth: true
//        visible: stModel.count > 0

//        Image {
//            id: eyeBtnImg
//            source: "normal" == stView.state ?
//                        "qrc:///qml/icons/eye_off.png"
//                      : "qrc:///qml/icons/eye_on.png"

//            width: 24
//            height: 24
//            anchors.right: parent.right
//            anchors.rightMargin: 20
//            anchors.bottom: parent.bottom
//            anchors.bottomMargin: 10
//            smooth: true
//            visible: stModel.count > 0

//            MouseArea {
//                anchors.fill: parent
//                onClicked: {
//                    if ("plane" == stView.state) {
//                        stView.state = "normal"
//                    } else if ("normal" == stView.state) {
//                        stView.state = "3d"
//                    } else if ("3d" == stView.state) {
//                        stView.state = "plane"
//                    }
//                }
//            }
//        }

//        Glow {
//            id: glow
//            anchors.fill: eyeBtnImg
//            visible: true
//            radius: 12
//            samples: 24
//            spread: 0.5
//            transparentBorder: true
//            color: "yellow"
//            source: eyeBtnImg
//        }
//    }
}
