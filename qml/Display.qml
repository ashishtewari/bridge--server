import QtQuick 1.1

Item {
    id: root
    width: content.width + 50
    height: content.height + 10

    property int dealer
    property int vulnerable
    property int declarer
    property string contract
    property int doubled

    Rectangle {
        id: background
        color: "black"
        radius: 10
        opacity: 0.3
        anchors.fill: parent
    }

    Behavior on height { NumberAnimation { duration: 250 } }

    property int fontSize: 16
    property color fontColor: "white"

    Column {
        id: content
        anchors {
            top: parent.top
            topMargin: 5
            left: parent.left
            leftMargin: 10
        }
        spacing: 5

        Text {
            visible: root.dealer!=-1
            text: visible ? "Dealer: " + ["North", "East", "South", "West"][root.dealer] : ""
            font.pixelSize: root.fontSize
            color: root.fontColor
        }

        Text {
            visible: root.vulnerable!=-1
            text: visible ? "Vulnerable: " + ["North-South", "East-West", "None", "All"][root.vulnerable] : ""
            font.pixelSize: root.fontSize
            color: root.fontColor
        }

        Text {
            visible: root.declarer!=-1
            text: visible ? "Declarer: " + ["North", "East", "South", "West"][root.declarer] : ""
            font.pixelSize: root.fontSize
            color: root.fontColor
        }

        Row {
            visible: root.contract!=""
            spacing: 10
            Text {
                text: "Contract: "
                font.pixelSize: root.fontSize
                color: root.fontColor
            }
            Bid {
                id: bid
                num: parseInt(root.contract.split(",")[0])
                suit: parseInt(root.contract.split(",")[1])
                fontColor: "white"
                width: 20
                height: 22
            }
            Text {
                text: root.doubled==2 ? " *" : root.doubled==4 ? " **" : ""
                font.pixelSize: root.fontSize
                color: root.fontColor
            }
        }
    }
}
