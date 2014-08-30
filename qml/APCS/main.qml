import QtQuick 2.2

Rectangle {
    id: mainWindow
    width: 1280
    height: 840
    color: "black"
    state: "default"

    states: [
        State {
            name: "default"
            AnchorChanges { target: defaultWind; anchors.left: mainWindow.left }
        },
        State {
            name: "surveillanceMode"
            AnchorChanges { target: defaultWind; anchors.right: mainWindow.left }
        },
        State {
            name: "trainingMode"
            AnchorChanges { target: defaultWind; anchors.left: mainWindow.right }
        }
    ]

    transitions: Transition {
        AnchorAnimation { duration: 600; easing.type: Easing.OutQuad }
    }

    // defaultWind
    Rectangle {
        id: defaultWind
        width: 1280
        height: 800
        color: "black"

        // btn_training
        ModuleButton {
            id: btn_training
            width: 100
            height: 100
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.horizontalCenter
            anchors.rightMargin: 10

            moduleName: "Training"
            moduleImage: "qrc:///qml/icons/module_training.png"
            glowColor: "yellow"

            onBtnClicked: {
                mainWindow.state = "trainingMode"
            }
        }

        // btn_surveillance
        ModuleButton {
            id: btn_surveillance
            width: 100
            height: 100
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.horizontalCenter
            anchors.leftMargin: 10

            moduleName: "Surveillance"
            moduleImage: "qrc:///qml/icons/module_surveillance.png"
            glowColor: "yellow"

            onBtnClicked: {
                mainWindow.state = "surveillanceMode"
            }
        }
    }

    // trainingWind
    TrainingWindow {
        id: trainingWind
        width: parent.width
        height: parent.height
        anchors.right: defaultWind.left
    }

    // surveillanceWind
    SurveillanceWindow {
        id: surveillanceWind
        width: parent.width
        height: parent.height
        anchors.left: defaultWind.right
    }
}
