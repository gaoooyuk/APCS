import QtQuick 2.2
import QtGraphicalEffects 1.0

Item {
    id: moduleBtnImp
    width: 100
    height: 100

    property string moduleName: ""
    property string moduleImage: ""
    property string glowColor: "yellow"

    signal btnClicked()

    Column {
        anchors.fill: parent

        Item {
            width: 80
            height: 80
            anchors.horizontalCenter: parent.horizontalCenter

            Image {
                id: img
                width: parent.width
                height: parent.height
                smooth: true
                source: moduleImage
            }

            Glow {
                id: glow
                anchors.fill: img
                visible: false
                radius: 12
                samples: 24
                spread: 0.5
                transparentBorder: true
                color: glowColor
                source: img
            }
        }

        Text {
            width: paintedWidth
            height: paintedHeight
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.horizontalCenter
            text: qsTr(moduleName)
            color: "white"
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            glow.visible = true;
        }
        onExited: {
            glow.visible = false;
        }
        onClicked: {
            btnClicked()
        }
    }
}
