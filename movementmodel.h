#ifndef MOVEMENTMODEL_H
#define MOVEMENTMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include <QPair>
#include "movement.h"
#include "scoresheet.h"

typedef QList<QStandardItem*> QStandardRow;

class MovementModel : public QObject
{
    Q_OBJECT
public:
    explicit MovementModel(int pairs, int boardCount, int movement, int scoring, QObject *parent = 0);
    inline int pairs() { return m_pairs; }
    inline int tables() { return m_movement->tables(); }
    inline int rounds() { return m_movement->rounds(); }
    inline int boardCount() { return m_boardCount; }
    inline int totalBoards() { return m_movement->boards()*m_boardCount; }
    inline QStandardItemModel* model(int round) { return m_models.value(round); }
    inline int modelSize() { return m_models.size(); }
    inline Movement::Type movementType() { return m_movement->type(); }
    inline ScoreSheet::ScoringType scoringType() { return m_sheets[0].type(); }
    inline QPair<int,int> roundBoard(int table) { return m_tableRounds.at(table); }
    int boardNo(int table, int round=-1, int board=-1);
    QPair<int,int> pairsOnTable(int round, int table);
    
signals:
    /* @params:
    table: table number who will receive the notification message
    ns_table: table number where NS will sit for next round
    ns_team: 0 if NS will sit as NS for next round, 1 otherwise
    ew_table: table number where EW will sit for next round
    ew_team: 0 if EW will sit as NS for next round, 1 otherwise
    next_ns: pair number that will sit as NS at this table for next round
    next_ew: pair number that will sit as EW at this table for next round
    */
    void tableChanged(int table, int ns_table, int ns_team, int ew_table, int ew_team, int next_ns, int next_ew);
    void tournamentEnded(int table);
    void resultReady();
    
public slots:
    void updateScore(int table, int score);
    void computeResult();

private:
    int m_pairs;
    int m_boardCount;
    Movement *m_movement;

    QList<QStandardItemModel*> m_models;
    QList< QPair<int,int> > m_tableRounds; // list< pair<round,board> >
    QList<ScoreSheet> m_sheets;

    QStandardItemModel* createModelForRound(int round);
    void notifyTableChange(int table);
    QPair<int,int> findTable(int round, QString pair);
    QList<QStandardRow> prepareRows(QList<int> pairs);
    
};

#endif // MOVEMENTMODEL_H
