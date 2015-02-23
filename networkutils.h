#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include <QObject>
#include <QByteArray>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>

class NetworkUtils : public QObject
{
    Q_OBJECT
public:
    explicit NetworkUtils(QObject *parent = 0);
    static QByteArray wrapString(QString msg);
    static void broadcast(QString msg, int port);
    static QString readBroadcast(QUdpSocket *sock, QHostAddress *senderPtr = 0, bool readFromLocal = false);
    static void tcpSend(QString msg, QTcpSocket *sock);
    static QString tcpRecv(QTcpSocket *sock);
    static void multicast(QString msg, QList<QTcpSocket*> receivers, int except = -1);
    
signals:
    
public slots:
    
};

#endif // NETWORKUTILS_H
