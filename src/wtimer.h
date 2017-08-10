#ifndef WTimer_H
#define WTimer_H
#include <QLabel>
#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QPushButton>
#include "src/settingsform.h"
#include <QSound>
#include "src/statisticsdialog.h"
#include <QPlainTextEdit>
#include "src/logger.h"
class QSystemTrayIcon;
class QMenu;

namespace Ui {
class WTimer;
}

class WTimer : public QWidget
{
    Q_OBJECT

public:
    explicit WTimer(QWidget *parent = 0);
    bool started;
    QTimer* ptimer;
    QTimer* pStimer;

    QSettings* pst;

    QTime  timeValue; 
    QTime WTimerDuration;
    QTime breakDuration;

    QPushButton* takeABreak;
    QPushButton* skipABreak;

    bool isABreak;
    bool Process;
    bool Break;
    bool SessionComplete;
    bool BreakComplete;
    bool Notification;
    bool Frameless;
    bool PopUp;



    QAction* pactStartSession;
    QAction* pactStopSession;
    QAction* pactStartBreak;
    QAction* pactStopBreak;
    QAction* pactSkipBreak;



    bool AlertBubbleStart; //
    bool AlertBubbleEnd; //
    bool portableConfig;
    bool highDpi;
    bool keepJournal;
    bool keepAlertAfterBreak;
    bool keepWindowPos;  //
    bool startMinimized; //

    bool haveLastPos;
    bool timerControlsInTrMenu;

    settingsForm* Settings;
    statisticsDialog* Statistics;


    QSound* soundBreak;
    QSound* soundComplete;
    QSound* soundNotification;
    QSound* soundProcess;
    QSound* soundAlertAfterBreak;


    QPoint* lastPos;

    double breakHrs;
    int breaks;
    double hours;
    int sessions;
    int xPos;
    int yPos;

    Logger *logger;

    ~WTimer();

private:
    Ui::WTimer *ui;
    QSystemTrayIcon* ptrayIcon;
    QMenu* ptrayIconMenu;
    bool IconSwitcher;

    void moveEvent(QMoveEvent *event);
protected:
    virtual void closeEvent();
signals:
    void signalCheckSettings();
    void signalCheckStatistics();
public slots:
    void slotShowHide();
    void slotChangeIcon();
    void slotStartStop();
    void slotWTimerEnded();
    void slotSetDisplay();
    void slotTakeABreak();
    void slotSkipABreak();
    void slotSettings();
    void slotReadSettings();
    void slotStatistics();
    void slotSaveWinPos();
    void slotAlertAfterBreak();

private slots:

};

#endif // WTimer_H
