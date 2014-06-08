import QtQuick 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0

import Cascade 1.0
import Analysis 1.0
import Core 1.0

Rectangle {
    id: trainingWindowImp

    property var imageModel: []
    property int currentIndex: 0

    width: 1280
    height: 800
    color: "black"
    state: "analysis"

    states: [
        State {
            name: "analysis"
            AnchorChanges { target: originalImgPanel; anchors.left: trainingWindowImp.left }
            AnchorChanges { target: parameterPanel; anchors.right: trainingWindowImp.right }
            AnchorChanges { target: featureDemoPanel; anchors.bottom: trainingWindowImp.bottom }

            AnchorChanges { target: trainingNavigatorPanel; anchors.right: trainingWindowImp.left }
        },
        State {
            name: "training"
            AnchorChanges { target: trainingNavigatorPanel; anchors.left: trainingWindowImp.left }

            AnchorChanges { target: originalImgPanel; anchors.right: trainingWindowImp.left }
            AnchorChanges { target: parameterPanel; anchors.left: trainingWindowImp.right }
            AnchorChanges { target: featureDemoPanel; anchors.top: trainingWindowImp.bottom; anchors.bottom: undefined }
        }
    ]

    transitions: Transition {
        AnchorAnimation { duration: 400; easing.type: Easing.OutQuad }
    }

    Rectangle {
        id: titleBar
        width: parent.width
        height: 40
        color: "darkGray"

        TrainingToolButton {
            id: analysisBtn
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.horizontalCenter
            anchors.rightMargin: 20
            buttonText: "Analysis"

            onBtnClicked: {
                trainingWindowImp.state = "analysis"
            }
        }

        TrainingToolButton {
            id: traningBtn
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.horizontalCenter
            anchors.leftMargin: 20
            buttonText: "Traning"

            onBtnClicked: {
                trainingWindowImp.state = "training"
            }
        }
    }

    ImagePanel {
        id: originalImgPanel
        width: 300
        height: 450
        anchors.top: titleBar.bottom
        anchors.topMargin: 10
        anchors.leftMargin: 10
        analysisImagePath: imageModel.length > 0 ? imageModel[currentIndex] : ""
    }

    TrainingNavigator {
        id: trainingNavigatorPanel
        width: 300
        height: 450
        anchors.top: titleBar.bottom
        anchors.topMargin: 10
        anchors.leftMargin: 10
    }

    Rectangle {
        id: parameterPanel
        width: 640
        height: 490
        y: titleBar.height
        color: "black"
    }

    // featureDemoPanel
    Rectangle {
        id: featureDemoPanel
        width: parent.width
        height: 280
        anchors.bottom: parent.bottom

        Image {
            id: btnDemoLeft
            width: sourceSize.width
            height: sourceSize.height
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            smooth: true
            source: "qrc:///qml/icons/demoBtnLeft.png"
        }

        Image {
            id: btnDemoRight
            width: sourceSize.width
            height: sourceSize.height
            anchors.left: analysisPanel.right
            anchors.verticalCenter: parent.verticalCenter
            smooth: true
            source: "qrc:///qml/icons/demoBtnRight.png"
        }

        AnalysisPanel {
            id: analysisPanel
            width: 800
            anchors.left: btnDemoLeft.right
            anchors.verticalCenter: parent.verticalCenter
            height: 240
            analysisImagePath: imageModel.length > 0 ? imageModel[currentIndex] : ""
        }
    }

    Item {
        id: btn_surveillance
        width: 32
        height: 32
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10

        Image {
            id: img
            width: parent.width
            height: parent.height
            smooth: true
            source: "qrc:///qml/icons/module_surveillance.png"
        }

        Glow {
            id: glow
            anchors.fill: img
            visible: false
            radius: 12
            samples: 24
            spread: 0.5
            transparentBorder: true
            color: "yellow"
            source: img
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                glow.visible = true;
            }
            onExited: {
                glow.visible = false;
            }
            onClicked: {
                mainWindow.state = "surveillanceMode"
            }
        }
    }

    TrainingSystem {
        id: trainingSys

        Component.onCompleted: {
            imageModel = trainingSys.getAllFilesOfDir("/Users/apple/Desktop/Courses/Penguin/training_images/positives/frontal")
        }
    }

    Rectangle {
        width: 80
        height: 30
        color: "white"
        anchors.centerIn: parent

        Text {
            anchors.centerIn: parent
            text: "Train"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                trainingSys.train(12, 36, 0.999, 0.5, 10, true)
            }
        }
    }

//    Indicator {
//        id: indicator
//        x: 10
//        y: 10
//        visible: !analysisPanel.visible

//        MouseArea {
//            anchors.fill: parent
//            drag.target: indicator
//            drag.axis: Drag.XandYAxis
//            drag.minimumX: 0
//            drag.maximumX: mainWindow.width - indicator.width
//            drag.minimumY: 0
//            drag.maximumY: mainWindow.height - indicator.height
//        }
//    }

//    Image {
//        id: colorClassifierBtn
//        source: "qrc:///color_classifier_switch.png"

//        anchors.top: parent.top
//        anchors.topMargin: 10
//        anchors.right: parent.right
//        anchors.rightMargin: 20

//        MouseArea {
//            anchors.fill: parent
//            onClicked: {
//                penguinViewer.applyColorFilter = !penguinViewer.applyColorFilter;
//            }
//        }
//    }

//    Image {
//        id: analysisPanelBtn
//        source: "qrc:///analysis_panel_btn.png"

//        anchors.top: parent.top
//        anchors.topMargin: 10
//        anchors.right: colorClassifierBtn.left
//        anchors.rightMargin: 10

//        MouseArea {
//            anchors.fill: parent
//            onClicked: {
//                analysisPanel.visible = !analysisPanel.visible
//            }
//        }
//    }
}
