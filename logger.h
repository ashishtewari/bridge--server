#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDomDocument>
#include <QTimer>

class MovementModel;
class Server;

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = 0);
    void initiate(Server *s, MovementModel *m);
    void exportLog(QString exportTo);
    bool loadEvent(QString fromFile, MovementModel* & model);
    void exportMatch(int round, int table, int board, QString toFile);
    bool importMatch(QString matchFile);
    bool loadMatch(int round, int table, int board);
    bool loadMatch(QDomElement match);
    
signals:
    void newEvent(int,int,int,int);
    void setBoardForMatch(QDomElement);
    void setMovesForMatch(QDomNodeList);
    
public slots:
    void initiateMatch(int table, int player);
    void logMove(int table, QString moveItem);
    void logScore(int table, int score);
    void writeOut();
    void stopWriter();
    void sendMatch(int table);
    
private:
    Server *m_server;
    MovementModel *m_model;
    QDomDocument *m_doc;
    QList<QDomElement> m_matches;
    QString m_filepath;
    QTimer *m_writeTimer;

    void insertChild(QDomElement& elem, QString tagName, QString text);
    void insertChild(QDomElement& elem, QString tagName, int number);
    void setupWriter();
    QDomElement getMatchElement(int round, int table, int board);
    QString getMatchString(int table);

};

#endif // LOGGER_H
