import QtQuick 2.0

Item {
    id: indicatorImp
    width: 144
    height: 114

    ListModel {
        id: indicatorModel
        ListElement { type: "Frontal"; iColor: "#66FF00" }
        ListElement { type: "Degree 90"; iColor: "#FFFF00" }
        ListElement { type: "Degree 90 Upper"; iColor: "#CC9933" }
        ListElement { type: "Degree 90 Lower"; iColor: "#CC6633" }
        ListElement { type: "Degree 45"; iColor: "#3399FF" }
    }

    Column {
        Repeater {
            model: indicatorModel
            Item {
                width: 100; height: 24
                Row {
                    spacing: 4
                    Rectangle {
                        width: 30
                        height: 2
                        anchors.verticalCenter: parent.verticalCenter
                        color: iColor
                    }

                    Text {
                        horizontalAlignment: Text.horizontalCenter
                        text: type
                        color: iColor
                    }
                }
            }
        }
    }
}
