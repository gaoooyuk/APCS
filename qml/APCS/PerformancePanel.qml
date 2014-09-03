import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.1

Window {
    width: 600
    height: 400
    visible: true

    ListModel {
        id: largeModel
        Component.onCompleted: {
            for (var i=0 ; i< 500 ; ++i)
            largeModel.append({ "resolution": "8x24",
                                  "stages": Math.round(Math.random()*100),
                                  "pos": "400",
                                  "neg": "215892",
                                  "cv": "200",
                                  "accuracy": 80 })
        }
    }

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

            TableView {
                model: largeModel
                anchors.margins: 12
                anchors.fill: parent

                TableViewColumn {
                    title: "Index"
                    width: 40

                    delegate: Item {
                        Text {
                            width: parent.width
                            anchors.margins: 4
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            elide: styleData.elideMode
                            text: (styleData.row + 1)
                            color: styleData.textColor
                        }
                    }
                }

                TableViewColumn {
                    role: "resolution"
                    title: "Resolution"
                    width: 80
                }

                TableViewColumn {
                    role: "stages"
                    title: "# Stages"
                    width: 60
                }

                TableViewColumn {
                    role: "pos"
                    title: "# Positive"
                    width: 80
                }

                TableViewColumn {
                    role: "neg"
                    title: "# Negative"
                    width: 80
                }

                TableViewColumn {
                    role: "cv"
                    title: "# Validation"
                    width: 80
                }

                TableViewColumn {
                    title: "Accuracy"
                    width: 120

                    delegate: Item {
                        Text {
                            width: parent.width
                            anchors.margins: 4
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            elide: styleData.elideMode
                            text: largeModel.get(styleData.row).accuracy
                            color: styleData.textColor
                        }

                        Rectangle {
                            anchors.top: parent.top
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            anchors.margins: 4
                            width: 32
                            border.color: "#666"

                            Rectangle{
                                color: "darkGrey"
                                anchors.top:parent.top
                                anchors.topMargin: 1
                                anchors.left:parent.left
                                anchors.leftMargin: 1
                                anchors.bottom:parent.bottom
                                anchors.bottomMargin: 1
                                width: parent.width * largeModel.get(styleData.row).accuracy / 100
                            }
                        }
                    }
                }
            }
        }
    }
}
