import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1

Item {
    id: surveillanceParamsPanelImp
    width: 800
    height: 200

    property int parameterLineHeight: 36
    property real selectedScaleFactor: 1.25
    property int selectedMinNeighbours: 0

    TabView {
        id: frame
        focus: true

        property int margins: Qt.platform.os === "osx" ? 6 : 0

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: margins

        Tab {
            title: "Detection"

            Item {
                GroupBox {
                    id: slidingWindowParaPanel
                    width: 300
                    height: parent.height
                    anchors.left: parent.left
                    anchors.leftMargin: 6

                    title: qsTr("Sliding Window Parameter")

                    Column {
                        anchors.fill: parent

                        // scalingFactorPanel
                        Item {
                            id: scalingFactorPanel
                            width: 200
                            height: parameterLineHeight
                            Row {
                                spacing: 4

                                Text {
                                    text: qsTr("Scale Factor")
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                ComboBox {
                                    model: ListModel {
                                        id: scalingFactorModel
                                        ListElement { text: "1.01"; value: 1.01 }
                                        ListElement { text: "1.05"; value: 1.05 }
                                        ListElement { text: "1.1"; value: 1.1 }
                                        ListElement { text: "1.15"; value: 1.15 }
                                        ListElement { text: "1.2"; value: 1.2 }
                                        ListElement { text: "1.25"; value: 1.25 }
                                        ListElement { text: "1.5"; value: 1.5 }
                                    }
                                    onCurrentIndexChanged: {
                                        selectedScaleFactor = scalingFactorModel.get(currentIndex).value;
                                    }

                                    Component.onCompleted: {
                                        currentIndex = 5;
                                    }
                                }
                            }
                        }

                        // minNeighboursPanel
                        Item {
                            id: minNeighboursPanel
                            width: 200
                            height: parameterLineHeight
                            Row {
                                spacing: 4

                                Text {
                                    text: qsTr("Min Neighbours")
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                ComboBox {
                                    model: ListModel {
                                        id: minNeighboursModel
                                        ListElement { text: "No Grouping"; value: 0 }
                                        ListElement { text: "1"; value: 1 }
                                        ListElement { text: "2"; value: 2 }
                                        ListElement { text: "3"; value: 3 }
                                        ListElement { text: "4"; value: 4 }
                                        ListElement { text: "5"; value: 5 }
                                        ListElement { text: "6"; value: 6 }
                                        ListElement { text: "7"; value: 7 }
                                        ListElement { text: "8"; value: 8 }
                                        ListElement { text: "9"; value: 9 }
                                        ListElement { text: "10"; value: 10 }
                                    }
                                    onCurrentIndexChanged: {
                                        selectedMinNeighbours = minNeighboursModel.get(currentIndex).value;
                                    }
                                }
                            }
                        }

                        Text {
                            id: name
                            text: qsTr("Size Constraints - Minimum")
                        }

                        GroupBox {
                            id: sizePanelGroup
                            Layout.fillWidth: true

                            RowLayout {
                                id: sizePanel
                                property int minBoxWidth: 80

                                ColumnLayout {
                                    Text {
                                        text: qsTr("Width")
                                    }
                                    SpinBox {
                                        id: minSizeBox
                                        Layout.minimumWidth: sizePanel.minBoxWidth
                                        minimumValue: 0
                                        maximumValue: 200
                                        value: 0
                                    }
                                }

                                ColumnLayout {
                                    Text {
                                        text: qsTr("Height")
                                    }
                                    SpinBox {
                                        id: maxSizeBox
                                        Layout.minimumWidth: sizePanel.minBoxWidth
                                        minimumValue: 0
                                        maximumValue: 200
                                        value: 0
                                    }
                                }
                            }
                        }
                    }
                }

                GroupBox {
                    id: hybridDetectorParaPanel
                    width: 300
                    height: parent.height
                    anchors.left: slidingWindowParaPanel.right
                    anchors.leftMargin: 6

                    title: qsTr("Hybrid Detector Parameter")

                    ColumnLayout {
                        Layout.fillWidth: true

                        CheckBox {
                            id: haarCheckbox
                            text: "Enable Haar-Like features"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                // TODO
                            }
                        }

                        CheckBox {
                            id: lbpCheckbox
                            text: "Enable LBP features"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                // TODO
                            }
                        }

                        CheckBox {
                            id: censusCheckbox
                            text: "Enable Census features"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                // TODO
                            }
                        }

                        CheckBox {
                            id: laplacianCheckbox
                            text: "Enable Laplacian features"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                // TODO
                            }
                        }
                    }
                }

                Item {
                    id: otherParaPanel

                    anchors.top: parent.top
                    anchors.topMargin: 21
                    anchors.left: hybridDetectorParaPanel.right
                    anchors.leftMargin: 20

                    ColumnLayout {
                        Layout.fillWidth: true

                        CheckBox {
                            id: frameCheckbox
                            text: "Save detections"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                penguinViewer.saveDetectedPenguins = checked;
                            }
                        }

                        CheckBox {
                            id: tickmarkCheck
                            text: "Show detection score"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                // TODO
                            }
                        }

                        CheckBox {
                            id: hybridDetectorCheckBox
                            text: "Enable Hybrid Detector"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                penguinViewer.applyColorFilter = checked;
                            }
                        }
                    }
                }

                Rectangle {
                    id: applyBtn
                    width: 80
                    height: 30
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 20
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    border.width: 2
                    radius: 6
                    smooth: true
                    antialiasing: true
                    color: "transparent"
                    border.color: "black"

                    Text {
                        anchors.centerIn: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: "black"
                        text: qsTr("Apply")
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            //                penguinViewer.setVJClassifierParams(selectedScaleFactor, selectedMinNeighbours, 20, 60);
                            penguinViewer.setVJClassifierParams(selectedScaleFactor, selectedMinNeighbours, 40, 120);
                        }
                    }
                }
            }
        }

        Tab {
            title: "Tracking"

            Item {
                Item {
                    id: condensationParaPanel

                    anchors.top: parent.top
                    anchors.topMargin: 21
                    anchors.left: parent.left
                    anchors.leftMargin: 20

                    ColumnLayout {
                        Layout.fillWidth: true

                        CheckBox {
                            id: particleCheckbox
                            text: "Show particles"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                penguinViewer.showSMCProcess = checked;
                            }
                        }

                        CheckBox {
                            id: densityMapCheckbox
                            text: "Show density map"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                penguinViewer.showConfidenceMap = checked;
                            }
                        }

                        CheckBox {
                            id: trajectoryCheckbox
                            text: "Show trajectory"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                penguinViewer.showTrajectories = checked;
                            }
                        }
                    }
                }
            }
        }

        Tab {
            title: "Tool"

            Item {
                Item {
                    id: toolSwitchPanel

                    anchors.top: parent.top
                    anchors.topMargin: 21
                    anchors.left: parent.left
                    anchors.leftMargin: 20

                    ColumnLayout {
                        Layout.fillWidth: true

                        CheckBox {
                            id: sizeToolCheckbox
                            text: "Enable sizeing tool"
                            checked: true
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                sizeRuler.visible = checked;

                                if (!sizeRuler.visible)
                                    sizeRuler.init();
                            }
                        }

                        CheckBox {
                            id: labellingToolCheckbox
                            text: "Enable labelling"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                surveillanceArea.enableManualLabeling = checked;
                            }
                        }

                        CheckBox {
                            id: inspectionCheckbox
                            text: "Enable inspection"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                penguinViewer.enableInspection = checked;
                            }
                        }
                    }
                }
            }
        }
    }
}
