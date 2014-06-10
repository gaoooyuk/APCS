import QtQuick 2.2
import QtQuick.Controls 1.2

Item {
    id: imagePanelImp

    property string analysisImagePath: ""

    width: 300
    height: 450

    Image {
        id: imgPanelTitleBar
        width: sourceSize.width
        height: sourceSize.height
        smooth: true
        fillMode: Image.PreserveAspectFit
        anchors.horizontalCenter: parent.horizontalCenter
        source: "qrc:///qml/icons/titleBar_1.png"
    }

    Rectangle {
        id: imgArea
        color: "#fafafa"
        width: 300
        height: parent.height - imgPanelTitleBar.height - bottomToolBar.height
        anchors.top: imgPanelTitleBar.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        clip: true

        // gridBg
        GridBackground {
            id: gridBg
        }

        Flickable {
            anchors.fill: parent
            contentWidth: penguinImage.width;
            contentHeight: penguinImage.height
            clip: true

            Image {
                id: penguinImage
                width: sourceSize.width
                height: sourceSize.height
                source: analysisImagePath
            }
        }
    }

    Rectangle {
        id: bottomToolBar
        color: "darkGray"
        width: 300
        height: 30
        anchors.top: imgArea.bottom

        Slider {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 4
            width: 100
            maximumValue: 5.0
            stepSize: 1.0
        }
    }

    Image {
        id: prevImgBtn
        source: "qrc:///qml/icons/left_round.png"

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.horizontalCenter
        anchors.rightMargin: 10

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (0 == imageModel.length)
                    return;

                if (0 == currentIndex)
                    currentIndex = imageModel.length - 1
                else
                    currentIndex = currentIndex - 1
            }
        }
    }

    Image {
        id: nextImgBtn
        source: "qrc:///qml/icons/right_round.png"

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.left: parent.horizontalCenter
        anchors.leftMargin: 10

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (0 == imageModel.length)
                    return;

                if (imageModel.length - 1 == currentIndex)
                    currentIndex = 0
                else
                    currentIndex = currentIndex + 1
            }
        }
    }
}
