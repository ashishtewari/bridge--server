#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <QObject>
#include <QList>

class Movement : public QObject
{
    Q_OBJECT
public:
    enum Type {
        Mitchell,
        Howell
    };
    explicit Movement(int pairs, QObject *parent = 0);
    virtual int NSPair(int round, int table) = 0;
    virtual int EWPair(int round, int table) = 0;
    virtual int board(int round, int table) = 0;
    virtual QList<int> NSPairs() = 0;
    virtual QList<int> EWPairs() = 0;
    //virtual int NSTable(int round, int pair) = 0;
    //virtual int EWTable(int round, int pair) = 0;

    inline int pairs() { return m_pairs; }
    inline int tables() { return m_tables; }
    inline int rounds() { return m_rounds; }
    inline int boards() { return m_rounds; }
    inline Type type() { return m_type; }
    inline void setType(Type t) { m_type = t; }
    
signals:
    
public slots:

protected:
    int m_pairs;
    int m_tables;
    int m_rounds;
    Type m_type;
    
};

#endif // MOVEMENT_H
