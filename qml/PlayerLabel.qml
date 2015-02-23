import QtQuick 1.1

Text {
    property Item player
    text: ["North", "East", "South", "West"][player.num]
    visible: phase>0 && phase<3
    font.pixelSize: 16
    color: "white"
    anchors {
        bottom: player.top
        bottomMargin: 5
        left: player.left
        leftMargin: 10
    }
}
