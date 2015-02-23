#ifndef PLAYER_H
#define PLAYER_H

#include <QDeclarativeView>
#include <QDomElement>
#include <QDomNodeList>
#include <QVariant>
#include <QTimer>

class Player : public QDeclarativeView
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = 0);
    
signals:
    void serverConnected();
    void authenticated(bool);
    void playerNumber(QVariant num);
    void boardReceived(QVariant data);
    void cardsReceived(QVariant player, QVariant cards);
    void moveReceived(QVariant move);
    
public slots:
    void setBoard(QDomElement boardElem);
    void setMoves(QDomNodeList moveItems);
    void start();
    void closePlayer();

private slots:
    void makeMove();

private:
    QObject *qmlItem;
    QDomElement m_board;
    QList<QDomElement> m_moves;
    QTimer m_moveTimer;
    bool m_gameActive;

};

#endif // PLAYER_H
