import QtQuick 2.2
import QtGraphicalEffects 1.0

import Cascade 1.0
import Analysis 1.0
import Core 1.0

Rectangle {
    id: analysisPanelImp
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#bbbbbb" }
        GradientStop { position: 0.33; color: "#cccccc"}
        GradientStop { position: 1.0; color: "#bbbbbb" }
    }

    property string analysisImagePath: ""

    Component {
        id: highlight
        Rectangle {
            width: 80; height: 240
            color: "lightsteelblue"; radius: 5
            y: analysisToolListView.currentItem.y
        }
    }

    ListView {
        id: analysisToolListView
        anchors.fill: parent
        clip: true
        orientation: ListView.Horizontal
        focus: true
        spacing: 10

        model: analysisToolModel
        delegate: Item {
            width: 80
            height: 240

            FeatureItem {
                id: featureImg
                width: 80
                height: 240
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                smooth: true
                imagePath: analysisImagePath
                featureExtractorType: t_type
            }

            Glow {
                id: glow
                anchors.fill: featureImg
                visible: index == analysisToolListView.currentIndex
                radius: 12
                samples: 24
                spread: 0.8
                transparentBorder: true
                color: "lightsteelblue"
                source: featureImg
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    analysisToolListView.currentIndex = index
                }
            }

//            Text {
//                id: toolName
//                width: paintedWidth
//                height: paintedHeight
//                anchors.top: toolImage.bottom
//                anchors.horizontalCenter: parent.horizontalCenter
//                text: t_name
//            }
        }
    }

    ListModel {
        id: analysisToolModel
        ListElement {
            t_name: "Haar-Like"
            t_type: FeatureItem.HaarLike
        }
        ListElement {
            t_name: "LBP"
            t_type: FeatureItem.LBP
        }
        ListElement {
            t_name: "CensusFeature"
            t_type: FeatureItem.CensusFeature
        }
        ListElement {
            t_name: "ColorBins"
            t_type: FeatureItem.ColorBins
        }
        ListElement {
            t_name: "Laplacian"
            t_type: FeatureItem.Laplacian
        }
    }

    // slidingWindowImg
//    Image {
//        id: slidingWindowImg
//        width: 200;
//        height: 600;
//        anchors.top: parent.top
//        anchors.topMargin: 20
//        anchors.left: parent.left
//        anchors.leftMargin: 40
//        source: "/Users/apple/Desktop/Courses/Penguin/prototype/training/training/positives/frontal/ratio_1_3/pos-003.jpg"
//    }

//    Rectangle {
//        id: analysisToolSelector
//        width: 800
//        anchors.left: slidingWindowImg.right
//        anchors.leftMargin: 20
//        anchors.top: slidingWindowImg.top
//        anchors.topMargin: 2
//        anchors.bottom: slidingWindowImg.bottom
//        anchors.bottomMargin: 2
//        border.color: "darkGray"
//        border.width: 2


//    }

//    Rectangle {
//        id: spacerLine
//        width: parent.width
//        height: 2
//        color: "gray"
//        anchors.bottom: parent.bottom
//        anchors.bottomMargin: 160
//    }

//    Timer {
//        id: timer
//        interval: 500; running: false; repeat: true
//        onTriggered: haarItem.showNextFeature();
//    }

//    HaarFeatureItem {
//        id: haarItem
//        objectName: "haarItem"
//        anchors.top: parent.top
//        anchors.topMargin: 20
//        anchors.left: parent.left
//        anchors.leftMargin: 40
//        width: 200;
//        height: 600;
//        visible: false
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
}
