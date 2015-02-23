import QtQuick 1.1
import "logic.js" as Logic

Image {
    id: canvas
    source: "images/backgrounds/"+statusBar.bgColor+".jpg"

    property int dealer: -1
    property int vulnerable: -1
    property int declarer: -1
    property int doubled: 1
    property int dummy: -1
    // 1 for bidding phase, 2 for tricks phase
    property int phase: 0
    property alias coverNo: settings.coverNo
    property alias deckNo: settings.deckNo
    property bool started: false

    signal gameClicked()
    signal quit()

    Component.onCompleted: {
        Logic.updateDB();
        scoreMessage.text = "Click to start the game";
    }

    function setBoard(dealer, vulnerable) {
        canvas.dealer = dealer;
        canvas.vulnerable = vulnerable;
        phase = 1;
    }
    function showCards(player, cards) { Logic.showCards(player, cards); }
    function showMove(player, type, data) { Logic.showMove(player, type, data); }

    MouseArea {
        anchors.fill: parent
        enabled: phase<3
        onClicked: {
            if (settings.height>0) {
                settings.height=0;
                return;
            }
            scoreMessage.text = "";

            if (moveCardsBackTimer.running) {
                moveCardsBackTimer.stop();
                moveCardsBackTimer.wasRunningAndStopped = true;
            }
            else if (moveCardsBackTimer.wasRunningAndStopped) {
                moveCardsBackTimer.wasRunningAndStopped = false;
                Logic.moveCardsToPlayers();
            }
            gameClicked();
        }
    }

    StatusBar {
        id: statusBar
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        showText: phase>0 && phase <3
        onSettingsTriggered: {
            settings.height = settings.height>0 ? 0 : 150;
        }
        onClose: quit();
    }

    Item {
        id: container
        height: parent.height
        width: height*4/3
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Image {
        id: centerShadow
        source: "images/Center_Shadow.png"
        anchors {
            centerIn: parent
            bottomMargin: statusBar.height
        }
        visible: phase>1
    }

    Display {
        id: display
        visible: phase>0
        anchors {
            top: parent.top
            topMargin: 5
            horizontalCenter: leftPlayer.horizontalCenter
        }

        dealer: canvas.dealer
        vulnerable: canvas.vulnerable
        declarer: phase>1 ? canvas.declarer : -1
        contract: phase>1 ? Logic.lastValidBid.split(":")[1] : ""
        doubled: phase>1 ? canvas.doubled : 0
    }

    BiddingBoard {
        id: biddingBoard
        anchors {
            top: topPlayer.bottom
            topMargin: 15
            horizontalCenter: me.horizontalCenter
        }
        height: 200
        width: 400
        visible: false
        opacity: visible ? 1 : 0
        Behavior on opacity { NumberAnimation { duration: 250 } }
    }

    Timer {
        id: moveCardsBackTimer
        interval: 1000
        onTriggered: Logic.moveCardsToPlayers();
        property bool wasRunningAndStopped: false
    }

    ScoreBoard {
        id: scoreBoard
        anchors {
            top: leftPlayer.bottom
            topMargin: 20
            left: parent.left
            leftMargin: 30
            bottom: statusBar.top
            bottomMargin: 10
            right: me.left
            rightMargin: 20
        }
        visible: phase>1
        opacity: visible ? 1 : 0
    }

    Timer {
        id: stopBidTimer
        interval: 1000
        onTriggered: {
            biddingBoard.visible = false;
        }
    }

    Rectangle {
        color: "black"
        property int margin: -5
        anchors {
            fill: scoreMessage
            topMargin: margin
            bottomMargin: margin
            leftMargin: margin*2
            rightMargin: margin*2
        }
        opacity: visible ? 0.7 : 0
        radius: 7
        visible: scoreMessage.visible
        Behavior on opacity { NumberAnimation { duration: 250 } }
    }
    Text {
        id: scoreMessage
        anchors {
            horizontalCenter: me.horizontalCenter
            bottom: me.top
            bottomMargin: 40
        }
        font.pixelSize: 20
        color: "white"
        visible: text!=""
        opacity: visible
        Behavior on opacity { NumberAnimation { duration: 250 } }
    }

    Timer {
        id: scoreTimer
        interval: moveCardsBackTimer.interval+100
        onTriggered: Logic.displayScore();
    }

    PlayerLabel { player: topPlayer }
    Player {
        id: topPlayer
        num: (me.num+2)%4
        orientation: Qt.Horizontal
        anchors {
            top: parent.top
            topMargin: 30
            horizontalCenter: parent.horizontalCenter
        }
    }

    PlayerLabel { player: leftPlayer }
    Player {
        id: leftPlayer
        num: (me.num+1)%4
        orientation: Qt.Vertical
        anchors {
            left: container.left
            leftMargin: 40
            verticalCenter: parent.verticalCenter
        }
    }

    PlayerLabel { player: rightPlayer }
    Player {
        id: rightPlayer
        num: (me.num+3)%4
        orientation: Qt.Vertical
        anchors {
            right: container.right
            rightMargin: 30
            verticalCenter: parent.verticalCenter
        }
    }

    PlayerLabel { player: me }
    Player {
        id: me
        num: 2
        orientation: Qt.Horizontal
        anchors {
            bottom: statusBar.top
            bottomMargin: 10
            horizontalCenter: parent.horizontalCenter
        }
    }

    SettingsBox {
        id: settings
        width: parent.width
        height: 0
        anchors {
            bottom: statusBar.top
        }
    }
}
