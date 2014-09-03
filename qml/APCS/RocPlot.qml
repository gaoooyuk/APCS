import QtQuick 2.0

Rectangle {
    id: rocPlotImp
    width: 400
    height: width
    color: "yellow"

    property int rocSize: 300

    function addPoint(tpr, fpr) {

    }

    ListModel {
        id: violaJonesRoc
    }

    Rectangle {
        id: box
        width: 300
        height: width
        anchors.centerIn: parent
        color: "transparent"
        border.color: "black"
        border.width: 2
        smooth: true
        antialiasing: true

        Rectangle {
            id: leftTopCorner
            width: 6
            height: 2
            color: "black"
            anchors.right: parent.left
            anchors.top: parent.top
        }

        Text {
            id: tprHighText
            width: paintedWidth
            height: paintedHeight
            anchors.verticalCenter: leftTopCorner.verticalCenter
            anchors.right: leftTopCorner.left
            anchors.rightMargin: 2
            text: qsTr("1")
        }

        Rectangle {
            id: leftBottomCorner
            width: 6
            height: 2
            color: "black"
            anchors.right: parent.left
            anchors.bottom: parent.bottom
        }

        Text {
            id: tprLowText
            width: paintedWidth
            height: paintedHeight
            anchors.verticalCenter: leftBottomCorner.verticalCenter
            anchors.right: leftBottomCorner.left
            anchors.rightMargin: 2
            text: qsTr("0.95")
        }

        Rectangle {
            id: bottomLeftCorner
            width: 2
            height: 6
            color: "black"
            anchors.top: parent.bottom
            anchors.left: parent.left
        }

        Text {
            id: fprLowText
            width: paintedWidth
            height: paintedHeight
            anchors.horizontalCenter: leftBottomCorner.horizontalCenter
            anchors.horizontalCenterOffset: 3
            anchors.top: bottomLeftCorner.bottom
            anchors.topMargin: 2
            text: qsTr("0")
        }

        Rectangle {
            id: bottomRightCorner
            width: 2
            height: 6
            color: "black"
            anchors.top: parent.bottom
            anchors.right: parent.right
        }

        Text {
            id: fprHighText
            width: paintedWidth
            height: paintedHeight
            anchors.horizontalCenter: bottomRightCorner.horizontalCenter
            anchors.horizontalCenterOffset: 3
            anchors.top: bottomRightCorner.bottom
            anchors.topMargin: 2
            text: qsTr("0.05")
        }

        GridBackground {
            id: gridBg
            spacingLineNum: 20
        }

        PathView {
            id: pathView
            anchors.fill: parent
            model: 300;
            delegate: Rectangle {
                id: dot
                width: 1
                height: 1
                color: "black"
                smooth: true
                antialiasing: true
            }
            path: Path {
                startX: 0
                startY: rocSize

                PathLine {
                    x: rocSize
                    y: 0
                }
            }
        }
    }
}
