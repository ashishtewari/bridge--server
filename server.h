#ifndef SERVER_H
#define SERVER_H

#include <QUdpSocket>
#include <QTcpServer>
#include <QList>
#include <QQueue>
#include "board.h"

class MovementModel;
class XMLEngine;

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(MovementModel *model, QObject *parent = 0);
    inline QList<Board> boards() { return m_boards; }
    void sendMatchToSpec(QString data, int table);
    void sendMatchToPlayer(QString data, int table, int player);
    ~Server();

signals:
    void clientConnected(int,int,bool);
    void tableReady(int table, int player);
    void moveReceived(int table, QString move);
    void scoreReceived(int table, int score);
    void specArrived(int table);

public slots:
    void readDatagrams();
    void connectToClient();
    void readClient();
    void prepareTable(int table);
    void startGame();
    void notifyTableChange(int,int,int,int,int,int,int);
    void notifyTournamentEnd(int table);
    void emitClientDisconnected();

private:
    void parseClient(QString msg, QTcpSocket *sender);
    void multicastMove(int table, int player, QString message);
    void notify(QString notification, QTcpSocket *receiver);
    void closeConnections(int table);

    QUdpSocket *m_udpListener; // listener for incoming broadcast
    QTcpServer *m_tcpListener; // listener for incoming connections
    QList<QTcpSocket*> m_authClients; // list of authenticated clients
    QList< QList<QTcpSocket*> > m_tableSockets; // list <table, list<player, socket>>
    QList< QList<QTcpSocket*> > m_waitingSpecs; // list <table, list<spec, socket>>
    QList<Board> m_boards; // list of boards
    QList<int> m_tableReadyCount; // list <table, readyCount>
    QQueue<int> m_tableQ; // queue of tables ready to begin the game
    MovementModel *m_model;

};

#endif // SERVER_H
