#include "server.h"
#include "networkutils.h"
#include "movementmodel.h"
#include "xmlengine.h"
#include <QRegExp>
#include <QTime>
#include <QTimer>
#include <QMessageBox>

#define UDP_PORT 5500
#define TCP_PORT 7070
#define CLIENT_PORT 6060

Server::Server(MovementModel *model, QObject *parent) :
    QObject(parent), m_model(model)
{
    // UDP listener
    m_udpListener = new QUdpSocket(this);
    while(!m_udpListener->bind(UDP_PORT, QUdpSocket::DontShareAddress));
    connect(m_udpListener, SIGNAL(readyRead()),
            this, SLOT(readDatagrams()));

    // TCP listener
    m_tcpListener = new QTcpServer(this);
    if (!m_tcpListener->listen(QHostAddress::Any, TCP_PORT)) {
        QMessageBox::critical(NULL, "Network error", "Another instance of server seems to be running already.");
    }
    else {
        connect(m_tcpListener, SIGNAL(newConnection()),
                this, SLOT(connectToClient()));
    }
    qDebug() << "Server up";

    // set up tables
    for (int i=0; i<m_model->tables(); i++) {
        QList<QTcpSocket*> t;
        t << 0 << 0 << 0 << 0;
        m_tableSockets << t;
        m_waitingSpecs << QList<QTcpSocket*>();
        m_tableReadyCount << 0;
    }

    // set up boards
    qsrand((uint) QTime::currentTime().msec());
    for (int i=0; i<m_model->totalBoards(); i++) {
        m_boards << Board(i);
    }
}

Server::~Server()
{
    m_tcpListener->close();
    for(int i=0; i<m_model->tables(); i++)
        closeConnections(i);
}

void Server::readDatagrams()
{
    NetworkUtils::readBroadcast(m_udpListener, 0, true);
    NetworkUtils::broadcast("iamhere", CLIENT_PORT);
}

void Server::connectToClient()
{
    QTcpSocket *s = m_tcpListener->nextPendingConnection();

    connect(s, SIGNAL(readyRead()),
            this, SLOT(readClient()));

    connect(s, SIGNAL(disconnected()),
            this, SLOT(emitClientDisconnected()));

    connect(s, SIGNAL(disconnected()),
            s, SLOT(deleteLater()));
}

void Server::readClient()
{
    QTcpSocket *s = qobject_cast<QTcpSocket*>(sender());
    QString msg = NetworkUtils::tcpRecv(s);
    parseClient(msg, s);
}

void Server::parseClient(QString msg, QTcpSocket *sender)
{
    const StringMap stuff = XMLEngine::parseXml(msg);
    QString message = stuff["message"];
    int table = stuff["table"].toInt()-1;
    int player = stuff["player"].toInt();

    if (message!="identify" && !m_authClients.contains(sender))
        return;

    if (message == "identify") {
        // Insert the player in the table
        int ok = (table>=0 && table<m_tableSockets.size() && m_model->roundBoard(table).first<m_model->rounds());
        if (ok) {
            if (player<4) {
                if (m_tableSockets[table][player] == 0) {
                    m_tableSockets[table][player] = sender;
                    m_authClients << sender;
                }
                else {
                    ok = 0;
                }
            }
            else { // spectator
                if (m_tableReadyCount[table]<4)
                    m_waitingSpecs[table] << sender;
                else
                    m_tableSockets[table] << sender;
            }
        }

        if (player<4 || !ok) {
            StringMap m;
            m.insert("message", "ack");
            m.insert("ok", QString::number(ok));
            NetworkUtils::tcpSend(XMLEngine::buildXml(m), sender);
        }

        if (ok) {
            if (player<4)
                emit clientConnected(table, player, true);
            else if (m_tableReadyCount[table]>=4)
                emit specArrived(table);
        }
    }
    else if (message == "ready") {
        if (++m_tableReadyCount[table] >= 4) {
            emit tableReady(table, player);
        }
    }
    else if (message == "move" || message == "dummy_cards") {
        multicastMove(table, player, msg);
        emit moveReceived(table, msg);
    }
    else if (message == "score") {
        int score = stuff["score"].toInt();
        emit scoreReceived(table, score);
        m_tableReadyCount[table] = 0;
    }
}

void Server::prepareTable(int table)
{
    m_tableQ.enqueue(table);
    QTimer::singleShot(1000, this, SLOT(startGame()));
}

void Server::startGame()
{
    // prepare dealer, vulnerable team
    int table = m_tableQ.dequeue();
    Board flap = m_boards[m_model->boardNo(table)];
    int round = m_model->roundBoard(table).first;
    QPair<int,int> pairs = m_model->pairsOnTable(round, table);
    StringMap m;
    m.insert("message", "board");
    m.insert("number", QString::number(m_model->boardNo(table)+1));
    m.insert("dealer", QString::number(flap.dealer()));
    m.insert("vulnerable", QString::number(flap.vulnerableTeam()));
    m.insert("round", QString::number(round+1));
    m.insert("ns_pair", QString::number(pairs.first+1));
    m.insert("ew_pair", QString::number(pairs.second+1));

    // prepare cards and send board+cards
    QList<QTcpSocket*> sockets = m_tableSockets[table];
    for (int j=0; j<4; j++) {
        QString cards;
        foreach(const int &card, flap.cardsOf(j)) {
            cards += QString::number(card)+",";
        }
        cards = cards.left(cards.size()-1); // remove the trailing comma
        m["cards"] = cards;
        NetworkUtils::tcpSend(XMLEngine::buildXml(m), sockets.at(j));
    }

    // send match to specs
    int size = m_waitingSpecs[table].size();
    for (int j=0; j<size; j++) {
        m_tableSockets[table] << m_waitingSpecs[table].takeFirst();
        emit specArrived(table);
    }
}

void Server::multicastMove(int table, int player, QString msg)
{
    QList<QTcpSocket*> sockets = m_tableSockets[table];
    NetworkUtils::multicast(msg, sockets, player);
}

void Server::notify(QString notification, QTcpSocket *receiver)
{
    if (!receiver)
        return;
    StringMap m;
    m.insert("message", "notification");
    m.insert("note", notification);
    NetworkUtils::tcpSend(XMLEngine::buildXml(m), receiver);
}

QString nextTableNotification(int table, int team, int nextHere)
{
    QString note;
    if (table == -1) {
        note = "Sit out for next round";
    }
    else {
        note = QString("Go next to Table-%1 %2").arg(table+1).arg(team ? " EW" : " NS");
    }
    note.append( QString("\nPair %1 will play here for next round").arg(nextHere) );
    return note;
}

/* @params:
table: table number who will receive the notification message
ns_table: table number where NS will sit for next round
ns_team: 0 if NS will sit as NS for next round, 1 otherwise
ew_table: table number where EW will sit for next round
ew_team: 0 if EW will sit as NS for next round, 1 otherwise
next_ns: pair number that will sit as NS at this table for next round
next_ew: pair number that will sit as EW at this table for next round
*/
void Server::notifyTableChange(int table, int ns_table, int ns_team, int ew_table, int ew_team, int next_ns, int next_ew)
{
    QList<QTcpSocket*> sockets = m_tableSockets[table];
    if (ns_table!=table) {
        QString ns_notification = nextTableNotification(ns_table, ns_team, next_ns);
        notify(ns_notification, sockets[0]);
        notify(ns_notification, sockets[2]);
    }
    if (ew_table!=table) {
        QString ew_notification = nextTableNotification(ew_table, ew_team, next_ew);
        notify(ew_notification, sockets[1]);
        notify(ew_notification, sockets[3]);
    }
}

void Server::notifyTournamentEnd(int table)
{
    foreach(QTcpSocket* s, m_tableSockets[table].mid(0, 4)) {
        notify("Tournament over.\nResults will be declared soon.\nThank you for participating in the tournament.", s);
    }
    closeConnections(table);
}

void Server::closeConnections(int table)
{
    for(int i=0; i<m_tableSockets[table].size(); i++) {
        if (m_tableSockets[table][i]==0) continue;
        m_tableSockets[table][i]->close();
        m_tableSockets[table][i] = 0;
    }
    for(int i=0; i<m_waitingSpecs[table].size(); i++) {
        if (m_waitingSpecs[table][i]==0) continue;
        m_waitingSpecs[table][i]->close();
        m_waitingSpecs[table][i] = 0;
    }
}

void Server::emitClientDisconnected()
{
    QTcpSocket *s = qobject_cast<QTcpSocket*>(sender());
    int table, player=-1;
    for (table=0; table<m_tableSockets.size() && (player=m_tableSockets[table].indexOf(s))==-1; table++);
    if (player==-1) return;
    qDebug() << "disconnected:" << "table" << table << "player" << player;
    m_tableSockets[table][player] = 0;
    m_authClients.removeAll(s);
    if (player<4)
        emit clientConnected(table, player, false);
}

void Server::sendMatchToSpec(QString data, int table)
{
    StringMap m;
    m.insert("message", "ack");
    m.insert("ok", "1");
    m.insert("data", data);
    NetworkUtils::tcpSend(XMLEngine::buildXml(m), m_tableSockets[table].last());
}

void Server::sendMatchToPlayer(QString data, int table, int player)
{
    StringMap m;
    m.insert("message", "resume_data");
    m.insert("data", data);
    NetworkUtils::tcpSend(XMLEngine::buildXml(m), m_tableSockets[table][player]);
}
