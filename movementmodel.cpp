#include "movementmodel.h"
#include "mitchell.h"
#include "howell.h"

#define TABLE_COLUMN 0
#define BOARD_COLUMN 1
#define NS_COLUMN 2
#define EW_COLUMN 3
#define SCORE_COLUMN 4
#define COLUMNS 5
#define RANK_COLUMN 0
#define PAIR_COLUMN 1
#define TOTAL_COLUMN 2
#define RESULT_COLUMNS 3

MovementModel::MovementModel(int pairs, int boardCount, int movement, int scoring, QObject *parent) :
    QObject(parent),
    m_pairs(pairs), m_boardCount(boardCount)
{
    if (movement==Movement::Mitchell) m_movement = new Mitchell(m_pairs);
    else if (movement==Movement::Howell) m_movement = new Howell(m_pairs);

    m_models.clear();
    for (int i=0; i<m_movement->rounds(); i++) {
        m_models << createModelForRound(i);
    }
    m_tableRounds.clear();
    for (int i=0; i<m_movement->tables(); i++) {
        m_tableRounds << QPair<int,int>(0,0);
    }
    m_sheets.clear();
    for (int i=0; i<m_movement->boards()*m_boardCount; i++) {
        m_sheets << ScoreSheet((ScoreSheet::ScoringType)scoring);
    }
}

QStringList headerLabels()
{
    QStringList labels;
    for (int i=0; i<COLUMNS; i++)
        labels << "";
    labels[TABLE_COLUMN] = "Table";
    labels[BOARD_COLUMN] = "Board";
    labels[NS_COLUMN] = "NS";
    labels[EW_COLUMN] = "EW";
    labels[SCORE_COLUMN] = "NS Score";
    return labels;
}

QStandardItemModel* MovementModel::createModelForRound(int round)
{
    QStringList labels = headerLabels();
    QStandardItemModel *model = new QStandardItemModel(m_movement->tables()*m_boardCount, labels.size(), this);
    model->setHorizontalHeaderLabels(labels);
    QStandardItem *item;
    for (int i=0; i<model->rowCount(); i++) {
        int table = i/m_boardCount;
        item = new QStandardItem(QString("Table %1").arg(table+1));
        item->setData(table);
        model->setItem(i, TABLE_COLUMN, item);

        int flap = m_movement->board(round, table)*m_boardCount + (i%m_boardCount);
        item = new QStandardItem(QString("%1").arg(flap+1));
        item->setData(flap);
        model->setItem(i, BOARD_COLUMN, item);

        int p = m_movement->NSPair(round, table);
        item = new QStandardItem(QString("%1").arg(p+1));
        item->setData(p);
        model->setItem(i, NS_COLUMN, item);

        p = m_movement->EWPair(round, table);
        item = new QStandardItem(QString("%1").arg(p+1));
        item->setData(p);
        model->setItem(i, EW_COLUMN, item);
    }
    return model;
}

QPair<int,int> MovementModel::findTable(int round, QString pair)
{
    QPair<int,int> next;
    QStandardItemModel *model = m_models[round];
    next.second = 0;
    QList<QStandardItem*> items = model->findItems(pair, Qt::MatchExactly, NS_COLUMN);
    if (items.size()==0) {
        items = model->findItems(pair, Qt::MatchExactly, EW_COLUMN);
        next.second = 1;
    }
    if (items.size()==0) next.first = -1;
    else {
        int row = items.at(0)->row();
        next.first = model->item(row, TABLE_COLUMN)->data().toInt();
    }
    return next;
}

void MovementModel::notifyTableChange(int table)
{
    // get pairs on this table
    QPair<int,int> r = m_tableRounds[table];
    QStandardItemModel *model = m_models[r.first];
    int row = table*m_boardCount + r.second;
    QString ns_pair = model->item(row, NS_COLUMN)->text();
    QString ew_pair = model->item(row, EW_COLUMN)->text();

    // get tables for next round for this pairs
    QPair<int,int> ns_next = findTable(r.first+1, ns_pair);
    QPair<int,int> ew_next = findTable(r.first+1, ew_pair);

    // get pairs for next round for this table
    QPair<int,int> next_pairs = pairsOnTable(r.first+1, table);

    emit tableChanged(table, ns_next.first, ns_next.second, ew_next.first, ew_next.second, next_pairs.first+1, next_pairs.second+1);
}

void MovementModel::updateScore(int table, int score)
{
    QPair<int,int>& r = m_tableRounds[table];
    QStandardItemModel *model = m_models.at(r.first);
    int row = table*m_boardCount + r.second;
    QStandardItem *item = new QStandardItem(QString::number(score));
    model->setItem(row, SCORE_COLUMN, item);

    int board = model->item(row, BOARD_COLUMN)->text().toInt()-1;
    int ns_pair = model->item(row, NS_COLUMN)->text().toInt()-1;
    int ew_pair = model->item(row, EW_COLUMN)->text().toInt()-1;
    m_sheets[board].insertScore(score, ns_pair, ew_pair);

    if (r.second==m_boardCount-1) {
        if (r.first<m_movement->rounds()-1) {
            notifyTableChange(table);
            r.second = 0;
            r.first++;
        }
        else {
            r.second = 0;
            r.first = m_movement->rounds();
            emit tournamentEnded(table);
        }
    }
    else {
        r.second++;
    }
}

int MovementModel::boardNo(int table, int round, int board)
{
    QPair<int,int> r = m_tableRounds[table];
    if (round==-1) {
        round = r.first;
        board = r.second;
    }
    return m_models[round]->item(table*m_boardCount+board, BOARD_COLUMN)->text().toInt()-1;
}

QString rankString(int rank)
{
    int u = rank%10;

    if (u==1 && rank!=11) return QString("%1st").arg(rank);
    else if (u==2 && rank!=12) return QString("%1nd").arg(rank);
    else if (u==3 && rank!=13) return QString("%1rd").arg(rank);
    else return QString("%1th").arg(rank);
}

QList<QStandardRow> MovementModel::prepareRows(QList<int> pairs)
{
    QMap<int, QStandardRow> sortRows; // inserts rows sorted by total MPs
    foreach(int p, pairs) {
        QStandardRow r;
        r << new QStandardItem() << new QStandardItem() << new QStandardItem();
        r[PAIR_COLUMN]->setText(QString::number(p+1));
        int total = 0;
        for (int i=0; i<m_sheets.size(); i++) {
            int points = m_sheets[i].points(p);
            total += points;
            r << new QStandardItem(QString::number(points));
        }
        r[TOTAL_COLUMN]->setText(QString::number(total));
        sortRows.insertMulti(total, r);
    }

    QList<QStandardRow> sortedRows = sortRows.values();
    QList<int> points = sortRows.keys();
    QList<QStandardRow> rankedRows;
    int rank=1;
    for (int i=sortedRows.size()-1; i>=0; i--) {
        sortedRows[i][RANK_COLUMN]->setText(rankString(rank));
        rankedRows << sortedRows[i];
        if ( !points.mid(0, i).contains(points[i]) )
            rank++;
    }
    return rankedRows;
}

QStringList resultHeaderLabels()
{
    QStringList labels;
    for (int i=0; i<RESULT_COLUMNS; i++)
        labels << "";
    labels[RANK_COLUMN] = "Rank";
    labels[PAIR_COLUMN] = "Pair";
    labels[TOTAL_COLUMN] = "Total";
    return labels;
}

void MovementModel::computeResult()
{
    if (m_models.size() > rounds())
        return;

    for (int i=0; i<m_sheets.size(); i++) {
        m_sheets[i].computePoints();
    }

    QStringList labels = resultHeaderLabels();
    for (int i=0; i<m_movement->rounds()*m_boardCount; i++) labels << QString("B%1").arg(i+1);
    QStandardItemModel *m = new QStandardItemModel(this);
    m->setHorizontalHeaderLabels(labels);

    QList<QStandardRow> rows;
    if (m_movement->type() == Movement::Mitchell) {
        rows << QStandardRow();
        rows.last() << new QStandardItem("North-South:");
        rows << prepareRows(m_movement->NSPairs());
        rows << QStandardRow();
        rows.last() << new QStandardItem("East-West:");
        rows << prepareRows(m_movement->EWPairs());
    }
    else if (m_movement->type() == Movement::Howell) {
        rows << prepareRows(m_movement->NSPairs());
    }

    foreach(QStandardRow r, rows) {
        m->appendRow(r);
    }

    m_models << m;
    emit resultReady();
}

QPair<int,int> MovementModel::pairsOnTable(int round, int table)
{
    return QPair<int,int>(m_movement->NSPair(round, table), m_movement->EWPair(round, table));
}
