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
                visible: index > 99 ? false : true

                Image {
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
            anchors.leftMargin: 25

        }

        Text {
            id: moveFild
            text: "MOVES - " + mymodel.moveCounter + " / " + mymodel.maxMoves
            font.pixelSize: 25
            font.bold: true
            color: "white"

            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 25

        }
    }

    Rectangle {
        id: gameField
        width: mymodel.columns * 70
        height: (mymodel.rows - 1) * 70
        anchors.centerIn: parent
        color: "transparent"

        GridView {
            id: view
            focus: true
            interactive: false

            cellHeight: gameField.height/mymodel.columns
            cellWidth: gameField.height/mymodel.columns
            anchors.fill: parent
            //        anchors.margins: 5
            model: MyModel {
                id: mymodel

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
                    NumberAnimation { easing.overshoot: 1.2; easing.type: Easing.InOutBack; properties: "x,y"; duration: 200 }
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
