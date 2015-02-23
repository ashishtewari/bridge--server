#include "neweventdialog.h"
#include "ui_neweventdialog.h"
#include "howell.h"
#include <QMessageBox>


NewEventDialog::NewEventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewEventDialog)
{
    ui->setupUi(this);
    ui->pairSpin->selectAll();
}

NewEventDialog::~NewEventDialog()
{
    delete ui;
}

int NewEventDialog::pairs() { return ui->pairSpin->value(); }

int NewEventDialog::movement() { return ui->moveCombo->currentIndex(); }

int NewEventDialog::boardCount() { return ui->boardSpin->value(); }

int NewEventDialog::scoring() { return ui->scoringCombo->currentIndex(); }

void NewEventDialog::on_buttonBox_accepted()
{
    int p = pairs(), min = Howell::min_pairs(), max = Howell::max_pairs();
    if (movement() == 1 && (p%2!=0 || p<min || p>max)) {
        QMessageBox::warning(this, "Invalid input", "For Howell movement, pairs should be:\n- Even, and\n- Between "+QString::number(min)+" and "+QString::number(max));
        return;
    }
    accept();
}
