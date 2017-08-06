#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class settingsForm;
}

class settingsForm : public QWidget
{
    Q_OBJECT

public:
    settingsForm(QWidget *parent = 0);
    int  cDuration;
    int  cBreak;
    bool sProcess;
    bool sBreak;
    bool sSessionComplete;
    bool sBreakComplete;
    bool sNotification;
    bool sPopUp;
    bool sFrameless;
    QSettings* pst;
    ~settingsForm();
public slots:
    void slotSendSettings();
    void slotCheckSettings();
private:
    Ui::settingsForm *ui;
signals:
    void saveSettings();
};

#endif // SETTINGSFORM_H
