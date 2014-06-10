import QtQuick 2.0

Rectangle {
    id: detectedObjectPanelImp
    width: 400
    height: 200

    property var detectedModel: []

    ListView {
        width: parent.width
        height: 180
        anchors.verticalCenter: parent.verticalCenter
        clip: true
        orientation: ListView.Horizontal
        model: detectedObjectPanelImp.detectedModel
        delegate: Image {
            width: 60
            height: 180
            source: modelData
        }
    }
}
