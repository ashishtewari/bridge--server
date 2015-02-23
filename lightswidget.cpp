#include "lightswidget.h"
#include "ui_lightswidget.h"

LightsWidget::LightsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LightsWidget)
{
    ui->setupUi(this);
    m_lights << ui->label;
    m_lights << ui->label_2;
    m_lights << ui->label_3;
    m_lights << ui->label_4;
}

LightsWidget::~LightsWidget()
{
    delete ui;
}

void LightsWidget::switchLight(int index, bool on)
{
    m_lights[index]->setPixmap(QPixmap( on ? ":/images/green.png" : ":/images/yellow.png" ));
}
