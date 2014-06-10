import QtQuick 2.0

Rectangle {
    id: rulerImp
    width: 160
    height: 240
    color: "transparent"
    clip: true

    property bool rulerEnabled: false
    property string roiColor: "cyan"

    Rectangle {
        width: 100
        height: 30
        color: "green"

        MouseArea {
            anchors.fill: parent
            drag.target: rulerImp
            drag.axis: Drag.XandYAxis
            drag.minimumX: 0
            drag.maximumX: surveillanceArea.width - rulerImp.width
            drag.minimumY: 0
            drag.maximumY: surveillanceArea.height - rulerImp.height
        }
    }

    Item {
        id: rulerBase
        width: 100
        height: 200
        anchors.centerIn: parent

        Rectangle {
            width: 100
            height: 200
            color: "transparent"
            border.width: 1
            border.color: "white"
        }

        // gridBg
        GridBackground {
            id: gridBg
            spacingLineNum: 10
        }

        Rectangle {
            id: dashLineHorizontal
            width: mouseArea.mouseX
            height: 2
            y: mouseArea.mouseY
            anchors.left: parent.left
            color: roiColor
        }

        Rectangle {
            id: dashLineVertical
            width: 2
            height: rulerBase.height - mouseArea.mouseY
            x: mouseArea.mouseX
            anchors.bottom: parent.bottom
            color: roiColor
        }

        Text {
            id: yValueText
            color: "white"
            width: paintedWidth
            height: paintedHeight
            anchors.right: dashLineHorizontal.left
            anchors.rightMargin: 1
            anchors.verticalCenter: dashLineHorizontal.verticalCenter

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: Math.floor(rulerBase.height - mouseArea.mouseY)
        }

        Text {
            id: xValueText
            color: "white"
            width: paintedWidth
            height: paintedHeight
            anchors.top: dashLineVertical.bottom
            anchors.topMargin: 1
            anchors.horizontalCenter: dashLineVertical.horizontalCenter

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: Math.floor(mouseArea.mouseX)
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: rulerEnabled
            onClicked: {
                rulerEnabled = !rulerEnabled;
            }
        }
    }
}

