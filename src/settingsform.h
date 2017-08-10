#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>
#include <QSettings>
#include <QPlainTextEdit>

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

    bool sAlertBubbleStart;
    bool sAlertBubbleEnd;
    bool sportableConfig;
    bool shighDpi;
    bool skeepJournal;
    bool skeepAlertAfterBreak;
    bool skeepWindowPos;
    bool sstartMinimized;
    QPlainTextEdit *journalPlText;
    bool stimerControlsInTrMenu;

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
