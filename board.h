#ifndef BOARD_H
#define BOARD_H

#include "enums.h"
#include <QList>

class Board
{
public:
    Board(int boardNo = 0);

    inline QList<int> cardsOf(int player) { return m_cards.at(player); }
    inline int boardNo() { return m_boardNo; }
    inline int dealer() { return m_dealer; }
    inline Team vulnerableTeam() { return m_vulnerableTeam; }

private:
    QList< QList<int> > m_cards;
    int m_boardNo;
    int m_dealer;
    Team m_vulnerableTeam;
};

#endif // BOARD_H
