import QtQuick 1.1

Item {
    id: box
    clip: true

    property int coverNo: covers.currentIndex
    property int deckNo: decks.currentIndex

    MouseArea { anchors.fill: parent }

    Rectangle {
        id: background
        color: "black"
        opacity: 0.6
        anchors.fill: parent
    }

    Behavior on height { NumberAnimation { duration: 200 } }

    Text {
        text: "Choose a deck:"
        color: "white"
        font.pixelSize: 16
        anchors {
            top: parent.top
            topMargin: 5
        }
        x: container.x + covers.x
    }

    Text {
        text: "Choose a deck style:"
        color: "white"
        font.pixelSize: 16
        anchors {
            top: parent.top
            topMargin: 5
        }
        x: container.x + decks.x
    }

    Row {
        id: container
        anchors { centerIn: parent; verticalCenterOffset: 10 }
        spacing: 30

        Row {
            id: covers
            spacing: 20
            anchors.verticalCenter: parent.verticalCenter
            property int currentIndex: 0

            Repeater {
                model: 3
                Image {
                    source: "images/deck/covers/cover"+(index+1)+"_thumb.png"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: covers.currentIndex = index
                    }
                }
            }
        }

        Image {
            source: "images/line.png"
            smooth: true
            height: parent.height
        }

        Row {
            id: decks
            spacing: 2
            anchors.verticalCenter: parent.verticalCenter
            property int currentIndex: 2

            Repeater {
                model: 4
                Item {
                    width: coverThumb.paintedWidth+deckThumb.paintedWidth
                    height: deckThumb.paintedHeight
                    Image {
                        id: coverThumb
                        source: "images/deck/covers/cover"+(box.coverNo+1)+"_thumb.png"
                        anchors.left: parent.left
                    }

                    Image {
                        id: deckThumb
                        source: "images/deck/deck"+(index+1)+"/sample.png"
                        anchors.left: coverThumb.right
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: decks.currentIndex = index
                    }
                }
            }
        } // row of decks
    } // container row
}
