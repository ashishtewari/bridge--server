#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

class Server;
class MovementModel;
class Logger;
class Player;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionNew_Event_triggered();
    void on_roundsList_currentRowChanged(int currentRow);
    void on_actionStart_Server_triggered();
    void updateLights(int table, int player, bool on);
    void showResult();
    void stopServer();
    void stopLogger();
    void stopMovementModel();
    inline void stopEvent() { stopServer(); stopLogger(); stopMovementModel(); }
    void on_actionQuit_triggered();
    void on_actionExport_Tournament_triggered();
    void on_actionImport_Tournament_triggered();
    void initialiseView(int pairs, int boardCount, int movement, int scoring);
    void on_actionExport_selected_match_triggered();
    void on_actionLoad_local_tournament_triggered();
    void on_actionPlay_a_match_triggered();
    void on_actionP_lay_selected_match_triggered();

private:
    Ui::MainWindow *ui;
    MovementModel *m_model;
    Server *m_server;
    Logger *m_logger;
    Player *m_player;

    QStandardItemModel* newMovementModel(int round);
    void loadLogFile(QString filepath);
};

#endif // MAINWINDOW_H
