import QtQuick 1.1

Grid {
    id: player

    property real cardWidth: 100
    property real cardHeight: 134

    property int num
    property int orientation
    property alias cards: cardSet
    property alias count: cardSet.count
    signal cardClicked(string indexString, string cardNoString)

    width: orientation==Qt.Horizontal ? 500 : cardWidth
    height: orientation==Qt.Vertical ? 500 : cardHeight
    spacing: orientation==Qt.Horizontal ? Math.min(((width-cardWidth)/(cards.count-1))-cardWidth,-cardWidth/2) : Math.min(((height-cardHeight)/(cards.count-1))-cardHeight, -cardHeight*2/3)

    rows: orientation==Qt.Horizontal ? 1 : cards.count
    columns: orientation==Qt.Horizontal ? cards.count : 1
    move: Transition { NumberAnimation { properties: "x,y"; duration: 250 } }

    Repeater {
        id: cardSet
        model: ListModel{}
        Card {
            cardNo: card
        }
    }

    function destroyLater(index) {
        destroyTimer.toDestroy = index;
        destroyTimer.start();
    }

    Timer {
        id: destroyTimer
        interval: 150
        property int toDestroy
        onTriggered: cards.model.remove(toDestroy);
    }
}
