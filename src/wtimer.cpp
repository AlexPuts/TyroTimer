#include "src/wtimer.h"
#include "ui_wtimer.h"

#include <QMenu>
#include <QSystemTrayIcon>
#include <QDesktopWidget>
#include <QtWidgets>
#include <QSizePolicy>
#include "src/app.h"
#include "src/settingsform.h"
#include <QSound>
#include <QDebug>



struct SettingsStruct
{
    QTime  timeValue;
    QTime WTimerDuration;
    QTime breakDuration;

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

};

















WTimer::WTimer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WTimer)
{
    slotReadSettings();
    pst = App::theApp()->settings();

    lastPos = new QPoint(0,0);
    soundBreak = new QSound(":sounds/break.wav");
    soundComplete = new QSound(":sounds/complete.wav");
    soundNotification = new QSound(":sounds/notification.wav");
    soundProcess = new QSound(":sounds/process.wav");
    soundProcess->setLoops(-1);
    soundBreak->setLoops(-1);

    isABreak = false;
    if (Frameless) this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    ui->setupUi(this);

    Settings=nullptr;
    ui->start_stop->setText("Start");
    ptimer = new QTimer(this);
    timeValue = WTimerDuration;
    ui->timeLabel->setText(timeValue.toString());
    ui->progressBar->setValue(0);
    connect(ptimer,SIGNAL(timeout()),this,SLOT(slotSetDisplay()));

    takeABreak = new QPushButton ("Take a break");
    takeABreak->setVisible(false);
    takeABreak->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);

    ui->horizontalLayout->addWidget(takeABreak);
    connect(this->takeABreak,SIGNAL(clicked(bool)),this, SLOT(slotTakeABreak()));

    skipABreak = new QPushButton ("Skip break");
    skipABreak->setVisible(false);
    skipABreak->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);
    ui->horizontalLayout->addWidget(skipABreak);
    connect(this->skipABreak,SIGNAL(clicked(bool)),this, SLOT(slotSkipABreak()));



    setWindowTitle("WTimer");
    QAction* pactShowHide=
            new QAction("&Show/Hide",this);
    connect(pactShowHide,SIGNAL(triggered()),
            this,        SLOT(slotShowHide())
            );

    QAction* pactSettings=
            new QAction("&Settings",this);
    connect(pactSettings,SIGNAL(triggered()),
            this,        SLOT(slotSettings())
            );

    QAction* pactStatistics=
            new QAction("&Statistics",this);
    connect(pactStatistics,SIGNAL(triggered()),
            this,        SLOT(slotStatistics())
            );

    QAction* pactQuit = new QAction("&Quit",this);
    connect(pactQuit,SIGNAL(triggered()),qApp,SLOT(quit()));
    if(keepWindowPos) connect(pactQuit,SIGNAL(triggered()),this,SLOT(slotSaveWinPos()));

    ui->start_stop->setAutoDefault(false);
    connect(ui->start_stop,SIGNAL(clicked()),this,SLOT(slotStartStop()));

    ptrayIconMenu = new QMenu(this);

    ptrayIconMenu->addAction(pactStatistics);

    ptrayIconMenu->addAction(pactSettings);
    ptrayIconMenu->addAction(pactShowHide);
    ptrayIconMenu->addAction(pactQuit);


    ptrayIcon = new QSystemTrayIcon(this);
    ptrayIcon->setContextMenu(ptrayIconMenu);
    ptrayIcon->setToolTip("System Tray");

    slotChangeIcon();

    Settings = new settingsForm;

    connect(Settings,SIGNAL(saveSettings()), this, SLOT(slotReadSettings()));
    Settings->move((QApplication::desktop()->width()-this->width())/2,
                   (QApplication::desktop()->height()-this->height())/2);

     connect(pactSettings,SIGNAL(triggered()), Settings,SLOT(slotCheckSettings()));


    ptrayIcon->show();
    if(!startMinimized)slotShowHide();

    Statistics = new statisticsDialog;
    Statistics->hide();

    connect(this,SIGNAL(signalCheckStatistics()),Statistics,SLOT(slotCheckStatistics()));

    breakHrs = pst->value("breakHrs").toDouble();
    breaks = pst->value("breaks").toInt();
    hours = pst->value("hours").toDouble();
    sessions = pst->value("sessions").toInt();


}

WTimer::~WTimer()
{
    delete ui;
}


void WTimer::closeEvent()
{
    slotSaveWinPos();
    if(ptrayIcon->isVisible())
    {
      hide();
    }

}
void WTimer::slotShowHide()
{
    int x = pst->value("xPos").toInt();
    int y = pst->value("yPos").toInt();
    QPoint pos(x,y);

    if(keepWindowPos)
    {
        if(lastPos->x()!=0&&lastPos->y()!=0)
        {
            this->move(*lastPos);
        }
        else this->move(pos);
    }

    else this->move((QApplication::desktop()->width()-this->width())-0,
               (QApplication::desktop()->height()-this->height())-0);
    setVisible(!isVisible());
}

void WTimer::slotShowMessage()
{

}

void::WTimer::slotChangeIcon()
{
    IconSwitcher = !IconSwitcher;
    QString strPixmapName = IconSwitcher ? ":/images/wtred.png"
                                            : ":/images/wtgreen.png";
    ptrayIcon->setIcon(QPixmap(strPixmapName));
}

void WTimer::slotStartStop()
{
    ui->progressBar->setMaximum(timeValue.minute()*60);

    if(!ptimer->isActive())
    {
        ui->start_stop->setText("Stop");
        ptimer->start(1000);
       if(AlertBubbleStart) ptrayIcon->showMessage("WTimer status:",
                                 "Work session started.",
                                 QSystemTrayIcon::Information,2200);
        if(Process)soundProcess->play();
    }
    else
    {
        ui->start_stop->setText("Start");
        soundProcess->stop();
        soundBreak->stop();
        ptimer->stop();
        timeValue = WTimerDuration;
        this->ui->timeLabel->setText(this->timeValue.toString());
        ui->progressBar->setValue(0);
        if(isABreak)
        {
            slotChangeIcon();
            isABreak = false;
        }
    }
}
void WTimer::slotSetDisplay()
{
    this->timeValue.setHMS(0,this->timeValue.addSecs(-1).minute(),this->timeValue.addSecs(-1).second());
    this->ui->timeLabel->setText(this->timeValue.toString());
    this->ui->progressBar->setValue(ui->progressBar->value()+1);
    if(this->timeValue.minute() ==0 &&this->timeValue.second() ==0)
    {
        if(SessionComplete)soundComplete->play();
        if(!isABreak)
        {
            ptimer->stop();
            slotWTimerEnded();
           if(AlertBubbleEnd) ptrayIcon->showMessage("WTimer status:",
                                     "Work session completed.",
                                     QSystemTrayIcon::Information,2200);
        }
        else
        {
            soundBreak->stop();
            ptimer->stop();
            timeValue = WTimerDuration;
            ui->progressBar->setMaximum(timeValue.minute()*60);
            ui->start_stop->setText("Start");
            ui->progressBar->setValue(0);
            this->ui->timeLabel->setText(this->timeValue.toString());
            slotChangeIcon();
            isABreak = false;

            breakHrs += breakDuration.minute()/60.0f;
            pst->setValue("breakHrs", breakHrs);
            breaks += 1;
            pst->setValue("breaks",breaks);

           if(AlertBubbleEnd) ptrayIcon->showMessage("WTimer status:",
                                     "Break completed.\n Get back to work!",
                                     QSystemTrayIcon::Information,2200);
        }
    }
    else if (PopUp && this->timeValue.minute() ==1 &&this->timeValue.second() ==0)
    {
        if(Notification)soundNotification->play();
        setVisible(true);
    }
};
void WTimer::slotWTimerEnded()
{
    //play sound, increment total WTimers
    //need to setup a header like work and like rest

    hours += WTimerDuration.minute()/60.0f;
    pst->setValue("hours", hours);
    sessions += 1;
    pst->setValue("sessions",sessions);

    ui->start_stop->setVisible(false);
    this->takeABreak->setVisible(true);
    this->skipABreak->setVisible(true);
    soundProcess->stop();
}

void WTimer::slotTakeABreak()
{
    timeValue = breakDuration;
   if(AlertBubbleStart) ptrayIcon->showMessage("WTimer status:",
                             "Break started.",
                             QSystemTrayIcon::Information,2200);
    ui->progressBar->setMaximum(timeValue.minute()*60);
    ptimer->start(1000);
    if(Break)soundBreak->play();
    this->ui->timeLabel->setText(this->timeValue.toString());
    ui->progressBar->setValue(0);
    this->takeABreak->setVisible(false);
    this->skipABreak->setVisible(false);
    ui->start_stop->setText("Stop");
    ui->start_stop->setVisible(true);
    isABreak = true;
    slotChangeIcon();
}

void WTimer::slotSkipABreak()
{
    this->takeABreak->setVisible(false);
    this->skipABreak->setVisible(false);
    ui->start_stop->setVisible(true);
    ui->start_stop->setText("Start");
    this->ptimer->stop();
    timeValue = WTimerDuration;
    ui->progressBar->setMaximum(timeValue.minute()*60);
    ui->progressBar->setValue(0);
    this->ui->timeLabel->setText(this->timeValue.toString());

}
void WTimer::slotSettings()
{
emit signalCheckSettings();
Settings->show();
Settings->raise();
}

void WTimer::slotReadSettings()
{
    QSettings* pst = App::theApp()->settings();
    pst->setValue("Language","en");
    if(pst->value("WTimerDuration").toInt()==0)
    {
        pst->setValue("WTimerDuration", (25*60));
    }

    if(pst->value("breakDuration").toInt()==0)
    {
        pst->setValue("breakDuration", 5*60);
    }

    WTimerDuration = QTime(0,0,0);
    breakDuration = QTime(0,0,0);
    WTimerDuration=WTimerDuration.addSecs(pst->value("WTimerDuration").toInt());
    breakDuration=breakDuration.addSecs(pst->value("breakDuration").toInt());

    Process = pst->value("soundInProcess").toBool();
    Break = pst->value("soundInBreak").toBool();
    SessionComplete = pst->value("soundInComplete").toBool();
    BreakComplete = pst->value("soundInBComplete").toBool();
    Notification = pst->value("soundInBComplete").toBool();
    Frameless = pst->value("Frameless").toBool();
    PopUp = pst->value("PopUp").toBool();

    AlertBubbleStart = pst->value("AlertBubbleStart").toBool();
    AlertBubbleEnd = pst->value("AlertBubbleEnd").toBool();
    portableConfig = pst->value("portableConfig").toBool();
    highDpi = pst->value("highDpi").toBool();
    keepJournal = pst->value("keepJournal").toBool();
    keepAlertAfterBreak = pst->value("keepAlertAfterBreak").toBool();
    keepWindowPos = pst->value("keepWindowPos").toBool();
    startMinimized = pst->value("startMinimized").toBool();

}


void WTimer::slotStatistics()
{
    emit signalCheckStatistics();
    this->Statistics->show();
}


void WTimer::slotSaveWinPos()
{
    pst->setValue("xPos", this->x());
    pst->setValue("yPos", this->y());
}

void WTimer::slotReadLocalSettings()
{

}
void WTimer::moveEvent()
{
    *lastPos = pos();
}

void WTimer::slotSaveLocalSettings()
{

}
void WTimer::slotFillSettingsStruct()
{
    struct SettingsStruct settingsSt;
    settingsSt.WTimerDuration = QTime(0,0,0);
    settingsSt.breakDuration = QTime(0,0,0);
    settingsSt.WTimerDuration=WTimerDuration.addSecs(pst->value("WTimerDuration").toInt());
    settingsSt.breakDuration=breakDuration.addSecs(pst->value("breakDuration").toInt());

    settingsSt.Process = pst->value("soundInProcess").toBool();
    settingsSt.Break = pst->value("soundInBreak").toBool();
    settingsSt.SessionComplete = pst->value("soundInComplete").toBool();
    settingsSt.BreakComplete = pst->value("soundInBComplete").toBool();
    settingsSt.Notification = pst->value("soundInBComplete").toBool();
    settingsSt.Frameless = pst->value("Frameless").toBool();
    settingsSt.PopUp = pst->value("PopUp").toBool();

    settingsSt.AlertBubbleStart = pst->value("AlertBubbleStart").toBool();
    settingsSt.AlertBubbleEnd = pst->value("AlertBubbleEnd").toBool();
    settingsSt.portableConfig = pst->value("portableConfig").toBool();
    settingsSt.highDpi = pst->value("highDpi").toBool();
    settingsSt.keepJournal = pst->value("keepJournal").toBool();
    settingsSt.keepAlertAfterBreak = pst->value("keepAlertAfterBreak").toBool();
    settingsSt.keepWindowPos = pst->value("keepWindowPos").toBool();
    settingsSt.startMinimized = pst->value("startMinimized").toBool();

}
