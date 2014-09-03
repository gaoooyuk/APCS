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

    color: "black"
    state: "analysis"

    states: [
        State {
            name: "analysis"
            AnchorChanges { target: originalImgPanel; anchors.left: trainingWindowImp.left }
            AnchorChanges { target: parameterPanel; anchors.right: trainingWindowImp.right }
            AnchorChanges { target: featureDemoPanel; anchors.top: titleBar.bottom }

            AnchorChanges { target: trainingNavigatorPanel; anchors.right: trainingWindowImp.left }
        },
        State {
            name: "detectionTraining"
            AnchorChanges { target: trainingNavigatorPanel; anchors.left: trainingWindowImp.left }

            AnchorChanges { target: originalImgPanel; anchors.right: trainingWindowImp.left }
            AnchorChanges { target: parameterPanel; anchors.left: trainingWindowImp.right }
            AnchorChanges { target: featureDemoPanel; anchors.bottom: trainingWindowImp.top; anchors.top: undefined }
        }
    ]

    transitions: Transition {
        AnchorAnimation { duration: 400; easing.type: Easing.OutQuad }
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
    Item {
        id: featureDemoPanel
        width: analysisPanel.width + btnDemoLeft.width + btnDemoRight.width
        height: 240
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10

        Image {
            id: btnDemoLeft
            width: sourceSize.width
            height: sourceSize.height
            anchors.left: parent.left
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
            height: parent.height
            anchors.left: btnDemoLeft.right
            anchors.verticalCenter: parent.verticalCenter
            analysisImagePath: imageModel.length > 0 ? imageModel[currentIndex] : ""
        }
    }

    TrainingParamsPanel {
        width: parent.width
        height: 240
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    TrainingSystem {
        id: trainingSys

        Component.onCompleted: {
//            imageModel = trainingSys.getAllFilesOfDir("/Users/apple/Desktop/Courses/Penguin/training_images/positives/frontal_8x24")
//            imageModel = trainingSys.getAllFilesOfDir("/Users/apple/Desktop/Courses/Penguin/training_images/positives/frontal_20x60")
            imageModel = trainingSys.getAllFilesOfDir("/Users/apple/Desktop/Courses/Penguin/training_images/positives/frontal")
        }
    }

    Rectangle {
        id: titleBar
        width: parent.width
        height: 40
        color: "darkGray"

        TrainingToolButton {
            id: analysisBtn
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: traningBtn.left
            anchors.rightMargin: 20
            buttonText: "Analysis"

            onBtnClicked: {
                trainingWindowImp.state = "analysis"
            }
        }

        TrainingToolButton {
            id: traningBtn
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.horizontalCenter
            anchors.rightMargin: 10
            buttonText: "Detecter"

            onBtnClicked: {
                trainingWindowImp.state = "detectionTraining"
            }
        }

        TrainingToolButton {
            id: trackingTrainingBtn
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.horizontalCenter
            anchors.leftMargin: 10
            buttonText: "Tracker"

            onBtnClicked: {
            }
        }

        TrainingToolButton {
            id: comparisonBtn
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: trackingTrainingBtn.right
            anchors.leftMargin: 20
            buttonText: "Compare"

            onBtnClicked: {
            }
        }

        Item {
            id: btn_surveillance
            width: 32
            height: 32
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter

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
    }

    PerformancePanel {
        visible: false
    }

    TestPanel {

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
}
