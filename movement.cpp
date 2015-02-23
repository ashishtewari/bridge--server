#include "movement.h"

Movement::Movement(int pairs, QObject *parent) :
    QObject(parent), m_pairs(pairs)
{
}
