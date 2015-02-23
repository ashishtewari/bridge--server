import QtQuick 1.1

Item {
    id: root
    height: 32
    property alias text: statusText.text
    property alias showText: statusText.visible
    property alias showCloseBtn: closeBtn.visible
    property alias bgColor: backgrounds.currentColor
    signal settingsTriggered
    signal close

    Rectangle {
        id: background
        color: "black"
        opacity: 0.4
        anchors.fill: parent
    }

    Row {
        id: backgrounds
        anchors { left: parent.left; leftMargin: 20 }
        height: parent.height
        property string currentColor: "green"
        spacing: 2

        Repeater {
            model: ListModel {
                ListElement { color: "green" }
                ListElement { color: "brown" }
                ListElement { color: "red" }
            }
            Image {
                source: "images/backgrounds/"+color+"_thumb.png"
                height: parent.height
                fillMode: Image.PreserveAspectFit
                smooth: true
                MouseArea {
                    anchors.fill: parent
                    onClicked: backgrounds.currentColor=color
                }
            }
        }
    }

    Text {
        id: statusText
        color: "white"
        anchors {
            fill: parent
            leftMargin: 10
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 14
    }

    Button {
        id: closeBtn
        text: "Close"
        anchors.centerIn: parent
        onClicked: parent.close()
    }

    Rectangle {
        id: iconHighlight
        color: "white"
        opacity: 0.5
        radius: 5
        anchors.fill: settingsIcon
        visible: mouseArea.pressed
    }

    Image {
        id: settingsIcon
        source: "images/settings.png"
        anchors {
            right: parent.right
            rightMargin: 10
            bottom: parent.bottom
            bottomMargin: 5
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: root.settingsTriggered()
        }
    }
}
