import QtQuick 2.2
import QtQuick.Controls 1.2

Item {
    id: trainingNavigatorImp
    width: 300
    height: 450
    clip: true

    //    state: "default"

    //    states: [
    //        State {
    //            name: "default"
    //            PropertyChanges {
    //                target: object

    //            }
    //        }
    //    ]

    StackView {
        id: stackView
        width: 300
        height: parent.height - navigatorTitleBar.height
        anchors.top: navigatorTitleBar.bottom
    }

    Image {
        id: navigatorTitleBar
        width: sourceSize.width
        height: sourceSize.height
        smooth: true
        fillMode: Image.PreserveAspectFit
        anchors.horizontalCenter: parent.horizontalCenter
        source: "qrc:///qml/icons/titleBar_1.png"
    }

    Component {
        id: defaultPanel
        Rectangle {
            color: "#fafafa"
            width: trainingNavigatorImp.width
            height: trainingNavigatorImp.height - navigatorTitleBar.height

            Text {
                anchors.centerIn: parent
                text: qsTr("Setup Classifier")
            }

            Rectangle {
                id: bottomToolBar
                color: "darkGray"
                width: 300
                height: 30
                anchors.bottom: parent.bottom

                Image {
                    id: prevImgBtn
                    source: "qrc:///qml/icons/left_round.png"
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.horizontalCenter
                    anchors.rightMargin: 10

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                        }
                    }
                }

                Image {
                    id: nextImgBtn
                    source: "qrc:///qml/icons/right_round.png"
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.horizontalCenter
                    anchors.leftMargin: 10

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            stackView.push(classifierSelectionPanel)
                        }
                    }
                }
            }
        }
    }

    Component {
        id: classifierSelectionPanel
        Rectangle {
            color: "#fafafa"
            width: trainingNavigatorImp.width
            height: trainingNavigatorImp.height - navigatorTitleBar.height

            Text {
                anchors.centerIn: parent
                text: qsTr("Setup Classifier")
            }

            ListModel {
                id: classifierModel
                ListElement {
                    name: "Viola-Jones"
                    portrait: "qrc:///qml/icons/randomForest.png"
                }
                ListElement {
                    name: "Random Forest"
                    portrait: "qrc:///qml/icons/randomForest.png"
                }
                ListElement {
                    name: "DPMs"
                    portrait: "qrc:///qml/icons/randomForest.png"
                }
            }

            GridView {
                id: classifierGridView
                width: parent.width
                height: parent.height - bottomToolBar.height
                cellWidth: parent.width/2; cellHeight: parent.width/2
                model: classifierModel
                delegate: Rectangle {
                    width: classifierGridView.cellWidth
                    height: classifierGridView.cellHeight
                    color: "darkGray"

                    Image {
                        id: classifierLogo
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 6
                        smooth: true
                        source: portrait
                    }

                    Text {
                        id: classifierName
                        text: qsTr(name)
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 6
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

            Rectangle {
                id: bottomToolBar
                color: "darkGray"
                width: 300
                height: 30
                anchors.bottom: parent.bottom

                Image {
                    id: prevBtn
                    source: "qrc:///qml/icons/left_round.png"
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.horizontalCenter
                    anchors.rightMargin: 10

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            stackView.pop()
                        }
                    }
                }

                Image {
                    id: nextBtn
                    source: "qrc:///qml/icons/right_round.png"
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.horizontalCenter
                    anchors.leftMargin: 10

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            stackView.push()
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        stackView.push(defaultPanel)
    }
}
