import QtQuick 2.3
import QtQuick.Window 2.2
import mymodel 1.0

Window {
    id: main
    visible: true
    width: 800
    height: 800

    Image {
        anchors.fill: parent
        id: name
        source: "qrc:/sprites/background.png"
        width: 1024
        height: 768
    }

    Component {
        id: mydelegate

        Item {
            width: view.cellWidth
            height: view.cellHeight
            Rectangle {
                color: "transparent"
                anchors.centerIn: parent
                anchors.fill: parent
                width: 50
                height: 50

                Image {
                    anchors.centerIn: parent
                    anchors.fill: parent
                    //                    width: view.cellWidth
                    //                    height: view.cellHeight
                    source: path
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        mymodel.activate(index)
                    }
                }
            }

        }
    }

    Rectangle {
        id: gameField
        width: 700
        height: 700
        anchors.centerIn: parent
        color: "transparent"
        GridView {
            id: view
            focus: true

            cellHeight: gameField.height/mymodel.columns
            cellWidth: gameField.height/mymodel.columns
            anchors.fill: parent
            //        anchors.margins: 5
            model: MyModel {
                id: mymodel
            }

            delegate: mydelegate

            verticalLayoutDirection: GridView.BottomToTop

            move: Transition {
                NumberAnimation { easing.overshoot: 1.2; easing.type: Easing.InOutBack; properties: "x,y"; duration: 300 }
            }

            remove: Transition {
                ParallelAnimation {
//                    NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 200 }
                    NumberAnimation { easing.type: Easing.InOutQuart; properties: "y"; to: 150; duration: 250 }
                }
            }

            add: Transition {
                ParallelAnimation {
                    NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 300 }

                    NumberAnimation { easing.type: Easing.OutInQuad; properties: "y"; to: 150; duration: 250 }
                }
            }

        }
    }
}
