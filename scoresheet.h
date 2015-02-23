#ifndef SCORESHEET_H
#define SCORESHEET_H

#include <QList>
#include <QMap>
#include <QPair>

typedef QPair<int,int> IntPair;

class ScoreSheet
{
public:
    enum ScoringType {
        MatchPoint,
        IMP
    };

    ScoreSheet(ScoringType t);
    inline ScoringType type() { return m_type; }
    void insertScore(int score, int ns_pair, int ew_pair);
    void computePoints();
    void computeMP();
    void computeIMP();
    inline int points(int pair) { return m_points.value(pair); }
    inline int size() { return m_scorePairs.size(); }

private:
    QMap<int, IntPair> m_scorePairs; // map<ns_score, pair<ns_pair, ew_pair>>
    QMap<int,int> m_points; // map<pair, mp>; pair=NS/EW
    ScoringType m_type;
};

#endif // SCORESHEET_H
