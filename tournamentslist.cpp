#include "tournamentslist.h"
#include "ui_tournamentslist.h"

#include <QDomDocument>
#include <QFile>
#include <QDir>

TournamentsList::TournamentsList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TournamentsList)
{
    ui->setupUi(this);
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));

    QDir db(QDir::homePath() + "/Bridge Club/", "*.xml", QDir::Name|QDir::IgnoreCase, QDir::Files|QDir::Readable);
    foreach(QString filename, db.entryList()) {
        filename = db.path()+"/"+filename;
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;
        QDomDocument doc;
        doc.setContent(&file);
        file.close();
        QString timestamp = doc.documentElement().elementsByTagName("timestamp").at(0).toElement().text();
        QListWidgetItem *item = new QListWidgetItem(timestamp);
        item->setData(Qt::WhatsThisRole, filename);
        ui->listWidget->addItem(item);
    }
}

TournamentsList::~TournamentsList()
{
    delete ui;
}

QString TournamentsList::selectedItem()
{
    return ui->listWidget->currentItem()->data(Qt::WhatsThisRole).toString();
}

void TournamentsList::on_buttonBox_accepted()
{
    if (!ui->listWidget->currentItem())
        return;
    accept();
}
