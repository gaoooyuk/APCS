import QtQuick 2.0

Item {
    id: gridBgImp
    width: parent.width
    height: parent.height

    property int spacingLineNum: 30
    property real lineHorizontalSpacing: (gridBg.width - spacingLineNum) / spacingLineNum
    property real lineVerticalSpacing: (gridBg.height - spacingLineNum) / spacingLineNum

    Row {
        anchors.fill: parent
        spacing: gridBg.lineHorizontalSpacing
        Repeater {
            model: gridBg.spacingLineNum
            Rectangle {
                width: 1;
                height: gridBg.height
                opacity: 0.8
                color: "darkGray"
            }
        }
    }

    Column {
        anchors.fill: parent
        spacing: gridBg.lineVerticalSpacing
        Repeater {
            model: gridBg.spacingLineNum
            Rectangle {
                width: gridBg.width;
                height: 1
                opacity: 0.8
                color: "darkGray"
            }
        }
    }
}
