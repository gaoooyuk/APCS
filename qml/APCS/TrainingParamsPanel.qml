import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1

Rectangle {
    id: trainingParamsPanelImp
    width: 800
    height: 200
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#bbbbbb" }
        GradientStop { position: 0.33; color: "#cccccc"}
        GradientStop { position: 1.0; color: "#bbbbbb" }
    }

    property int parameterLineHeight: 32

    FileDialog {
        id: folderDialog
        selectFolder: true
    }

    TabView {
        id: frame
        focus: true

        property int margins: Qt.platform.os === "osx" ? 6 : 0

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: margins

        Tab {
            title: "Training"

            Item {
                GroupBox {
                    id: inputParaPanel
                    width: 300
                    height: parent.height
                    anchors.left: parent.left
                    anchors.leftMargin: 6

                    title: qsTr("Input Setup")

                    ColumnLayout {
                        anchors.fill: parent

                        // positiveSamplePanel
                        Item {
                            id: positiveSamplePanel
                            width: 200
                            height: parameterLineHeight
                            RowLayout {
                                spacing: 4

                                Image {
                                    id: positiveLoadedImg
                                    source: "qrc:///qml/icons/tick.png"
                                }

                                Text {
                                    text: qsTr("Positive Samples")
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }
                            }
                        }

                        Button {
                            id: loadPositiveBtn
                            text:"Choose Dir (Positive)"
                            Layout.fillWidth: true
                            menu: Menu {
                                MenuItem {
                                    text: "Browse"
                                    onTriggered: {
                                        folderDialog.open()
                                    }
                                }
                            }
                        }

                        // negativeSamplePanel
                        Item {
                            id: negativeSamplePanel
                            width: 200
                            height: parameterLineHeight
                            RowLayout {
                                spacing: 4

                                Image {
                                    id: negativeLoadedImg
                                    source: "qrc:///qml/icons/tick.png"
                                }

                                Text {
                                    text: qsTr("Negative Samples")
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }
                            }
                        }

                        Button {
                            id: loadNegativeBtn
                            text:"Choose Dir (Negative)"
                            Layout.fillWidth: true
                            menu: Menu {
                                MenuItem {
                                    text: "Browse"
                                    onTriggered: {
                                        folderDialog.open()
                                    }
                                }
                            }
                        }

                        // crossValidPanel
                        Item {
                            id: crossValidPanel
                            width: 200
                            height: parameterLineHeight
                            RowLayout {
                                spacing: 4

                                CheckBox {
                                    id: crossValidCheckbox
                                    text: qsTr("Cross-Validation")
                                    checked: false
                                    Layout.minimumWidth: 100

                                    onCheckedChanged: {
                                        crossValidFoldBox.enabled = checked;
                                    }
                                }

                                SpinBox {
                                    id: crossValidFoldBox
                                    Layout.minimumWidth: 60
                                    minimumValue: 2
                                    maximumValue: 10
                                    value: 10
                                    enabled: false
                                }

                                Text {
                                    text: qsTr("Fold")
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }
                            }
                        }
                    }
                }

                GroupBox {
                    id: outputParaPanel
                    width: 240
                    height: parent.height
                    anchors.left: inputParaPanel.right
                    anchors.leftMargin: 6

                    title: qsTr("Output Setup")

                    ColumnLayout {
                        anchors.fill: parent

                        // outputDirPanel
                        Item {
                            id: outputDirPanel
                            width: 200
                            height: 48

                            ColumnLayout {
                                RowLayout {
                                    spacing: 4

                                    Image {
                                        id: outputDirConfigImg
                                        source: "qrc:///qml/icons/tick.png"
                                    }

                                    Text {
                                        text: qsTr("Output Dir")
                                        anchors.verticalCenter: parent.verticalCenter
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                }

                                Button {
                                    id: setOutputDirBtn
                                    text:"Choose Dir (Output)"
                                    Layout.fillWidth: true
                                    menu: Menu {
                                        MenuItem {
                                            text: "Default"
                                            onTriggered: {
                                            }
                                        }
                                        MenuItem {
                                            text: "Browse"
                                            onTriggered: {
                                                folderDialog.open()
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // classifierNamePanel
                        Item {
                            id: classifierNamePanel
                            width: 200
                            height: 48

                            ColumnLayout {
                                RowLayout {
                                    spacing: 4

                                    Image {
                                        id: classifierNameSetImg
                                        source: "qrc:///qml/icons/tick.png"
                                    }

                                    Text {
                                        text: qsTr("Classifier Name")
                                        anchors.verticalCenter: parent.verticalCenter
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                }

                                RowLayout {
                                    spacing: 4

                                    TextField {
                                        placeholderText: qsTr("classifier")
                                    }

                                    Text {
                                        text: qsTr(".xml")
                                        anchors.verticalCenter: parent.verticalCenter
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                }
                            }
                        }

                        // resultNamePanel
                        Item {
                            id: resultNamePanel
                            width: 200
                            height: 48
                            ColumnLayout {
                                RowLayout {
                                    spacing: 4

                                    Image {
                                        id: resultNameSetImg
                                        source: "qrc:///qml/icons/tick.png"
                                    }

                                    Text {
                                        text: qsTr("Result")
                                        anchors.verticalCenter: parent.verticalCenter
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                }

                                RowLayout {
                                    spacing: 4

                                    TextField {
                                        placeholderText: qsTr("result")
                                    }

                                    Text {
                                        text: qsTr(".json")
                                        anchors.verticalCenter: parent.verticalCenter
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                }
                            }
                        }
                    }
                }

                GroupBox {
                    id: vjParaPanel
                    width: 280
                    height: parent.height
                    anchors.left: outputParaPanel.right
                    anchors.leftMargin: 6

                    title: qsTr("Viola-Jones Setup")

                    ColumnLayout {
                        anchors.fill: parent

                        // numStagesPanel
                        Item {
                            id: numStagesPanel
                            height: parameterLineHeight
                            RowLayout {
                                anchors.fill: parent
                                spacing: 4

                                Text {
                                    text: qsTr("numStages:")
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                SpinBox {
                                    id: numStagesBox
                                    Layout.minimumWidth: 60
                                    minimumValue: 1
                                    maximumValue: 20
                                    value: 8
                                }
                            }
                        }

                        // minHitRatePanel
                        Item {
                            id: minHitRatePanel
                            height: parameterLineHeight
                            RowLayout {
                                anchors.fill: parent
                                spacing: 4

                                Text {
                                    text: qsTr("minHitRate:")
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                SpinBox {
                                    id: minHitRateBox
                                    Layout.minimumWidth: 60
                                    minimumValue: 0.95
                                    maximumValue: 0.999
                                    value: 0.995
                                    stepSize: 0.001
                                    decimals: 3
                                }
                            }
                        }

                        // maxFalseAlarmRatePanel
                        Item {
                            id: maxFalseAlarmRatePanel
                            height: parameterLineHeight
                            RowLayout {
                                anchors.fill: parent
                                spacing: 4

                                Text {
                                    text: qsTr("maxFalAlarmRate:")
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                ComboBox {
                                    id: maxFalseAlarmRateBox
                                    Layout.minimumWidth: 60
                                    model: [ "0.5" ]
                                }
                            }
                        }

                        // precalcValBufSizePanel
                        Item {
                            id: precalcValBufSizePanel
                            height: parameterLineHeight
                            RowLayout {
                                anchors.fill: parent
                                spacing: 4

                                Text {
                                    text: qsTr("precalcValBufSize:")
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                ComboBox {
                                    id: precalcValBufSizeBox
                                    Layout.minimumWidth: 60
                                    model: [ "256MB", "512MB", "1GB" ]
                                }
                            }
                        }

                        // precalcIdxBufSizePanel
                        Item {
                            id: precalcIdxBufSizePanel
                            height: parameterLineHeight
                            RowLayout {
                                anchors.fill: parent
                                spacing: 4

                                Text {
                                    text: qsTr("precalcIdxBufSize:")
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                ComboBox {
                                    id: precalcIdxBufSizeBox
                                    Layout.minimumWidth: 60
                                    model: [ "256MB", "512MB", "1GB" ]
                                }
                            }
                        }
                    }
                }

                Item {
                    id: otherParaPanel

                    anchors.top: parent.top
                    anchors.topMargin: 21
                    anchors.left: vjParaPanel.right
                    anchors.leftMargin: 20

                    ColumnLayout {
                        Layout.fillWidth: true

                        CheckBox {
                            id: frameCheckbox
                            text: "Save detections"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                penguinViewer.saveDetectedPenguins = checked;
                            }
                        }

                        CheckBox {
                            id: tickmarkCheck
                            text: "Show detection score"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                // TODO
                            }
                        }

                        CheckBox {
                            id: hybridDetectorCheckBox
                            text: "Enable Hybrid Detector"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                penguinViewer.applyColorFilter = checked;
                            }
                        }

                        Row {
                            spacing: 4

                            ProgressBar {
                                id: trainingProgressBar
                                minimumValue: 0
                                maximumValue: 100
                                value: 60
                            }

                            Text {
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignJustify
                                color: "black"
                                text: trainingProgressBar.value + "%"
                            }
                        }
                    }
                }

                Rectangle {
                    id: applyBtn
                    width: 80
                    height: 30
                    anchors.bottom: testBtn.top
                    anchors.bottomMargin: 10
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    border.width: 2
                    radius: 6
                    smooth: true
                    antialiasing: true
                    color: "transparent"
                    border.color: "black"

                    Text {
                        anchors.centerIn: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: "black"
                        text: qsTr("Train")
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
//                            trainingSys.trainVJ(20, 60, 0.995, 0.5, 6, true)
//                            trainingSys.trainVJ(8, 24, 0.995, 0.5, 8, true)
                            trainingSys.trainRF(8, 24);
//                            trainingSys.trainRF(20, 60);
                        }
                    }
                }

                Rectangle {
                    id: testBtn
                    width: 80
                    height: 30
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 20
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    border.width: 2
                    radius: 6
                    smooth: true
                    antialiasing: true
                    color: "transparent"
                    border.color: "black"

                    Text {
                        anchors.centerIn: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: "black"
                        text: qsTr("Test")
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
//                            trainingSys.testVJ("/Users/apple/Desktop/Courses/Penguin/training_data/positives/validation",
//                                               "/Users/apple/Desktop/Courses/Penguin/training_data/negatives",
//                                               8,
//                                               24);

                            trainingSys.testRF("/Users/apple/Desktop/Courses/Penguin/training_data/positives/validation_8x24",
                                               "/Users/apple/Desktop/Courses/Penguin/training_data/negatives");
                        }
                    }
                }
            }
        }

        Tab {
            title: "Tracking"

            Item {
                Item {
                    id: condensationParaPanel

                    anchors.top: parent.top
                    anchors.topMargin: 21
                    anchors.left: parent.left
                    anchors.leftMargin: 20

                    ColumnLayout {
                        Layout.fillWidth: true

                        CheckBox {
                            id: particleCheckbox
                            text: "Show particles"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                penguinViewer.showSMCProcess = checked;
                            }
                        }

                        CheckBox {
                            id: densityMapCheckbox
                            text: "Show density map"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                penguinViewer.showConfidenceMap = checked;
                            }
                        }

                        CheckBox {
                            id: trajectoryCheckbox
                            text: "Show trajectory"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                penguinViewer.showTrajectories = checked;
                            }
                        }
                    }
                }
            }
        }

        Tab {
            title: "Tool"

            Item {
                Item {
                    id: toolSwitchPanel

                    anchors.top: parent.top
                    anchors.topMargin: 21
                    anchors.left: parent.left
                    anchors.leftMargin: 20

                    ColumnLayout {
                        Layout.fillWidth: true

                        CheckBox {
                            id: sizeToolCheckbox
                            text: "Enable sizeing tool"
                            checked: true
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                sizeRuler.visible = checked;

                                if (!sizeRuler.visible)
                                    sizeRuler.init();
                            }
                        }

                        CheckBox {
                            id: labellingToolCheckbox
                            text: "Enable labelling"
                            checked: false
                            Layout.minimumWidth: 100

                            onCheckedChanged: {
                                surveillanceArea.enableManualLabeling = checked;
                            }
                        }
                    }
                }
            }
        }
    }
}
