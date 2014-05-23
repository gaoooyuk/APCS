import QtQuick 2.0
import Cascade 1.0
import Analysis 1.0
import Core 1.0

Rectangle {
    id: analysisPanelImp
    width: mainWindow.width
    height: mainWindow.height
    visible: false

    // slidingWindowImg
    Image {
        id: slidingWindowImg
        width: 200;
        height: 600;
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 40
        source: "/Users/apple/Desktop/Courses/Penguin/prototype/training/training/positives/frontal/ratio_1_3/pos-003.jpg"
    }

    Rectangle {
        id: analysisToolSelector
        width: 800
        anchors.left: slidingWindowImg.right
        anchors.leftMargin: 20
        anchors.top: slidingWindowImg.top
        anchors.topMargin: 2
        anchors.bottom: slidingWindowImg.bottom
        anchors.bottomMargin: 2
        border.color: "darkGray"
        border.width: 2

        ListView {
            id: analysisToolListView
            anchors.fill: parent
            clip: true
            orientation: ListView.Horizontal
            model: analysisToolModel
            delegate: Item {
                width: analysisToolSelector.width/3
                height: analysisToolSelector.height

                property int margin: 10

                Image {
                    id: toolImage
                    width: 180
                    height: 540
                    anchors.top: parent.top
                    anchors.topMargin: margin
                    anchors.horizontalCenter: parent.horizontalCenter
                    smooth: true
                    source: t_image
                }

                Text {
                    id: toolName
                    width: paintedWidth
                    height: paintedHeight
                    anchors.top: toolImage.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: t_name
                }
            }
        }

        ListModel {
            id: analysisToolModel
            ListElement {
                t_name: "Haar-Like"
                t_image: "qrc:///analysis_haarlike.png"
            }
            ListElement {
                t_name: "LBP"
                t_image: "qrc:///analysis_lbp.png"
            }
            ListElement {
                t_name: "Pixels(Bins)"
                t_image: "qrc:///analysis_pixels.png"
            }
            ListElement {
                t_name: "Edge(Gradient)"
                t_image: ""
            }
            ListElement {
                t_name: "HSV"
                t_image: ""
            }
        }
    }

    Rectangle {
        id: spacerLine
        width: parent.width
        height: 2
        color: "gray"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 160
    }

//    Timer {
//        id: timer
//        interval: 500; running: false; repeat: true
//        onTriggered: haarItem.showNextFeature();
//    }

    HaarFeatureItem {
        id: haarItem
        objectName: "haarItem"
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 40
        width: 200;
        height: 600;
        visible: false
    }

//    ColorMapItem {
//        id: colorMapItem
//        objectName: "colorMapItem"
//        anchors.top: parent.top
//        anchors.topMargin: 20
//        anchors.right: parent.right
//        anchors.rightMargin: 40
//        width: 800;
//        height: 600;
//    }

//    LbpItem {
//        width: 200
//        height: 600
//        anchors.centerIn: parent
//    }

//    Rectangle {
//        id: prevBtn
//        width: 80
//        height: 40
//        anchors.top: slidingWindowImg.bottom
//        anchors.topMargin: 10
//        anchors.left: slidingWindowImg.left
//        border.color: "black"

//        Text {
//            anchors.centerIn: parent
//            text: qsTr("Prev")
//        }

//        MouseArea {
//            anchors.fill: parent
//            onClicked: {
//                haarItem.showPrevFeature();
//            }
//        }
//    }

//    Rectangle {
//        id: nextBtn
//        width: 80
//        height: 40
//        anchors.top: slidingWindowImg.bottom
//        anchors.topMargin: 10
//        anchors.right: slidingWindowImg.right
//        border.color: "black"

//        Text {
//            anchors.centerIn: parent
//            text: qsTr("Next")
//        }

//        MouseArea {
//            anchors.fill: parent
//            onClicked: {
//                haarItem.showNextFeature();
//            }
//        }
//    }

//    Rectangle {
//        id: timerBtn
//        width: 160
//        height: 40
//        anchors.top: slidingWindowImg.bottom
//        anchors.topMargin: 60
//        anchors.left: slidingWindowImg.left
//        anchors.right: slidingWindowImg.right
//        border.color: "black"

//        Text {
//            id: timerText
//            anchors.centerIn: parent
//            text: qsTr("Start")
//        }

//        MouseArea {
//            anchors.fill: parent
//            onClicked: {
//                if (timerText.text == "Start")
//                {
//                    timerText.text = "Stop"
//                    timer.start();
//                } else {
//                    timerText.text = "Start"
//                    timer.stop();
//                }
//            }
//        }
//    }
}
