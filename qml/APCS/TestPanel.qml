import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1

Window {
    id: testPanelImp
    width: 740
    height: 600
    minimumWidth: 740
    maximumWidth: 740
    minimumHeight: 600
    maximumHeight: 600
    visible: true

    TabView {
        focus: true

        property int margins: Qt.platform.os === "osx" ? 6 : 0

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: margins

        Tab {
            id: summaryTab
            title: "Summary"

            RowLayout {
                anchors.left: parent.left
                anchors.leftMargin: 6
                anchors.right: parent.right
                anchors.rightMargin: 6
                spacing: 6

                Rectangle {
                    id: testImageView
                    width: 300
                    height: 530
                    color: "grey"

                    Rectangle {
                        id: imageArea
                        width: 280
                        height: 420
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 10

                        Image {
                            id: testImg
                            anchors.fill: parent
                            Connections {
                                target: trainingSys
                                onTestImageChanged: {
//                                    testImg.source = imgFile
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    id: resultView
                    width: 400
                    height: 530

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 10

                        Rectangle {
                            id: rateResultPanel
                            width: resultView.width
                            height: 120
                            color: "transparent"

                            GroupBox {
                                width: resultView.width
                                height: 120

                                title: qsTr("Test Summary")

                                ColumnLayout {
                                    anchors.fill: parent
                                    spacing: 4

                                    Connections {
                                        target: trainingSys
                                        onTestSampleCountChanged: {
                                            posCountText.text = qsTr("No. positives: ") + posCount
                                            negCountText.text = qsTr("No. negatives: ") + negCount
                                        }

                                        onTestTPRChanged: {
                                            tprBar.value = tpr;
                                        }

                                        onTestFPRChanged: {
                                            fprBar.value = fpr;
                                        }
                                    }

                                    RowLayout {
                                        spacing: 60

                                        Text {
                                            id: posCountText
                                            width: paintedWidth
                                            verticalAlignment: Text.AlignJustify
                                            color: "black"
                                            text: qsTr("No. positives: 0")
                                        }

                                        Text {
                                            id: negCountText
                                            width: paintedWidth
                                            verticalAlignment: Text.AlignJustify
                                            color: "black"
                                            text: qsTr("No. negatives: 0")
                                        }
                                    }

                                    TestValueBar {
                                        id: tprBar
                                        title: qsTr("TPR:")
                                        value: 0.0
                                    }

                                    TestValueBar {
                                        id: fprBar
                                        title: qsTr("FPR:")
                                        value: 0.0
                                    }
                                }
                            }
                        }

                        RocPlot {
                            id: rocPlot
                            width: resultView.width
                            height: resultView.width
                        }
                    }
                }
            }
        }
    }
}
