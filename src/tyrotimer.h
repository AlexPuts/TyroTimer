#ifndef TyroTimer_H
#define TyroTimer_H

#include "src/logger.h"
#include "src/statisticsdialog.h"
#include "src/settingsform.h"
#include "src/tyrotimertask.h"

#include <QLabel>
#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QPushButton>
#include <QSound>
#include <QPlainTextEdit>
#include <QComboBox>

class QSystemTrayIcon;
class QMenu;

namespace Ui {
class TyroTimer;
}

class TyroTimer : public QWidget
{
    Q_OBJECT

public:
    explicit TyroTimer(QWidget *parent = 0);
    bool started;
    QTimer* ptimer;
    QTimer* pStimer;

    QSettings* pst;

    QTime  timeValue; 
    QTime TyroTimerDuration;
    QTime breakDuration;

    QPushButton* takeABreak;
    QPushButton* skipABreak;
    QComboBox* taskComboBox;

    bool isABreak;
    bool Process;
    bool Break;
    bool SessionComplete;
    bool BreakComplete;
    bool Notification;
    bool Frameless;
    bool PopUp;

    QVector <TyroTimerTask> * tasks;


    QAction* pactStartSession;
    QAction* pactStopSession;
    QAction* pactStartBreak;
    QAction* pactStopBreak;
    QAction* pactSkipBreak;



    bool AlertBubbleStart; //
    bool AlertBubbleEnd; //
    bool keepJournal;
    bool keepAlertAfterBreak;
    bool keepWindowPos;  //
    bool startMinimized; //

    bool haveLastPos;
    bool timerControlsInTrMenu;

    bool useTaskSystem;
    settingsForm* Settings;



    QSound* soundBreak;
    QSound* soundComplete;
    QSound* soundNotification;
    QSound* soundProcess;
    QSound* soundAlertAfterBreak;


    QPoint* lastPos;

    double breakHrs;
    int breaks;
    qint32 lastActiveTask;
    double hours;
    int sessions;
    int xPos;
    int yPos;

    Logger *logger;



    ~TyroTimer();

private:
    Ui::TyroTimer *ui;
    QSystemTrayIcon* ptrayIcon;
    QMenu* ptrayIconMenu;
    bool IconSwitcher;
    void delay();
    void moveEvent(QMoveEvent *event);
protected:
    virtual void closeEvent();
signals:
    void signalCheckSettings();

public slots:
    void slotShowHide();
    void slotUpdateIcon(int state,int number);
    void slotStartStop();
    void slotTyroTimerEnded();
    void slotSetDisplay();
    void slotTakeABreak();
    void slotSkipABreak();
    void slotSettings();
    void slotReadSettings();
    void slotSaveWinPos();
    void slotAlertAfterBreak();
    void slotReadTasks();
    void slotWriteTasks();
    void slotSaveLastActiveTask();
    void slotQuit();

private slots:

};

#endif // TyroTimer_H
