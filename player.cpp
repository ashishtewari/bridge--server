#include "player.h"
#include "xmlengine.h"

#include <QUrl>
#include <QDebug>

Player::Player(QObject *parent) :
    QDeclarativeView(0)
{
    Q_UNUSED (parent);
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
    setAttribute(Qt::WA_LockLandscapeOrientation, true);
    setSource(QUrl("qrc:/qml/main.qml"));

    qmlItem = (QObject*)rootObject();

    connect(qmlItem, SIGNAL(gameClicked()), this, SLOT(start()));
    connect(qmlItem, SIGNAL(quit()), this, SLOT(closePlayer()));

    m_moveTimer.setInterval(1300);
    connect(&m_moveTimer, SIGNAL(timeout()), this, SLOT(makeMove()));
    m_gameActive = false;
}

void Player::setBoard(QDomElement boardElem)
{
    m_board = boardElem;
}

void Player::setMoves(QDomNodeList moveItems)
{
    for (int i=0; i<moveItems.size(); i++)
        m_moves << moveItems.at(i).toElement();
}

void Player::makeMove()
{
    QDomElement moveElem = m_moves.takeFirst();
    const StringMap stuff = XMLEngine::parseXml(moveElem);
    QString message = stuff["message"].toLower();

    if (message != "move") {
        if (!m_moves.isEmpty())
            makeMove();
        return;
    }

    QMetaObject::invokeMethod(qmlItem, "showMove",
                              Q_ARG(QVariant, stuff["player"].toInt()),
                              Q_ARG(QVariant, stuff["type"].toInt()),
                              Q_ARG(QVariant, stuff["data"]));

    if (m_moves.isEmpty())
        m_moveTimer.stop();
}

void Player::start()
{
    if (!m_moveTimer.isActive()) {
        if (!m_gameActive) {
            int dealer = m_board.attribute("dealer").toInt();
            int vulnerable = m_board.attribute("vulnerable").toInt();
            QMetaObject::invokeMethod(qmlItem, "setBoard",
                                      Q_ARG(QVariant, dealer),
                                      Q_ARG(QVariant, vulnerable));

            QDomNodeList cards = m_board.elementsByTagName("cards");
            for (int i=0; i<cards.size(); i++) {
                QDomElement e = cards.at(i).toElement();
                int player = e.attribute("player").toInt();
                QStringList cards = e.text().split(QChar(','));
                QMetaObject::invokeMethod(qmlItem, "showCards",
                                          Q_ARG(QVariant, player),
                                          Q_ARG(QVariant, cards));
            }
        }
        m_gameActive = true;
        m_moveTimer.start();
    }
    else {
        m_moveTimer.stop();
    }
}

void Player::closePlayer()
{
    m_moveTimer.stop();
    deleteLater();
}
