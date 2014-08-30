import QtQuick 2.0

Rectangle {
    id: detectedObjectPanelImp
    width: 400
    height: 200
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#202020" }
        GradientStop { position: 0.33; color: "#222222"}
        GradientStop { position: 1.0; color: "#202020" }
    }
    radius: 5

    property var detectedModel: []

    Text {
        id: centerText
        width: paintedWidth
        height: paintedHeight
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "white"
        text: qsTr("Detected\nPenguins")
    }

    ListView {
        width: parent.width - 8
        height: 180
        anchors.centerIn: parent
        clip: true
        orientation: ListView.Horizontal
        model: detectedObjectPanelImp.detectedModel
        delegate: Image {
            width: 60
            height: 180
            source: modelData
        }
    }

    Image {
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: 2
        width: parent.width + 14
        height: parent.height
        smooth: true
        source: "qrc:///qml/icons/overlay_01.png"
    }
}
