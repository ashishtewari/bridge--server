#ifndef LIGHTSWIDGET_H
#define LIGHTSWIDGET_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class LightsWidget;
}

class LightsWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit LightsWidget(QWidget *parent = 0);
    ~LightsWidget();
    void switchLight(int index, bool on);
    
private:
    Ui::LightsWidget *ui;
    QList<QLabel*> m_lights;
};

#endif // LIGHTSWIDGET_H
