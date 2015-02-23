#include "logger.h"
#include "server.h"
#include "movementmodel.h"
#include "board.h"

#include <QDomElement>
#include <QDateTime>
#include <QDate>
#include <QDir>
#include <QMessageBox>

Logger::Logger(QObject *parent) :
    QObject(parent)
{
}

void Logger::initiate(Server *s, MovementModel *m)
{
    m_server = s;
    m_model = m;
    m_doc = new QDomDocument();
    QDomElement root = m_doc->createElement("event");
    m_doc->appendChild(root);
    insertChild(root, "timestamp", QDateTime::currentDateTime().toString());
    insertChild(root, "pairs", m_model->pairs());
    insertChild(root, "movement", m_model->movementType());
    insertChild(root, "boardCount", m_model->boardCount());
    insertChild(root, "scoring", m_model->scoringType());

    foreach(Board b, m_server->boards()) {
        QDomElement board = m_doc->createElement("board");
        board.setAttribute("number", b.boardNo());
        board.setAttribute("dealer", b.dealer());
        board.setAttribute("vulnerable", b.vulnerableTeam());
        for (int i=0; i<4; i++) {
            QDomElement cards = m_doc->createElement("cards");
            cards.setAttribute("player", i);
            QStringList list;
            foreach(int card, b.cardsOf(i)) {
                list << QString::number(card);
            }
            QDomText t = m_doc->createTextNode(list.join(","));
            cards.appendChild(t);
            board.appendChild(cards);
        }
        root.appendChild(board);
    }

    setupWriter();

    QDomElement dummy;
    for (int i=0; i<m_model->tables(); i++) {
        m_matches << dummy;
    }

    connect(m_server, SIGNAL(tableReady(int,int)), this, SLOT(initiateMatch(int,int)));
    connect(m_server, SIGNAL(moveReceived(int,QString)), this, SLOT(logMove(int,QString)));
    connect(m_server, SIGNAL(scoreReceived(int,int)), this, SLOT(logScore(int,int)));
    connect(m_server, SIGNAL(specArrived(int)), this, SLOT(sendMatch(int)));
    connect(m_model, SIGNAL(resultReady()), this, SLOT(stopWriter()));
}

void Logger::insertChild(QDomElement& elem, QString tagName, QString text)
{
    QDomElement tag = m_doc->createElement(tagName);
    tag.appendChild(m_doc->createTextNode(text));
    elem.appendChild(tag);
}

void Logger::insertChild(QDomElement& elem, QString tagName, int number)
{
    insertChild(elem, tagName, QString::number(number));
}

void Logger::initiateMatch(int table, int player)
{
    if (m_matches[table].isNull() || m_matches[table].elementsByTagName("score").size()>0) {
        QPair<int,int> rb = m_model->roundBoard(table);
        QPair<int,int> pairs = m_model->pairsOnTable(rb.first, table);
        m_matches[table] = m_doc->createElement("match");
        m_matches[table].setAttribute("round", rb.first);
        m_matches[table].setAttribute("board", rb.second);
        m_matches[table].setAttribute("table", table);
        m_matches[table].setAttribute("boardNo", m_model->boardNo(table, rb.first, rb.second));
        m_matches[table].setAttribute("ns_pair", pairs.first);
        m_matches[table].setAttribute("ew_pair", pairs.second);
        m_doc->documentElement().appendChild(m_matches[table]);
        m_server->prepareTable(table);
    }
    else {
        m_server->sendMatchToPlayer(getMatchString(table), table, player);
    }
}

void Logger::logMove(int table, QString moveItem)
{
    QDomDocument doc;
    doc.setContent(moveItem);
    m_matches[table].appendChild(doc.documentElement());
}

void Logger::logScore(int table, int score)
{
    insertChild(m_matches[table], "score", score);
}

void Logger::setupWriter()
{
    QString path = QDir::homePath() + "/Bridge Club/";
    QDir().mkpath(path);
    QString filename = QDate::currentDate().toString();
    m_filepath = QString("%1%2.xml").arg(path, filename);
    for (int i=1; QFile(m_filepath).exists(); i++) {
        m_filepath = QString("%1%2 (%3).xml").arg(path, filename).arg(i);
    }
    writeOut();

    m_writeTimer = new QTimer(this);
    connect(m_writeTimer, SIGNAL(timeout()), this, SLOT(writeOut()));
    m_writeTimer->start(5000);
}

void Logger::stopWriter()
{
    m_writeTimer->stop();
    writeOut();
}

void Logger::writeOut()
{
    exportLog(m_filepath);
}

void Logger::exportLog(QString exportTo)
{
    QFile file(exportTo);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << m_doc->toString();
    file.close();
}

int readSimpleElement(QDomElement& node, QString tagName)
{
    QDomNodeList elems = node.elementsByTagName(tagName);
    if (elems.isEmpty())
        return -1;
    bool ok;
    int value = elems.at(0).toElement().text().toInt(&ok);
    if (ok)
        return value;
    else
        return -1;
}

bool Logger::loadEvent(QString fromFile, MovementModel* & model)
{
    // read from file
    QFile file(fromFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    m_doc = new QDomDocument();
    m_doc->setContent(&file);
    file.close();
    QDomElement root = m_doc->documentElement();

    // get the movement model up and running
    int pairs = readSimpleElement(root, "pairs");
    int movement = readSimpleElement(root, "movement");
    int boardCount = readSimpleElement(root, "boardCount");
    int scoring = readSimpleElement(root, "scoring");
    if (pairs==-1 || movement==-1 || boardCount==-1)
        return false;
    emit newEvent(pairs, boardCount, movement, scoring); // model will be initialized in the slot of this signal
    m_model = model;

    // fetch and store the scores from log
    QList<int> scores;
    for (int i=0; i<m_model->rounds()*m_model->tables()*m_model->boardCount(); i++) {
        scores << 0;
        m_matches << QDomElement();
    }

    QDomNodeList matches = root.elementsByTagName("match");
    if (matches.isEmpty())
        return false;

    // put all the scores in an array sorted by the round, table and boardNo
    int rows = m_model->tables()*m_model->boardCount();
    for (int i=0; i<matches.count(); i++) {
        QDomElement elem = matches.at(i).toElement();
        if (elem.isNull())
            return false;
        int round = elem.attribute("round").toInt();
        int board = elem.attribute("board").toInt();
        int table = elem.attribute("table").toInt();
        int score = readSimpleElement(elem, "score");
        if (score==-1)
            return false;
        int j = round*rows + table*m_model->boardCount() + board;
        scores[j] = score;
        m_matches[j] = elem;
    }

    // put the scores into the model
    for (int i=0; i<scores.count(); i++) {
        int table = (i%rows) / m_model->boardCount();
        m_model->updateScore(table, scores[i]);
    }

    return true;
}

QDomElement Logger::getMatchElement(int round, int table, int board)
{
    // find the said match
    QDomElement root = m_doc->documentElement();
    QDomNodeList matches = root.elementsByTagName("match");
    QDomElement match;
    for (int i=0; i<matches.count(); i++) {
        match = matches.at(i).cloneNode().toElement();
        if (round != match.attribute("round").toInt()) continue;
        if (table != match.attribute("table").toInt()) continue;
        if (board != match.attribute("board").toInt()) continue;

        // match found!
        int boardNo = m_model->boardNo(table, round, board);

        // find the board used in that match and append to the match
        QDomNodeList boards = root.elementsByTagName("board");
        for (int j=0; j<boards.count(); j++) {
            QDomElement boardElem = boards.at(j).cloneNode().toElement();
            if (boardNo == boardElem.attribute("number").toInt()) {
                match.appendChild(boardElem);
                break;
            }
        }
        break;
    }

    return match;
}

void Logger::exportMatch(int round, int table, int board, QString toFile)
{
    QFile file(toFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(0, "Error opening file", "There was an error opening the specified file for writing. Please choose alternate save location.");
        return;
    }

    QDomDocument matchDoc;
    matchDoc.createElement("dummy"); // this is for making matchDoc non-null node
    matchDoc.appendChild(getMatchElement(round, table, board));
    QTextStream out(&file);
    out << matchDoc.toString();

    file.close();
}

bool Logger::importMatch(QString matchFile)
{
    QFile file(matchFile);
    QDomDocument doc;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    doc.setContent(&file);
    file.close();
    return loadMatch(doc.documentElement());
}

bool Logger::loadMatch(int round, int table, int board)
{
    return loadMatch(getMatchElement(round, table, board));
}

bool Logger::loadMatch(QDomElement match)
{
    if (match.tagName() != "match") return false;

    QDomNodeList nodes = match.elementsByTagName("board");
    if (nodes.isEmpty()) return false;
    emit setBoardForMatch(nodes.at(0).toElement());

    nodes = match.elementsByTagName("item");
    if (nodes.isEmpty()) return false;
    emit setMovesForMatch(nodes);

    return true;
}

QString Logger::getMatchString(int table)
{
    QPair<int,int> rb = m_model->roundBoard(table);
    QDomElement match = getMatchElement(rb.first, table, rb.second);
    if (match.isNull()) {
        return QString();
    }
    QDomDocument doc;
    doc.createElement("dummy"); // this is for making the doc non-null node
    doc.appendChild(match);
    return doc.toString(-1);
}

void Logger::sendMatch(int table)
{
    m_server->sendMatchToSpec(getMatchString(table), table);
}
