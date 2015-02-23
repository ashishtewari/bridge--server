import QtQuick 1.1

Item {

    Rectangle {
        id: background
        color: "white"
        opacity: 0.15
        radius: 8
        anchors.fill: parent
    }

    Rectangle {
        id: headerBg
        color: "white"
        opacity: 0.3
        radius: 5
        anchors {
            top: parent.top
            topMargin: 7
            left: parent.left
            leftMargin: 7
            right: parent.right
            rightMargin: 7
        }
        height: 30
    }

    Row {
        id: header
        anchors.fill: headerBg
        Repeater {
            id: repeater
            model: ["North", "East", "South", "West"]
            Text {
                text: modelData
                font.pixelSize: 18
                width: header.width/repeater.count
                height: header.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    ListModel { id: bidsModel }

    GridView {
        id: bids
        anchors {
            top: header.bottom
            topMargin: 10
            bottom: parent.bottom
            left: header.left
            right: header.right
        }
        clip: true
        cellWidth: (width/4)-1
        cellHeight: height/4

        model: bidsModel
        delegate: Bid {
            num: bidNo
            suit: bidSuit
            specialText: {
                if (bidSpecial == "pass") return "<font color=\"#006400\">Pass</font>";
                if (bidSpecial == "x") return "<font color=\"#000080\">X</font>";
                if (bidSpecial == "xx") return "<font color=\"red\">XX</font>";
                return "";
            }
            width: bids.cellWidth
            height: bids.cellHeight
            visible: bidNo!=-1
        }
    }

    function append(bid) {
        if (bid=="") {
            bids.model.append({"bidNo": -1, "bidSuit": 0, "bidSpecial": ""});
        } else if (bid=="pass" || bid=="x" || bid=="xx") {
            bids.model.append({"bidNo": 0, "bidSuit": 0, "bidSpecial": bid});
        } else {
            var bidSplit = bid.split(",");
            bids.model.append({"bidNo": parseInt(bidSplit[0]), "bidSuit": parseInt(bidSplit[1]), "bidSpecial": ""});
        }
        bids.positionViewAtEnd();
    }
}
