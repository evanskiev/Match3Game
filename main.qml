import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import mymodel 1.0

ApplicationWindow {
    id: main
    visible: true
    width: 800
    height: 800

    menuBar: MenuBar {
            Menu {
                visible: false
                title: "Menu"
                MenuItem {
                    text: "New Game"
                    shortcut: "Ctrl+N"
                    onTriggered: { mymodel.newGame() }
                }
                MenuItem {
                    text: "Quit"
                    shortcut: "Ctrl+Q"
                    onTriggered: Qt.quit()
                }
            }
        }

    statusBar: StatusBar {
                    Label { text: "New Game : Ctrl + N" }
    }

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

                visible: index > mymodel.size - 1 ? false : true

                Image {
                    id: imagea
                    anchors.centerIn: parent
                    anchors.fill: parent
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
        height: 50
        width: parent.width
        color: "transparent"
        anchors.horizontalCenter: parent.hrizontalcenter

        Text {
            id: scoreFild
            text: "SCORE - " + mymodel.score + " / " + mymodel.minScore
            font.pixelSize: 25
            font.bold: true
            color: "white"

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 50

        }

        Text {
            id: moveFild
            text: "MOVES - " + mymodel.moveCounter + " / " + mymodel.maxMoves
            font.pixelSize: 25
            font.bold: true
            color: "white"

            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 50

        }
    }

    Rectangle {
        id: gameField
        width:  mymodel.columns >= (mymodel.rows-1) ? 700 :  mymodel.columns * view.cellWidth
        height: (mymodel.rows-1) >= mymodel.columns ? 700 :  (mymodel.rows-1) * view.cellWidth
        anchors.centerIn: parent
        color: "transparent"

        GridView {
            id: view
            focus: true
            interactive: false

            cellHeight: 700/mymodel.cellSize
            cellWidth: 700/mymodel.cellSize
            anchors.fill: parent

            model: MyModel {
                id: mymodel

                property var size: (mymodel.rows-1) * mymodel.columns
                property var cellSize: mymodel.rows-1 > mymodel.columns ?  (mymodel.rows-1) : mymodel.columns

                onVictory: {
                    messageDialog.title = "Victory"
                    messageDialog.informativeText = "Congratulations, you are the winner!"
                    messageDialog.open()
                }

                onDefeat: {
                    messageDialog.title = "Defeat"
                    messageDialog.informativeText = "Congratulations, you are not winner!"
                    messageDialog.open()
                }
            }

            delegate: mydelegate

            verticalLayoutDirection: GridView.BottomToTop

            moveDisplaced: Transition {
                NumberAnimation { easing.overshoot: 1.2; easing.type: Easing.InOutBack; properties: "x,y"; duration: 200 }
            }

            move: Transition {
                ParallelAnimation {
                    NumberAnimation { easing.overshoot: 1.2; easing.type: Easing.OutCirc; properties: "x,y"; duration: 200 }
                    NumberAnimation { properties: "rotation"; from: 0; to: 360; duration: 150 }
                }
            }

            remove: Transition {
                    NumberAnimation { property: "opacity"; to: 0; duration: 200 }
            }

            add: Transition {
                    NumberAnimation { property: "opacity"; to: 1; duration: 300 }
            }

        }
    }

    MessageDialog {
        id: messageDialog
        visible: false
        modality: Qt.WindowModal
        onAccepted: { mymodel.newGame() }
    }
}
