import QtQuick 1.1

Item {
    id: board
    property int tricks: 0
    property int opponentTricks: 0
    property alias scoreTiles: handRepeater

    Behavior on opacity { NumberAnimation { duration: 250 } }

    function updateTricksLater(we_win) {
        updateTricksTimer.we_win = we_win;
        updateTricksTimer.start();
    }

    function updateTricks(we_win) {
        if (we_win) {
            tricks++;
        } else {
            opponentTricks++;
        }
        scoreTiles.model.append({"score": we_win});
    }

    Timer {
        id: updateTricksTimer
        interval: moveCardsBackTimer.interval
        property bool we_win
        onTriggered: updateTricks(we_win);
    }

    Row {
        id: hands
        anchors {
            bottom: scoreTile.top
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
        }
        spacing: {
            var model = handRepeater.model;
            if (model.count==0) return 0;
            var c = model.get(model.count-1).score ? 100 : 134;
            return (width-c)/12-100;
        }
        height: 80
        Repeater {
            id: handRepeater
            model: ListModel {}
            Image {
                source: "images/score_tiles/"+ (score ? "v"+(coverNo+1) :
                                                index==handRepeater.count-1 ? "h"+(coverNo+1) : "h"+(coverNo+1)+"_x") + ".png"
                y: score ? 0 : hands.height-height
            }
        }
    }

    Rectangle {
        id: scoreTile
        color: "white"
        opacity: 0.3
        radius: 5
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 30
    }

    property int fontSize: 14

    Text {
        id: scoreLabel
        text: ["NS","EW"][me.num%2] + ": " + tricks
        font.pixelSize: board.fontSize
        font.bold: true
        anchors {
            verticalCenter: scoreTile.verticalCenter
            left: scoreTile.left
            leftMargin: 10
        }
    }

    Text {
        id: opponentScore
        text: ["EW","NS"][me.num%2] + ": " + opponentTricks
        font.pixelSize: board.fontSize
        font.bold: true
        anchors {
            verticalCenter: scoreTile.verticalCenter
            right: scoreTile.right
            rightMargin: 10
        }
    }
}
