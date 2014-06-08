import QtQuick 2.0
import QtQuick.Controls 1.2

Rectangle {
    id: surveillanceParamsPanelImp
    width: 800
    height: 200
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#202020" }
        GradientStop { position: 0.33; color: "#222222"}
        GradientStop { position: 1.0; color: "#202020" }
    }

    property int parameterLineHeight: 36

    Item {
        id: slidingWindowParamsPanel
        width: parent.width
        height: parent.height

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
                             ListElement { text: "1.05"; value: 1.05 }
                             ListElement { text: "1.1"; value: 1.1 }
                             ListElement { text: "1.15"; value: 1.15 }
                             ListElement { text: "1.2"; value: 1.2 }
                         }
                         onAccepted: {

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
                         onAccepted: {

                         }
                     }
                }
            }

            // minSizePanel
            Item {
                id: minSizePanel
                width: 200
                height: parameterLineHeight
                Row {
                    spacing: 4

                    Text {
                        text: qsTr("Min Size")
                        anchors.verticalCenter: parent.verticalCenter
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

            // maxSizePanel
            Item {
                id: maxSizePanel
                width: 200
                height: parameterLineHeight
                Row {
                    spacing: 4

                    Text {
                        text: qsTr("Max Size")
                        anchors.verticalCenter: parent.verticalCenter
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }
}
