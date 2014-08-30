import QtQuick 2.0

Image {
    id: scaleBarImp
    width: sourceSize.width
    height: sourceSize.height

    source: "qrc:///qml/icons/scaleBar.png"

    property real range: scaleBarImp.width - scaleCircle.width
    function init() {
        scaleCircle.x = scaleBarImp.range / 2
    }

    signal offsetChanged(var offset)

    Image {
        id: scaleCircle
        width: sourceSize.width
        height: sourceSize.height
        x: scaleBarImp.range / 2
        anchors.verticalCenter: parent.verticalCenter
        source: "qrc:///qml/icons/scaleCircle.png"
        onXChanged: {
            var offset = (x - scaleBarImp.range / 2) / (scaleBarImp.range / 2);
            offsetChanged(offset)
        }

        MouseArea {
            anchors.fill: parent
            drag.target: scaleCircle
            drag.axis: Drag.XAxis
            drag.minimumX: 16
            drag.maximumX: scaleBarImp.width - scaleCircle.width - 16
        }
    }
}
