#ifndef NEWEVENTDIALOG_H
#define NEWEVENTDIALOG_H

#include <QDialog>

namespace Ui {
class NewEventDialog;
}

class NewEventDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewEventDialog(QWidget *parent = 0);
    ~NewEventDialog();
    int pairs();
    int movement();
    int boardCount();
    int scoring();
    
private slots:
    void on_buttonBox_accepted();

private:
    Ui::NewEventDialog *ui;
};

#endif // NEWEVENTDIALOG_H
