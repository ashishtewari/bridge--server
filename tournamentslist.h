#ifndef TOURNAMENTSLIST_H
#define TOURNAMENTSLIST_H

#include <QDialog>

namespace Ui {
class TournamentsList;
}

class TournamentsList : public QDialog
{
    Q_OBJECT
    
public:
    explicit TournamentsList(QWidget *parent = 0);
    ~TournamentsList();
    QString selectedItem();
    
private slots:
    void on_buttonBox_accepted();

private:
    Ui::TournamentsList *ui;
};

#endif // TOURNAMENTSLIST_H
