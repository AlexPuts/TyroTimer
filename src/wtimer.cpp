#include "src/wtimer.h"
#include "ui_wtimer.h"
#include "src/logger.h"

#include <QMenu>
#include <QSystemTrayIcon>
#include <QDesktopWidget>
#include <QtWidgets>
#include <QSizePolicy>
#include "src/app.h"
#include "src/settingsform.h"
#include <QSound>
#include <QDebug>
#include "ui_settingsform.h"


WTimer::WTimer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WTimer)
{

    pst = App::theApp()->settings();
    lastPos = new QPoint(0,0);
    soundBreak = new QSound(":sounds/break.wav");
    soundComplete = new QSound(":sounds/complete.wav");
    soundNotification = new QSound(":sounds/notification.wav");
    soundProcess = new QSound(":sounds/process.wav");
    soundAlertAfterBreak = new QSound(":sounds/alert.wav");
    soundProcess->setLoops(-1);
    soundBreak->setLoops(-1);

    isABreak = false;

    ui->setupUi(this);

    Settings=nullptr;
    ui->start_stop->setText("Start");
    ptimer = new QTimer(this);
    pStimer = new QTimer(this);

    connect(ptimer,SIGNAL(timeout()),this,SLOT(slotSetDisplay()));
    connect(pStimer,SIGNAL(timeout()),this,SLOT(slotAlertAfterBreak()));

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

    pactStartSession=
            new QAction("&Start session",this);
    connect(pactStartSession,SIGNAL(triggered()),
            this,        SLOT(slotStartStop())
            );
    pactStopSession=
            new QAction("&Stop",this);
    connect(pactStopSession,SIGNAL(triggered()),
            this,        SLOT(slotStartStop())
            );
    pactStartBreak=
            new QAction("&Take a break",this);
    connect(pactStartBreak,SIGNAL(triggered()),
            this,        SLOT(slotTakeABreak())
            );
    pactStopBreak=
            new QAction("&Stop break",this);
    connect(pactStopBreak,SIGNAL(triggered()),
            this,        SLOT(slotStartStop())
            );
    pactSkipBreak=
            new QAction("&Skip break",this);
    connect(pactSkipBreak,SIGNAL(triggered()),
            this,        SLOT(slotSkipABreak())
            );


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
    connect(pactQuit,SIGNAL(triggered()),this,SLOT(slotSaveWinPos()));
    connect(pactQuit,SIGNAL(triggered()),qApp,SLOT(quit()));


    ui->start_stop->setAutoDefault(false);
    connect(ui->start_stop,SIGNAL(clicked()),this,SLOT(slotStartStop()));

    ptrayIconMenu = new QMenu(this);


    ptrayIconMenu->addAction(pactStartSession);
    ptrayIconMenu->addAction(pactStopSession);
    ptrayIconMenu->addAction(pactStartBreak);
    ptrayIconMenu->addAction(pactStopBreak);
    ptrayIconMenu->addAction(pactSkipBreak);

    pactStartSession->setVisible(false);
    pactStopSession->setVisible(false);
    pactStartBreak->setVisible(false);
    pactStopBreak->setVisible(false);
    pactSkipBreak->setVisible(false);



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


    Statistics = new statisticsDialog;
    Statistics->hide();

    connect(this,SIGNAL(signalCheckStatistics()),Statistics,SLOT(slotCheckStatistics()));

    slotReadSettings();
    if (Frameless) this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    timeValue = WTimerDuration;
    ui->timeLabel->setText(timeValue.toString());
    ui->progressBar->setValue(0);
    if(timerControlsInTrMenu)pactStartSession->setVisible(true);
    if(!startMinimized)slotShowHide();
    breakHrs = pst->value("breakHrs").toDouble();
    breaks = pst->value("breaks").toInt();
    hours = pst->value("hours").toDouble();
    sessions = pst->value("sessions").toInt();


    Settings->journalPlText->setReadOnly(true);

    QString fileName = "logs/WTimer log " +
            QDateTime::currentDateTime().toString("MM yyyy") + ".txt";
    logger = new Logger(this, fileName, Settings->journalPlText);


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
        if(lastPos->x()!=0&&lastPos->y()!=0&&haveLastPos)
        {
            this->move(*lastPos);
        }
        else this->move(pos);
    }
    else this->move((QApplication::desktop()->width()-this->width())-0,
               (QApplication::desktop()->height()-this->height())-0);

    setVisible(!isVisible());
}



void::WTimer::slotChangeIcon()
{
    IconSwitcher = !IconSwitcher;
    QString strPixmapName = IconSwitcher ? ":/images/wtred.png"
                                            : ":/images/wtgreen.png";
    QPixmap pixmap(strPixmapName);
    QPainter painter(&pixmap);
    painter.drawText(pixmap.rect(),Qt::AlignCenter,"Hi!");

    ptrayIcon->setIcon(QPixmap(strPixmapName));


}

void WTimer::slotStartStop()
{

    ui->progressBar->setMaximum(timeValue.minute()*60);

    if(!ptimer->isActive())
    {
        ui->start_stop->setText("Stop"); 
        if(timerControlsInTrMenu)pactStopSession->setVisible(true);
        if(timerControlsInTrMenu)pactStartSession->setVisible(false);
        ptimer->start(1000);
       if(AlertBubbleStart) ptrayIcon->showMessage("WTimer status:",
                                 "Work session started.",
                                 QSystemTrayIcon::Information,2200);
        if(Process)soundProcess->play();
    }
    else
    {
        ui->start_stop->setText("Start");

        if(timerControlsInTrMenu)pactStartSession->setVisible(true);
        if(timerControlsInTrMenu)pactStopSession->setVisible(false);
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
    if(keepAlertAfterBreak) pStimer->start(60000);
    this->timeValue.setHMS(0,this->timeValue.addSecs(-1).minute(),
                           this->timeValue.addSecs(-1).second());
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

            if(timerControlsInTrMenu)pactStartSession->setVisible(true);
            if(timerControlsInTrMenu)pactStopSession->setVisible(false);

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
           if(keepJournal)logger->write("Break of " +QString::number(breakDuration.minute()) +
                                        " minutes completed\n" );
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
    if(keepJournal)logger->write("Work session of " +QString::number(WTimerDuration.minute()) +
                                 " minutes completed\n" );
    pst->setValue("hours", hours);
    sessions += 1;
    pst->setValue("sessions",sessions);
    if(timerControlsInTrMenu)pactStartSession->setVisible(false);
    if(timerControlsInTrMenu)pactStopSession->setVisible(false);
    ui->start_stop->setVisible(false);
    this->takeABreak->setVisible(true);
    this->skipABreak->setVisible(true);
    if(timerControlsInTrMenu)pactStartBreak->setVisible(true);
    if(timerControlsInTrMenu)pactSkipBreak->setVisible(true);
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

    if(timerControlsInTrMenu)pactStartBreak->setVisible(false);
    if(timerControlsInTrMenu)pactStopBreak->setVisible(false);
    if(timerControlsInTrMenu)pactSkipBreak->setVisible(false);
    if(timerControlsInTrMenu)pactStopSession->setVisible(true);

}

void WTimer::slotSkipABreak()
{
    this->takeABreak->setVisible(false);
    this->skipABreak->setVisible(false);

    if(timerControlsInTrMenu)pactStartBreak->setVisible(false);
    if(timerControlsInTrMenu)pactStopBreak->setVisible(false);
    if(timerControlsInTrMenu)pactSkipBreak->setVisible(false);

    ui->start_stop->setVisible(true);

    if(timerControlsInTrMenu)pactStartSession->setVisible(true);

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
        pst->setValue("breakDuration", (5*60));
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
    timerControlsInTrMenu = pst->value("timerControlsInTrMenu").toBool();

    pst->setValue("WTimerDuration", WTimerDuration.minute()*60);
    pst->setValue("breakDuration", breakDuration.minute()*60);
    pst->setValue("soundInProcess", Process);
    pst->setValue("soundInBreak", Break);
    pst->setValue("soundInComplete", SessionComplete);
    pst->setValue("soundInBComplete", BreakComplete);
    pst->setValue("Notification", Notification);
    pst->setValue("PopUp", PopUp);
    pst->setValue("Frameless", Frameless);

    pst->setValue("AlertBubbleStart", AlertBubbleStart);
    pst->setValue("AlertBubbleEnd", AlertBubbleEnd);
    pst->setValue("portableConfig", portableConfig);
    pst->setValue("highDpi", highDpi);
    pst->setValue("keepJournal", keepJournal);
    pst->setValue("keepAlertAfterBreak", keepAlertAfterBreak);
    pst->setValue("keepWindowPos", keepWindowPos);
    pst->setValue("startMinimized", startMinimized);
    pst->setValue("timerControlsInTrMenu", timerControlsInTrMenu);

    if(timerControlsInTrMenu)pactStartSession->setVisible(true);
    if(!timerControlsInTrMenu)pactStartSession->setVisible(false);
}


void WTimer::slotStatistics()
{
    emit signalCheckStatistics();
    this->Statistics->show();
}


void WTimer::slotSaveWinPos()
{
    pst->setValue("xPos", xPos);
    pst->setValue("yPos", yPos);
     qDebug() << "xPos : " << xPos << "yPos"<< yPos;
}

void WTimer::moveEvent(QMoveEvent *event)
{
    qDebug() << (QString::number(x())+","+QString::number(y()));
    *lastPos = pos();
    xPos = x();
    yPos = y();
    haveLastPos = true;
    qDebug() << "Keep window pos : " << keepWindowPos;
    slotSaveWinPos();
}

void WTimer::slotAlertAfterBreak()
{
    if(keepAlertAfterBreak)
    {
    soundAlertAfterBreak->play();
    ptrayIcon->showMessage("WTimer says: ",
                           "You are slacking... Get back here!",
                            QSystemTrayIcon::Information,4200);
    }
}

