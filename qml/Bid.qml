import QtQuick 1.1

Item {
    id: bidItem
    property int num
    property int suit
    property string specialText
    property bool special: specialText!=""
    property color fontColor: "black"
    implicitWidth: container.width
    implicitHeight: container.height

    Row {
        id: container
        anchors.centerIn: parent
        spacing: 1

        Text {
            id: bidNo
            text: special ? specialText : bidItem.num
            font.pixelSize: 20
            color: bidItem.fontColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            height: bidSuit.visible ? bidSuit.height : implicitHeight
        }

        Text {
            id: noTrump
            text: bidItem.suit==4 && !special ? "NT" : ""
            font.pixelSize: 18
            color: bidItem.fontColor
            height: bidNo.height
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Image {
            id: bidSuit
            source: bidItem.suit<4 ? "images/suits/"+["C","D","H","S"][bidItem.suit]+".png" : ""
            visible: bidItem.suit!=4 && !special
        }
    }
}
