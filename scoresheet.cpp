#include "scoresheet.h"

ScoreSheet::ScoreSheet(ScoringType t)
{
    m_type = t;
}

void ScoreSheet::insertScore(int score, int ns_pair, int ew_pair)
{
    m_scorePairs.insertMulti(score, IntPair(ns_pair, ew_pair));
}

void ScoreSheet::computePoints()
{
    if (m_type == MatchPoint)
        computeMP();
    else if (m_type == IMP)
        computeIMP();
}

// remember:  2 points for each worse score
//          + 1 point for each equal score
void ScoreSheet::computeMP()
{
    QList<int> scores = m_scorePairs.keys();
    QList<IntPair> pairs = m_scorePairs.values();
    int size = scores.size();
    int totalMP = (size-1)*2;
    for (int i=0; i<size; i++) {
        int j=0, mp=0;
        while (scores[j++] < scores[i]) mp+=2;
        mp += scores.count(scores[i]) - 1;
        m_points.insert(pairs[i].first, mp);
        m_points.insert(pairs[i].second, totalMP-mp);
    }
}

int lookupImp(int diff)
{
    int sign=1;
    if (diff<0) sign=-1;
    diff = abs(diff);

    int table[] = {1,4,8,12,16,21,26,31,36,42,49,59,74,89,109,129,149,174,199,224,249,299,349,399};
    int i=0;
    for (i=0; i<24; i++) {
        if (diff <= table[i]*10) return sign*i;
    }
    return sign*i;
}

// average out the scores excluding max and min scores
// (if there are more than 2 matches) and compute IMP
// based on score minus the average
void ScoreSheet::computeIMP()
{
    QList<int> scores = m_scorePairs.keys();
    QList<IntPair> pairs = m_scorePairs.values();
    int size = scores.size();

    // compute average
    int avg_score=0, start=0, end=size;
    if (size>2) {
        start = 1;    // ignore minimum score
        end = size-1; // ignore maximum score
    }
    for (int i=start; i<end; i++) {
        avg_score += scores[i];
    }
    avg_score /= (end-start);

    // compute IMPs
    for (int i=0; i<size; i++) {
        int imp = lookupImp(scores[i]-avg_score);
        m_points.insert(pairs[i].first, imp);
        m_points.insert(pairs[i].second, -imp);
    }
}
