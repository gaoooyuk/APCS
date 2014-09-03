import QtQuick 2.2

Item {
    id: detectedObjectPanelImp
    width: 400
    height: 200

    ListModel {
        id: detectedModel
    }

    Connections {
        target: penguinViewer
        onDetectedPenguinImageSaved: {
            detectedModel.append({"path": savedPath})
            savedImageView.positionViewAtEnd()
        }
    }

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
        id: savedImageView
        width: parent.width - 8
        height: 180
        anchors.centerIn: parent
        clip: true
        orientation: ListView.Horizontal
        model: detectedModel
        delegate: Image {
            width: 60
            height: 180
            source: detectedModel.get(index).path
        }

        move: Transition {
            NumberAnimation { properties: "x"; duration: 1000 }
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
