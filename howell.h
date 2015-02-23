#ifndef HOWELL_H
#define HOWELL_H

#include "movement.h"

class Howell : public Movement
{
public:
    Howell(int pairs);
    int NSPair(int round, int table);
    int EWPair(int round, int table);
    int board(int round, int table);
    QList<int> NSPairs();
    QList<int> EWPairs();

    static int min_pairs() { return 2; }
    static int max_pairs() { return 20; }

private:
    QList<int> m_startNS;
    QList<int> m_startBoard;
    QList<int> m_startEW;
};

#endif // HOWELL_H
