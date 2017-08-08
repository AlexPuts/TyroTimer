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


    bool AlertBubbleStart; //
    bool AlertBubbleEnd; //
    bool portableConfig;
    bool highDpi;
    bool keepJournal;
    bool keepAlertAfterBreak;
    bool keepWindowPos;  //
    bool startMinimized; //

    settingsForm* Settings;
    statisticsDialog* Statistics;


    QSound* soundBreak;
    QSound* soundComplete;
    QSound* soundNotification;
    QSound* soundProcess;
    QPoint* lastPos;
    double breakHrs;
    int breaks;
    double hours;
    int sessions;



    ~WTimer();

private:
    Ui::WTimer *ui;
    QSystemTrayIcon* ptrayIcon;
    QMenu* ptrayIconMenu;
    bool IconSwitcher;

    void moveEvent();
protected:
    virtual void closeEvent();
signals:
    void signalCheckSettings();
    void signalCheckStatistics();
public slots:
    void slotShowHide();
    void slotShowMessage();
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
    void slotReadLocalSettings();
    void slotSaveLocalSettings();
    void slotFillSettingsStruct();
private slots:

};

#endif // WTimer_H
