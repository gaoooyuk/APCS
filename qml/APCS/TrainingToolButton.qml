import QtQuick 2.2

Rectangle {
    id: toolBtnImp

    property string buttonText: ""
    signal btnClicked()

    width: 80
    height: 30
    border.width: 1
    radius: 15
    color: "transparent"
    border.color: "white"

    Text {
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "white"
        text: qsTr(buttonText)
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            btnClicked()
        }
    }
}
