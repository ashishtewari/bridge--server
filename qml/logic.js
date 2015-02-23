var bidPassCount = 0;
var players = new Array(4);
var centerCards = new Array(4);

function updateDB() {
    players[0] = topPlayer;
    players[1] = rightPlayer;
    players[2] = me;
    players[3] = leftPlayer;
}

function cards(i) {
    return players[i].cards;
}

function setStatus(text) {
    statusBar.text = text;
}

function getCardString(cardNo) {
    var cardString = "";
    // Card no
    var n = (cardNo%13);
    if (n<9)
        cardString += (n+2).toString();
    else
        cardString += ["J", "Q", "K", "A"][n-9];

    // Card suit
    var suit = Math.floor(cardNo/13);
    // Make sure having S before H does not arise bugs
    cardString += ["C", "D", "H", "S"][suit];

    return cardString;
}

function sortFunction(a, b) {
    var x = Math.floor(a/13);
    var y = Math.floor(b/13);
    return ((x==2 && y==3) || (x==3 && y==2)) ? b-a : a-b;
}

var cardsReceivedCount=0;
function showCards(player, the_cards) {
    for (var k=0; k<13; k++) {
        the_cards[k] = parseInt(the_cards[k]);
    }
    the_cards = the_cards.sort(sortFunction);

    cards(player).model.clear();
    for (var i in the_cards) {
        var cardNo = the_cards[i];
        cards(player).model.append({"card": cardNo});
    }

    if (++cardsReceivedCount == 4) {
        biddingBoard.visible = true;
        for (i=0; i<dealer; i++)
            biddingBoard.append("");
    }
}

// Last non-(pass/x/xx) bid
var lastValidBid = "";
// Last non-pass bid
var lastSemiValidBid = "";
var trumpSuit;

var lastCard=-1;
var currentSuit=-1;
var trickWinner;
var playedCardsCount = 0;

function showMove(player, type, data) {
    if (type == 0) {
        biddingBoard.append(data);
        if (data=="pass") {
            bidPassCount++;
            if (bidPassCount==3) {
                stopBidTimer.start();
                trumpSuit = parseInt(lastValidBid.split(":")[1].split(",")[1]);
                phase = 2;
            }
        }
        else {
            bidPassCount=0;
            lastSemiValidBid = player+":"+data;
            if (data=="x") {
                doubled = 2;
            } else if (data=="xx") {
                doubled = 4;
            } else {
                doubled = 1;
                updateDeclarer(player, data);
                lastValidBid = lastSemiValidBid;
            }
        }
    } else if (type == 1) {
        var index;
        var cardNo;
        var item;
        if (player==dummy) {
            index = parseInt(data);
            item = cards(player).itemAt(index);
            cardNo = item.cardNo;
        } else {
            cardNo = parseInt(data);
            var model = cards(player).model;
            for (index=0; index<model.count; index++) {
                if (model.get(index).card == cardNo)
                    break;
            }
            item = cards(player).itemAt(index);
        }

        moveCardToCenter(item, player);
        centerCards[player] = index;

        if (isGreater(cardNo, lastCard)) {
            lastCard = cardNo;
            trickWinner = player;
        }

        playedCardsCount++;
        if (playedCardsCount<4) {
            if (playedCardsCount==1) {
                currentSuit = Math.floor(cardNo/13);
            }
        } else if (playedCardsCount==4) {
            playedCardsCount = 0;
            lastCard = -1;
            currentSuit = -1;
            moveCardsBackTimer.start();
            var left = cards(me.num).count-1;
            if (left == 0) {
                phase = 3;
                scoreTimer.start();
            }
        }
    } else if (type == 2) {
        showMove(dummy, 1, data);
    }
}

var teamDeclarers = [-1, -1];
var teamLastBidSuit = [-1, -1];
function updateDeclarer(player, data) {
    var team = player%2;
    var suit = data.split(",")[1];
    if (teamDeclarers[team] == -1 || suit != teamLastBidSuit[team]) {
        teamDeclarers[team] = player;
        teamLastBidSuit[team] = suit;
    }
    declarer = teamDeclarers[team];
    dummy = (declarer+2)%4;
}

function moveCardToCenter(card, player) {
    var x;
    var y;
    var margins = 20;
    if (player==me.num) {
        x = centerShadow.width/2-card.width/2;
        y = centerShadow.height-card.height-margins;
    } else if (player==leftPlayer.num) {
        x = margins;
        y = centerShadow.height/2-card.height/2;
    } else if (player==topPlayer.num) {
        x = centerShadow.width/2-card.width/2;
        y = margins;
    } else if (player==rightPlayer.num) {
        x = centerShadow.width-card.width-margins;
        y = centerShadow.height/2-card.height/2;
    }
    var pos = centerShadow.mapToItem(card.parent, x, y);
    card.x = pos.x; card.y = pos.y;
}

function moveCardsToPlayers() {
    for (var i=0; i<4; i++) {
        var card = cards(i).itemAt(centerCards[i]);
        card.x = 0; card.y = 0;
        players[i].destroyLater(centerCards[i]);
    }
    scoreBoard.updateTricks(trickWinner==me.num || trickWinner==(me.num+2)%4);
}

function isGreater(now, last) {
    if (last==-1) return true;
    if (now==-1) return false;
    var suit = Math.floor(now/13);
    if (suit != Math.floor(last/13)) {
        return suit==trumpSuit;
    } else {
        return now>last;
    }
}

function displayScore() {
    var score = computeScore();
    if (score>0)
        scoreMessage.text = "North-South wins "+score+" points" + scoreMessage.text;
    else
        scoreMessage.text = "East-West wins "+-score+" points" + scoreMessage.text;
}

function computeScore() {
    var contract = lastValidBid.split(":")[1];
    var bidNo = parseInt(contract.split(",")[0]);
    var bidSuit = ["C", "D", "H", "S", "NT"][trumpSuit];
    var bidMinorSuit = (bidSuit == "C" || bidSuit == "D");
    var isVulnerable = (vulnerable==3 || vulnerable==declarer.num%2);
    var target = 6 + bidNo;
    var score = 0;
    var areWeDeclarer = (declarer%2 == me.num%2);
    var tricks = areWeDeclarer ? scoreBoard.tricks : scoreBoard.opponentTricks;

    // Contract
    if (tricks >= target) {
        if (bidSuit != "NT")
            score += bidNo * (bidMinorSuit ? 20 : 30) * doubled;
        else
            score += (40 + (bidNo-1)*30) * doubled;

        // Making game bonus
        if (score >= 100)
            score += (isVulnerable ? 500 : 300);
        else
            score += 50;
        if (doubled > 1)
            score += 25*doubled;
    }

    // Slam bonus
    if (target == 12 && tricks >= 12)
        score += (isVulnerable ? 750 : 500);
    else if (target==13 && tricks>=13)
        score += (isVulnerable ? 1500 : 1000);

    // Over tricks
    if (tricks > target) {
        if (doubled == 1)
            score += (tricks-target) * (bidMinorSuit ? 20 : 30 );
        else
            score += (tricks-target) * (isVulnerable ? 100:50) * doubled;
    }

    // Under tricks
    if (tricks < target) {
        var penalty = 0;
        if (doubled == 1) {
            penalty = (target-tricks) * (isVulnerable ? 100 : 50);
        }
        else {
            var underTricks = target - tricks;

            // First under trick
            penalty += (isVulnerable ? 100 : 50) * doubled;

            // 2nd & 3rd under trick
            if (underTricks>1) {
                penalty += (isVulnerable ? 150:100) * doubled * (underTricks>2 ? 2:1);

                // Subsequent under tricks
                if (underTricks>3) {
                    penalty += 150*doubled*(underTricks-3);
                }
            }
        }
        score -= penalty;
    }

    return areWeDeclarer ? score : -score;
}
