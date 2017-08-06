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


WTimer::WTimer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WTimer)
{
    slotReadSettings();
    pst = App::theApp()->settings();

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

    QAction* pactAbout=
            new QAction("&About",this);
    connect(pactAbout,SIGNAL(triggered()),
            this,        SLOT(slotAbout())
            );

    QAction* pactStatistics=
            new QAction("&Statistics",this);
    connect(pactStatistics,SIGNAL(triggered()),
            this,        SLOT(slotStatistics())
            );

    QAction* pactQuit = new QAction("&Quit",this);
    connect(pactQuit,SIGNAL(triggered()),qApp,SLOT(quit()));
    ui->start_stop->setAutoDefault(false);
    connect(ui->start_stop,SIGNAL(clicked()),this,SLOT(slotStartStop()));

    m_ptrayIconMenu = new QMenu(this);

    m_ptrayIconMenu->addAction(pactStatistics);
     m_ptrayIconMenu->addAction(pactAbout);
    m_ptrayIconMenu->addAction(pactSettings);
    m_ptrayIconMenu->addAction(pactShowHide);
    m_ptrayIconMenu->addAction(pactQuit);


    m_ptrayIcon = new QSystemTrayIcon(this);
    m_ptrayIcon->setContextMenu(m_ptrayIconMenu);
    m_ptrayIcon->setToolTip("System Tray");

    slotChangeIcon();

    Settings = new settingsForm;
    about = new About;
    about->hide();
    connect(Settings,SIGNAL(saveSettings()), this, SLOT(slotReadSettings()));
    Settings->move((QApplication::desktop()->width()-this->width())/2,
                   (QApplication::desktop()->height()-this->height())/2);

     connect(pactSettings,SIGNAL(triggered()), Settings,SLOT(slotCheckSettings()));


    m_ptrayIcon->show();
    slotShowHide();

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
    if(m_ptrayIcon->isVisible())
    {
        hide();
    }
}
void WTimer::slotShowHide()
{
    this->move((QApplication::desktop()->width()-this->width())-0,
               (QApplication::desktop()->height()-this->height())-0);
    setVisible(!isVisible());
}

void WTimer::slotShowMessage()
{
    m_ptrayIcon->showMessage("For your information",
                             "You have selected the "
                             "\"Show Message!\"option",
                             QSystemTrayIcon::Information,
                             3000);
}

void::WTimer::slotChangeIcon()
{
    m_bIconSwitcher = !m_bIconSwitcher;
    QString strPixmapName = m_bIconSwitcher ? ":/images/wtred.png"
                                            : ":/images/wtgreen.png";
    m_ptrayIcon->setIcon(QPixmap(strPixmapName));
}

void WTimer::slotStartStop()
{
    ui->progressBar->setMaximum(timeValue.minute()*60);

    if(!ptimer->isActive())
    {
        ui->start_stop->setText("Stop");
        ptimer->start(1000);
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
    //change countdownd display, move progress bar
    //move animation ?
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
    if(pst->value("soundInProcess").toBool()==0)
    {
        pst->setValue("soundInProcess", false);
    }

    if(pst->value("soundInBreak").toBool()==0)
    {
        pst->setValue("soundInBreak", false);
    }

    if(pst->value("soundInComplete").toBool()==0)
    {
        pst->setValue("soundInComplete", false);
    }

    if(pst->value("soundInBComplete").toBool()==0)
    {
        pst->setValue("soundInBComplete", false);
    }

    if(pst->value("Notification").toBool()==0)
    {
        pst->setValue("Notification", false);
    }

    if(pst->value("Frameless").toBool()==0)
    {
        pst->setValue("Frameless", false);
    }

    if(pst->value("PopUp").toBool()==0)
    {
        pst->setValue("PopUp", false);
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

}

void WTimer::slotAbout()
{
    about->show();
}

void WTimer::slotStatistics()
{
    emit signalCheckStatistics();
    this->Statistics->show();
}
