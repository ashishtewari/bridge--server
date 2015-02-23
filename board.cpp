#include "board.h"
#include <QTime>

Board::Board(int boardNo)
    : m_boardNo(boardNo)
{
    // Shuffle the cards
    // prepare the deck
    QList<int> deck;
    for (int i=0; i<52; i++) {
        deck << i;
    }

    // prepare the pockets
    for (int i=0; i<4; i++) {
        m_cards << QList<int>();
    }

    // shuffle cards into pockets
    for (int i=0; i<13; i++) {
        for (int j=0; j<4; j++) {
            m_cards[j].append( deck.takeAt(qrand()%deck.size()) );
        }
    }

    int n = boardNo%16;

    // Determine the dealer
    m_dealer = n%4;

    //Determine the vulnerable team
    if (n==0 || n==7 || n==10 || n==13)
        m_vulnerableTeam = None;
    else if (n==1 || n==4 || n==11 || n==14)
        m_vulnerableTeam = NorthSouth;
    else if (n==2 || n==5 || n==8 || n==15)
        m_vulnerableTeam = EastWest;
    else
        m_vulnerableTeam = All;
}

