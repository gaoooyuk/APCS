import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle {
    id: testValueBarImp
    width: 120 + titleText.width + valueText.width
    height: 20
    color: "transparent"

    property string title: ""
    property real value: 0.0

    RowLayout {
        Text {
            id: titleText
            width: paintedWidth
            verticalAlignment: Text.AlignJustify
            color: "black"
            text: title
        }

        Rectangle {
            width: 100
            height: 12
            border.color: "#666"

            Rectangle{
                color: "darkGrey"
                anchors.top:parent.top
                anchors.topMargin: 1
                anchors.left:parent.left
                anchors.leftMargin: 1
                anchors.bottom:parent.bottom
                anchors.bottomMargin: 1
                width: 20
            }
        }

        Text {
            id: valueText
            width: paintedWidth
            verticalAlignment: Text.AlignJustify
            color: "black"
            text: value
        }
    }
}
