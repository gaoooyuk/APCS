import QtQuick 2.0
import Analysis 1.0

Item {
    id: trajectoryAnalysisControllerImp
    width: 200
    height: deviceSelector.height + 10

    property real scaleFactor: 0.788
    property var currentPlotType: SpatioFrame.PlotType_Detection
    signal callChangePlotType(var type)

    MouseArea {
        anchors.fill: parent
        drag.target: trajectoryAnalysisControllerImp
        drag.axis: Drag.XandYAxis
        drag.minimumX: 0
        drag.maximumX: mainWindow.width - trajectoryAnalysisControllerImp.width
        drag.minimumY: 0
        drag.maximumY: mainWindow.height - trajectoryAnalysisControllerImp.height
        visible: false
    }

    ListModel {
        id: deviceModel
        ListElement {deviceName: "Detections"}
        ListElement {deviceName: "Particles"}
    }

    // deviceSelectPanel
    Item {
        id: deviceSelectPanel
        width: parent.width
        height: deviceModel.count*24
        anchors.left: parent.left
        anchors.bottom: parent.top
        visible: false

        Column {
            Repeater {
                model: deviceModel

                Item {
                    id: dsItem
                    width: deviceSelectPanel.width
                    height: 24

                    property bool hover: false

                    Rectangle {
                        anchors.fill: parent
                        color: hover ? "steelBlue" : "black"
                        opacity: 0.6
                    }

                    Text {
                        id: dsText
                        anchors.centerIn: parent
                        color: hover ? "lightGreen" : "white"
                        text: deviceName
                    }

                    MouseArea {
                        id: ma
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            deviceSelectPanel.visible = false

                            if ("Detections" == deviceName) {
                                currentPlotType = SpatioFrame.PlotType_Detection
                                callChangePlotType(SpatioFrame.PlotType_Detection);
                            } else if ("Particles" == deviceName) {
                                currentPlotType = SpatioFrame.PlotType_Particle
                                callChangePlotType(SpatioFrame.PlotType_Particle);
                            }

                            deviceNameText.text = deviceName
                        }
                        onEntered: {
                            dsText.scale = 1.2
                            dsItem.hover = true;
                        }
                        onExited: {
                            dsText.scale = 1.0
                            dsItem.hover = false;
                        }
                    }
                }
            }
        }
    }

    // deviceSelector
    Item {
        id: deviceSelector
        width: parent.width
        height: phoneText.height
        Text {
            id: phoneText
            width: paintedWidth
            height: paintedHeight
            anchors.verticalCenter: parent.verticalCenter
            color: "white"
            text: qsTr("Plot Type: ")
        }

        Rectangle {
            id: deviceNameItem
            width: parent.width - phoneText.width
            height: parent.height
            anchors.left: phoneText.right
            anchors.verticalCenter: parent.verticalCenter

            Text {
                id: deviceNameText
                anchors.centerIn: parent
                text: qsTr("Detections")
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    deviceSelectPanel.visible = !deviceSelectPanel.visible
                }
            }
        }
    }
}
