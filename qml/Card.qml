import QtQuick 1.1
import "logic.js" as Logic

Image {
    property int cardNo: -1
    property string cardString: cardNo==-1 ? "" : Logic.getCardString(cardNo).toUpperCase()

    source: "images/deck/"+getCardImage()
    smooth: true

    function getCardImage() {
        if (cardString=="") {
            var n = canvas.coverNo+1;
            return "covers/"+n+".png";
        } else {
            var n = canvas.deckNo+1;
            if (n==1) return "deck1/"+cardString+".png";
            else {
                var sign = cardString[0];
                if (sign=="J" || sign=="Q" || sign == "K") return "deck"+n+"/"+cardString+".png";
                else return "common/"+cardString+".png";
            }
        }
    }

    Behavior on x { NumberAnimation { duration: 150 } }
    Behavior on y { NumberAnimation { duration: 150 } }
}
