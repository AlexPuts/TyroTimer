#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>
#include <QSettings>
#include <QPlainTextEdit>
#include <QDesktopServices>
#include "wtimertask.h"
#include "statisticsdialog.h"
#include <QMediaPlayer>
#include <QUrl>
#include <QSlider>

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

    bool skeepJournal;
    bool skeepAlertAfterBreak;
    bool skeepWindowPos;
    bool sstartMinimized;
    QPlainTextEdit *journalPlText;
    bool stimerControlsInTrMenu;
    bool suseTaskSystem;

    QVector <WtimerTask> * tasks;
    QVector <QSlider*> sliders;
    QSettings* pst;
    statisticsDialog* Statistics;
    QUrl uProcess,uBreak,uSessionComplete,uBreakComplete,uNotification;
    int vProcess,vBreak,vSessionComplete,vBreakComplete,vNotification;
    void createTask(QString s_taskTitle);
    void deleteTask();
    void refreshTaskList();

    ~settingsForm();
public slots:
    void slotSendSettings();
    void slotCheckSettings();
    void slotAddTask();
    void slotDeleteTask();
    void slotOpenJournalFolder();
    void slotReadTasks();
    void slotWriteTasks();
    void slotShowStatistics();


private:
    Ui::settingsForm *ui;
signals:
    void saveSettings();
    void signalAddTask(QString taskTitle);
};

#endif // SETTINGSFORM_H
