#include "mitchell.h"

Mitchell::Mitchell(int pairs) :
    Movement(pairs)
{
    setType(Movement::Mitchell);
    m_tables = m_pairs/2;
    m_rounds = (m_pairs+1)/2;
}

int Mitchell::NSPair(int round, int table)
{
    Q_UNUSED(round)
    return table;
}

int Mitchell::EWPair(int round, int table)
{
    return (m_rounds+table-round)%m_rounds + m_tables;
}

int Mitchell::board(int round, int table)
{
    if (m_rounds%2==1) {
        return (round+table)%m_rounds;
    }
    else {
        return (round+table+(table>=m_rounds/2))%m_rounds;
    }
}

QList<int> Mitchell::NSPairs()
{
    QList<int> t;
    for (int i=0; i<m_tables; i++) t << i;
    return t;
}

QList<int> Mitchell::EWPairs()
{
    QList<int> t;
    for (int i=m_tables; i<m_pairs; i++) t << i;
    return t;
}

/*int Mitchell::NSTable(int round, int pair)
{
    Q_UNUSED(round)
    return pair;
}

int Mitchell::EWTable(int round, int pair)
{
    int t = (round+pair)%m_tables;
    if (t<m_tables) return t;
    else return -1; // sit out
}*/
