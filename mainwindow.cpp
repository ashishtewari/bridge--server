#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "neweventdialog.h"
#include "lightswidget.h"
#include "server.h"
#include "movementmodel.h"
#include "logger.h"
#include "tournamentslist.h"
#include "player.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDate>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_server = 0;
    m_model = 0;
    m_logger = 0;
    m_player = 0;
    connect(ui->actionStop_Event, SIGNAL(triggered()), this, SLOT(stopEvent()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_Event_triggered()
{
    if (m_server) {
        QString message = "There is already a tournament being conducted.";
        message.append("\n\nAre you sure you want to cancel it and start a new one?");
        QMessageBox::StandardButton which = QMessageBox::question(this,
                                                                  "Event going on",
                                                                  message,
                                                                  QMessageBox::Yes | QMessageBox::No,
                                                                  QMessageBox::No);
        if (which == QMessageBox::Yes)
            stopEvent();
        else
            return;
    }
    NewEventDialog dlg(this);
    if (dlg.exec()) {
        initialiseView(dlg.pairs(), dlg.boardCount(), dlg.movement(), dlg.scoring());
        ui->actionStart_Server->trigger();
        on_roundsList_currentRowChanged(0);
    }
}

void MainWindow::initialiseView(int pairs, int boardCount, int movement, int scoring)
{
    stopMovementModel();
    m_model = new MovementModel(pairs, boardCount, movement, scoring, this);
    for (int i=0; i<m_model->rounds(); i++) {
        ui->roundsList->addItem(QString("Round %1").arg(i+1));
    }
    for (int i=0; i<m_model->tables(); i++) {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, QString("Table %1").arg(i+1));
        ui->statusList->addTopLevelItem(item);
        ui->statusList->setItemWidget(item, 1, new LightsWidget);
    }
    connect(ui->actionCompute_Result, SIGNAL(triggered()), m_model, SLOT(computeResult()));
    connect(m_model, SIGNAL(resultReady()), this, SLOT(showResult()));
}

void MainWindow::on_roundsList_currentRowChanged(int currentRow)
{
    if (currentRow>=0 && currentRow<m_model->modelSize()) {
        ui->tablesList->setModel(m_model->model(currentRow));
        if (currentRow == m_model->rounds()) {
            ui->tablesList->setColumnWidth(1, 35);
            ui->tablesList->setColumnWidth(2, 70);
            for (int i=3; i<ui->tablesList->header()->count(); i++)
                ui->tablesList->setColumnWidth(i, 30);
        }
        else {
            int w = ui->tablesList->width()/ui->tablesList->header()->count() - 1;
            for (int i=0; i<ui->tablesList->header()->count(); i++)
                ui->tablesList->setColumnWidth(i, w);
        }
    }
}

void MainWindow::on_actionStart_Server_triggered()
{
    if (m_server) return;
    if (!m_model) {
        QMessageBox::critical(this, "Not ready", "The movement table is not ready");
        return;
    }
    stopServer();
    m_server = new Server(m_model, this);
    connect(m_server, SIGNAL(clientConnected(int,int,bool)), this, SLOT(updateLights(int,int,bool)));
    connect(m_server, SIGNAL(scoreReceived(int,int)), m_model, SLOT(updateScore(int,int)));
    connect(m_model, SIGNAL(tableChanged(int,int,int,int,int,int,int)), m_server, SLOT(notifyTableChange(int,int,int,int,int,int,int)));
    connect(m_model, SIGNAL(tournamentEnded(int)), m_server, SLOT(notifyTournamentEnd(int)));
    stopLogger();
    m_logger = new Logger(this);
    m_logger->initiate(m_server, m_model);
}

void MainWindow::stopServer()
{
    if (m_server) {
        disconnect(m_server, 0, 0, 0);
        disconnect(this, 0, m_server, 0);
        if (m_model)
            disconnect(m_model, 0, m_server, 0);
        delete m_server;
        m_server = 0;
    }
}

void MainWindow::stopLogger()
{
    if (m_logger) {
        disconnect(m_logger, 0, 0, 0);
        if (m_server)
            disconnect(m_server, 0, m_logger, 0);
        if (m_model)
            disconnect(m_model, 0, m_logger, 0);
        delete m_logger;
        m_logger = 0;
    }
}

void MainWindow::stopMovementModel()
{
    if (m_model) {
        disconnect(m_model, 0, 0, 0);
        disconnect(this, 0, m_model, 0);
        if (m_server) disconnect(m_server, 0, m_model, 0);
        disconnect(ui->actionCompute_Result, 0, m_model, 0);
        delete m_model;
        m_model = 0;
    }
    ui->tablesList->setModel(0);
    ui->roundsList->clear();
    ui->statusList->clear();
}

void MainWindow::updateLights(int table, int player, bool on)
{
    QTreeWidgetItem *item = ui->statusList->topLevelItem(table);
    if (!item)
        return;
    LightsWidget *w = qobject_cast<LightsWidget*>(ui->statusList->itemWidget(item, 1));
    w->switchLight(player, on);
}

void MainWindow::showResult()
{
    ui->roundsList->addItem("Result");
    ui->roundsList->setCurrentRow(ui->roundsList->count()-1);
}

void MainWindow::on_actionQuit_triggered()
{
    if (m_server) {
        QString message = "There is already a tournament being conducted.";
        message.append("\n\nAre you sure you want to cancel it and quit?");
        if (QMessageBox::Yes == QMessageBox::question(this,
                                                      "Event going on",
                                                       message,
                                                       QMessageBox::Yes | QMessageBox::No,
                                                       QMessageBox::No)
                ) {
            close();
        }
        return;
    }
    close();
}

void MainWindow::on_actionExport_Tournament_triggered()
{
    if (!m_logger) {
        QMessageBox::warning(this, "No tournament", "There is no tournament to export!");
        return;
    }
    QString defaultPath = QString("%1/%2.xml").arg(QDir::homePath(), QDate::currentDate().toString());
    QString saveFilepath = QFileDialog::getSaveFileName(this,
                                                        "Browse to the location for saving tournament file",
                                                        defaultPath,
                                                        "XML document (*.xml)");
    if (saveFilepath.isEmpty()) return;
    m_logger->exportLog(saveFilepath);
}

void MainWindow::on_actionImport_Tournament_triggered()
{
    QString logFile = QFileDialog::getOpenFileName(this,
                                                   "Browse to the tournament file",
                                                   QDir::homePath(),
                                                   "XML document (*.xml)");
    if (logFile.isEmpty())
        return;
    loadLogFile(logFile);
}

void MainWindow::on_actionExport_selected_match_triggered()
{
    QModelIndex currentIndex = ui->tablesList->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::critical(this, "No match selected", "There is no match selected. Please select a match from the list.");
        return;
    }

    int round = ui->roundsList->currentRow();
    if (round >= m_model->rounds()) {
        QMessageBox::critical(this, "Invalid match", "Invalid round selected for export.");
        return;
    }
    int row = currentIndex.row();
    int table = row/m_model->boardCount();
    int board = row%m_model->boardCount();
    QPair<int,int> roundBoard = m_model->roundBoard(table);
    int ongoing_round = roundBoard.first;
    if (ongoing_round<round || (ongoing_round==round && roundBoard.second<=board)) {
        QMessageBox::critical(this, "Error", "This match hasn't happened yet. You can only export it after the scores for this match have arrived.");
        return;
    }

    QString saveFilepath = QFileDialog::getSaveFileName(this,
                                                        "Browse to a location for saving the match",
                                                        QDir::homePath(),
                                                        "XML document (*.xml)");
    if (saveFilepath.isEmpty()) return;

    m_logger->exportMatch(round, table, board, saveFilepath);
}

void MainWindow::on_actionLoad_local_tournament_triggered()
{
    TournamentsList dlg(this);
    if (dlg.exec()) {
        QString filename = dlg.selectedItem();
        loadLogFile(filename);
    }
}

void MainWindow::loadLogFile(QString filepath)
{
    stopLogger();
    m_logger = new Logger(this);
    connect(m_logger, SIGNAL(newEvent(int,int,int,int)), this, SLOT(initialiseView(int,int,int,int)));
    if (m_logger->loadEvent(filepath, m_model))
        ui->actionCompute_Result->trigger();
    else {
        ui->roundsList->clear();
        ui->statusList->clear();
        ui->tablesList->setModel(0);
        stopEvent();
        QMessageBox::critical(this, "Error", "There was an error loading the file. It is either not readable or a wrong file is selected. Please try again.");
    }
}

void MainWindow::on_actionPlay_a_match_triggered()
{
    QString logFile = QFileDialog::getOpenFileName(this,
                                                   "Browse to the match file",
                                                   QDir::homePath(),
                                                   "XML document (*.xml)");
    if (logFile.isEmpty())
        return;

    stopLogger();
    m_logger = new Logger(this);
    m_player = new Player(this);
    connect(m_logger, SIGNAL(setBoardForMatch(QDomElement)), m_player, SLOT(setBoard(QDomElement)));
    connect(m_logger, SIGNAL(setMovesForMatch(QDomNodeList)), m_player, SLOT(setMoves(QDomNodeList)));
    if (m_logger->importMatch(logFile)) {
        m_player->showFullScreen();
    }
    else {
        delete m_player;
        QMessageBox::critical(this, "Error", "There was an error loading the file. It is either not readable or a wrong file is selected. Please try again.");
    }
    stopLogger();
}

void MainWindow::on_actionP_lay_selected_match_triggered()
{
    QModelIndex currentIndex = ui->tablesList->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::critical(this, "No match selected", "There is no match selected to export. Please select a match from the list.");
        return;
    }

    int round = ui->roundsList->currentRow();
    if (round >= m_model->rounds()) {
        QMessageBox::critical(this, "Invalid match", "Invalid round selected for export.");
        return;
    }
    int row = currentIndex.row();
    int table = row/m_model->boardCount();
    int board = row%m_model->boardCount();
    QPair<int,int> roundBoard = m_model->roundBoard(table);
    int ongoing_round = roundBoard.first;
    if (ongoing_round<round || (ongoing_round==round && roundBoard.second<=board)) {
        QMessageBox::critical(this, "Error", "This match hasn't happened yet. You can only export it after the scores for this match have arrived.");
        return;
    }

    m_player = new Player(this);
    connect(m_logger, SIGNAL(setBoardForMatch(QDomElement)), m_player, SLOT(setBoard(QDomElement)));
    connect(m_logger, SIGNAL(setMovesForMatch(QDomNodeList)), m_player, SLOT(setMoves(QDomNodeList)));
    if (m_logger->loadMatch(round, table, board)) {
        m_player->showFullScreen();
    }
    else {
        disconnect(m_logger, 0, m_player, 0);
        delete m_player;
    }
}
