import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1

Rectangle {
    id: stAnalysisParamsPanelImp
    width: 800
    height: 200
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#bbbbbb" }
        GradientStop { position: 0.33; color: "#cccccc"}
        GradientStop { position: 1.0; color: "#bbbbbb" }
    }

    property int parameterLineHeight: 36

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
            title: "Building"

            Item {
                GroupBox {
                    id: slidingWindowParaPanel
                    width: 300
                    height: parent.height
                    anchors.left: parent.left
                    anchors.leftMargin: 6

                    title: qsTr("Display")

                    ColumnLayout {
                        Layout.fillWidth: true

                        CheckBox {
                            id: frameCheckbox
                            text: "Line segments"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                surveillanceArea.isDrawClusterEigenLine = checked;
                            }
                        }
                    }
                }
            }
        }

        Tab {
            title: "Outlier Removal"

            Item {
                Item {
                    id: outlierRemovalParamsPanel

                    anchors.top: parent.top
                    anchors.topMargin: 21
                    anchors.left: parent.left
                    anchors.leftMargin: 20

                    ColumnLayout {
                        Layout.fillWidth: true

                        CheckBox {
                            id: particleCheckbox
                            text: "Enable outlier removal"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                surveillanceArea.isDrawFilteringSTPoints = checked;
                            }
                        }
                    }
                }
            }
        }

        Tab {
            title: "Analysis"

            Item {
                Item {
                    id: analysisSwitchPanel

                    anchors.top: parent.top
                    anchors.topMargin: 21
                    anchors.left: parent.left
                    anchors.leftMargin: 20

                    ColumnLayout {
                        Layout.fillWidth: true

                        CheckBox {
                            id: showOriginalDetectionsCheckbox
                            text: "Show original detections"
                            checked: true
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                surveillanceArea.isDrawSTPoints = checked;
                            }
                        }

                        CheckBox {
                            id: highlightClustersCheckbox
                            text: "Highlight clusters"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                surveillanceArea.isHighlightClusters = checked;
                            }
                        }

                        CheckBox {
                            id: showLabelledDetectionsCB
                            text: "Display labelled detections"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                surveillanceArea.isDrawLBPoints = checked;
                            }
                        }

                        RowLayout {
                            Button {
                                id: loadLBFileBtn
                                text:"Load .lb file"
                                Layout.fillWidth: true
                                menu: Menu {
                                    MenuItem {
                                        text: "File.."
                                        onTriggered: {
                                            stPlotItem.lbFileDialog.open()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
