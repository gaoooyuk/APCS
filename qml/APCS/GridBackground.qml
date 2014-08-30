import QtQuick 2.0

Item {
    id: gridBgImp
    width: parent.width
    height: parent.height
    antialiasing: true
    smooth: true

    property int spacingLineNum: 30
    property real lineHorizontalSpacing: (gridBgImp.width - spacingLineNum) / spacingLineNum
    property real lineVerticalSpacing: (gridBgImp.height - spacingLineNum) / spacingLineNum

    Row {
        anchors.fill: parent
        spacing: gridBgImp.lineHorizontalSpacing
        antialiasing: true
        smooth: true

        Repeater {
            antialiasing: true
            smooth: true

            model: gridBgImp.spacingLineNum
            Rectangle {
                antialiasing: true
                smooth: true
                width: 1;
                height: gridBgImp.height
                opacity: 0.8
                color: "darkGray"
            }
        }
    }

    Column {
        anchors.fill: parent
        spacing: gridBgImp.lineVerticalSpacing
        antialiasing: true
        smooth: true

        Repeater {
            antialiasing: true
            smooth: true

            model: gridBgImp.spacingLineNum
            Rectangle {
                antialiasing: true
                smooth: true
                width: gridBgImp.width;
                height: 1
                opacity: 0.8
                color: "darkGray"
            }
        }
    }
}
