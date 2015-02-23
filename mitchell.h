#ifndef MITCHELL_H
#define MITCHELL_H

#include "movement.h"

class Mitchell : public Movement
{
public:
    Mitchell(int pairs);
    int NSPair(int round, int table);
    int EWPair(int round, int table);
    int board(int round, int table);
    QList<int> NSPairs();
    QList<int> EWPairs();
    /*int NSTable(int round, int pair);
    int EWTable(int round, int pair);*/

};

#endif // MITCHELL_H
