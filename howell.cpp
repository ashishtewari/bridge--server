#include "howell.h"

Howell::Howell(int pairs) :
    Movement(pairs, 0)
{
    setType(Movement::Howell);
    m_tables = m_pairs/2;
    m_rounds = m_pairs-1;

    switch(m_pairs) {
    case 2:
        m_startNS << 1;
        m_startBoard << 0;
        m_startEW << 0;
    case 4:
        m_startNS << 3 << 1;
        m_startBoard << 0 << 0;
        m_startEW << 0 << 2;
        break;
    case 6:
        m_startNS << 5 << 3 << 1;
        m_startBoard << 0 << 1 << 3;
        m_startEW << 0 << 2 << 4;
        break;
    case 8:
        m_startNS << 7 << 5 << 3 << 6;
        m_startBoard << 0 << 1 << 2 << 4;
        m_startEW << 0 << 4 << 1 << 2;
        break;
    case 10:
        m_startNS << 9 << 4 << 2 << 8 << 5;
        m_startBoard << 2 << 1 << 0 << 2 << 4;
        m_startEW << 0 << 1 << 6 << 7 << 3;
        break;
    case 12:
        m_startNS << 11 << 10 << 8 << 2 << 7 << 6;
        m_startBoard << 0 << 2 << 6 << 7 << 8 << 10;
        m_startEW << 0 << 5 << 4 << 1 << 9 << 3;
        break;
    case 14:
        m_startNS << 4 << 1 << 8 << 13 << 7 << 6 << 5;
        m_startBoard << 0 << 1 << 2 << 3 << 4 << 5 << 6;
        m_startEW << 11 << 3 << 9 << 0 << 12 << 10 << 2;
        break;
    case 16:
        m_startNS << 11 << 15 << 9 << 5 << 12 << 3 << 13 << 8;
        m_startBoard << 0 << 1 << 2 << 3 << 4 << 8 << 9 << 11;
        m_startEW << 6 << 0 << 7 << 1 << 4 << 2 << 10 << 14;
        break;
    case 18:
        m_startNS << 15 << 10 << 2 << 14 << 17 << 12 << 11 << 4 << 1;
        m_startBoard << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7;
        m_startEW << 6 << 13 << 3 << 7 << 0 << 8 << 5 << 9 << 16;
        break;
    case 20:
        m_startNS << 14 << 3 << 19 << 8 << 13 << 2 << 10 << 18 << 15 << 12;
        m_startBoard << 0 << 1 << 3 << 4 << 7 << 8 << 9 << 10 << 12 << 14;
        m_startEW << 5 << 16 << 0 << 11 << 6 << 17 << 9 << 1 << 4 << 7;
        break;
    }
}

int Howell::NSPair(int round, int table)
{
    if (m_startNS[table] == m_pairs-1) return m_pairs-1;
    else return (m_startNS[table]+round)%m_rounds;
}

int Howell::EWPair(int round, int table)
{
    if (m_startEW[table] == m_pairs-1) return m_pairs-1;
    else return (m_startEW[table]+round)%m_rounds;
}

int Howell::board(int round, int table)
{
    if (m_pairs==6 && table>0) {
        if (table==1) {
            if (round==0 || round==2) return 1;
            else if (round==1 || round==3) return 2;
            return 4;
        }
        else if (table==2) {
            if (round<2) return 3;
            else if (round<4) return 0;
            return 4;
        }
    }
    return (m_startBoard[table]+round)%m_rounds;
}

QList<int> Howell::NSPairs()
{
    QList<int> t;
    for (int i=0; i<m_pairs; i++) t << i;
    return t;
}

QList<int> Howell::EWPairs()
{
    return NSPairs();
}
