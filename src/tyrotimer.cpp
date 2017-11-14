#include "src/tyrotimer.h"
#include "ui_tyrotimer.h"
#include "src/logger.h"
#include "src/app.h"
#include "src/settingsform.h"
#include "ui_settingsform.h"
#include "src/tyrotimertask.h"


#include <QMenu>
#include <QSystemTrayIcon>
#include <QDesktopWidget>
#include <QtWidgets>
#include <QSizePolicy>
#include <QSound>
#include <QDebug>



TyroTimer::TyroTimer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TyroTimer)
{
    qRegisterMetaTypeStreamOperators<TyroTimerTask>("TyroTimerTask");

    pst = App::theApp()->settings();

    lastPos = new QPoint(0,0);
    soundBreak = new QSound("sounds/break.wav");
    soundComplete = new QSound("sounds/complete.wav");
    soundNotification = new QSound("sounds/notification.wav");
    soundProcess = new QSound("sounds/process.wav");
    soundAlertAfterBreak = new QSound("sounds/alert.wav");
    soundProcess->setLoops(-1);
    soundBreak->setLoops(-1);

    isABreak = false;

    ui->setupUi(this);

    ui->start_stop->setText("Start");
    ptimer = new QTimer(this);
    pStimer = new QTimer(this);

    connect(ptimer,SIGNAL(timeout()),this,SLOT(slotSetDisplay()));
    connect(pStimer,SIGNAL(timeout()),this,SLOT(slotAlertAfterBreak()));

    takeABreak = new QPushButton ("Take a break");
    takeABreak->setVisible(false);
    takeABreak->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);

    taskComboBox = new QComboBox();
    taskComboBox->setVisible(false);
    taskComboBox->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);

    ui->verticalLayout->addWidget(taskComboBox);

    ui->horizontalLayout->addWidget(takeABreak);
    lastActiveTask =  pst->value("lastActiveTask").toInt();
    taskComboBox->setCurrentIndex(lastActiveTask);
    connect(this->takeABreak,SIGNAL(clicked(bool)),this, SLOT(slotTakeABreak()));

    skipABreak = new QPushButton ("Skip break");
    skipABreak->setVisible(false);
    skipABreak->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);
    ui->horizontalLayout->addWidget(skipABreak);
    connect(this->skipABreak,SIGNAL(clicked(bool)),this, SLOT(slotSkipABreak()));



    setWindowTitle("TyroTimer");

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


    QAction* pactQuit = new QAction("&Quit",this);
    connect(pactQuit,SIGNAL(triggered()),this,SLOT(slotQuit()));



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

    ptrayIconMenu->addAction(pactSettings);
    ptrayIconMenu->addAction(pactShowHide);
    ptrayIconMenu->addAction(pactQuit);

    ptrayIcon = new QSystemTrayIcon(this);
    ptrayIcon->setContextMenu(ptrayIconMenu);
    ptrayIcon->setToolTip("TyroTimer");


    Settings = new settingsForm;

    connect(Settings,SIGNAL(saveSettings()), this, SLOT(slotReadSettings()));
    Settings->move((QApplication::desktop()->width()-this->width())/2,
                   (QApplication::desktop()->height()-this->height())/2);

    connect(pactSettings,SIGNAL(triggered()), Settings,SLOT(slotCheckSettings()));
    connect(taskComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSaveLastActiveTask()));
    connect(ptrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(slotShowHide()));

    tasks = Settings->tasks;

    slotReadSettings();
    if (Frameless) this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    timeValue = TyroTimerDuration;
    ui->timeLabel->setText(timeValue.toString());
    slotUpdateIcon(0,timeValue.minute());

    ui->progressBar->setValue(0);
    if(timerControlsInTrMenu)pactStartSession->setVisible(true);
    if(!startMinimized)slotShowHide();
    breakHrs = pst->value("breakHrs").toDouble();
    breaks = pst->value("breaks").toInt();
    hours = pst->value("hours").toDouble();
    sessions = pst->value("sessions").toInt();


    Settings->journalPlText->setReadOnly(true);

    QString fileName = "journal/TyroTimer log " +
            QDateTime::currentDateTime().toString("MM yyyy") + ".txt";
    logger = new Logger(this, fileName, Settings->journalPlText);



}

TyroTimer::~TyroTimer()
{
    delete ui;
}


void TyroTimer::closeEvent()
{
    slotSaveWinPos();
    slotWriteTasks();
    if(ptrayIcon->isVisible())
    {
      hide();
    }

}

void TyroTimer::slotShowHide()
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



void::TyroTimer::slotUpdateIcon(int state,int number)
{
            /*QString strPixmapName = state ? ":/images/wtred.png"
                                                : ":/images/wtgreen.png";*/

            QString strPixmapName =  ":/images/wtred.png";
            QPixmap fopixmap(strPixmapName);
            QPixmap pixmap( 64, 64 );
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            painter.drawPixmap(0,0,64,64,fopixmap);
            QString string;
            string = QString::number(number);
            this->setWindowTitle("TyroTimer " + string + " min");
            painter.setFont(QFont("times",28, QFont::Bold));
            QPen penHText(QColor("#C0C0C0"));
            painter.setPen(penHText);
            painter.drawText(0,0,64,64,Qt::AlignHCenter| Qt::AlignBottom , string);

        ptrayIcon->setIcon(QIcon(pixmap));

        ptrayIcon->show();
}




void TyroTimer::slotStartStop()
{

    ui->progressBar->setMaximum(TyroTimerDuration.minute()*60);

    if(!ptimer->isActive())
    {
        timeValue = TyroTimerDuration;
        this->ui->timeLabel->setText(this->timeValue.toString());
        slotUpdateIcon(0,timeValue.minute());
        ui->progressBar->setValue(0);

        ui->start_stop->setText("Stop"); 
        if(timerControlsInTrMenu)pactStopSession->setVisible(true);
        if(timerControlsInTrMenu)pactStartSession->setVisible(false);
        ptimer->start(1000);
       if(AlertBubbleStart) ptrayIcon->showMessage("TyroTimer status:",
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
        timeValue = TyroTimerDuration;
        this->ui->timeLabel->setText(this->timeValue.toString());
        slotUpdateIcon(0,timeValue.minute());
        ui->progressBar->setValue(0);
        if(isABreak)
        {
            slotUpdateIcon(0,TyroTimerDuration.minute());
            isABreak = false;
        }
    }
}
void TyroTimer::slotSetDisplay()
{
    if(keepAlertAfterBreak) pStimer->start(60000);

    if(useTaskSystem&&!isABreak)(tasks->begin()+taskComboBox->currentIndex())->timeOnTask++;
    else (tasks->begin()+taskComboBox->currentIndex())->timeOnBreak++;

    this->timeValue.setHMS(0,this->timeValue.addSecs(-1).minute(),
                           this->timeValue.addSecs(-1).second());
    this->ui->timeLabel->setText(this->timeValue.toString());
    slotUpdateIcon(0,timeValue.minute());
    this->ui->progressBar->setValue(ui->progressBar->value()+1);
    if(this->timeValue.minute() ==0 &&this->timeValue.second() ==0)
    {
        if(SessionComplete)soundComplete->play();
        if(!isABreak)
        {
            ptimer->stop();
            slotTyroTimerEnded();
            if(AlertBubbleEnd) ptrayIcon->showMessage("TyroTimer status:",
                                     "Work session completed.",
                                     QSystemTrayIcon::Information,2200);

            if(useTaskSystem)(tasks->begin()+taskComboBox->currentIndex())->sessionsOnTask++;
        }
        else
        {
            soundBreak->stop();
            ptimer->stop();
            timeValue = TyroTimerDuration;
            ui->progressBar->setMaximum(timeValue.minute()*60);
            ui->start_stop->setText("Start");

            if(timerControlsInTrMenu)pactStartSession->setVisible(true);
            if(timerControlsInTrMenu)pactStopSession->setVisible(false);

            ui->progressBar->setValue(0);
            this->ui->timeLabel->setText(this->timeValue.toString());
            slotUpdateIcon(0,timeValue.minute());
            slotUpdateIcon(1,TyroTimerDuration.minute());
            isABreak = false;

            breakHrs += breakDuration.minute()/60.0f;
            pst->setValue("breakHrs", breakHrs);
            breaks += 1;
            pst->setValue("breaks",breaks);

           if(AlertBubbleEnd) ptrayIcon->showMessage("TyroTimer status:",
                                     "Break completed.\n Get back to work!",
                                     QSystemTrayIcon::Information,2200);
           if(keepJournal)logger->write("Break of " +QString::number(breakDuration.minute()) +
                                        " minutes completed\n" );
           if(useTaskSystem)(tasks->begin()+taskComboBox->currentIndex())->breaksOnTask++;
        }
        if (PopUp)
        {
            if(Notification)soundNotification->play();
            setVisible(true);
            this->raise();
        }
    }

    slotUpdateIcon(0,timeValue.minute());
};
void TyroTimer::slotTyroTimerEnded()
{

    //play sound, increment total TyroTimers
    //need to setup a header like work and like rest

    hours += TyroTimerDuration.minute()/60.0f;
    if(keepJournal)logger->write("Work session of " +QString::number(TyroTimerDuration.minute()) +
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

void TyroTimer::slotTakeABreak()
{
    timeValue = breakDuration;
   if(AlertBubbleStart) ptrayIcon->showMessage("TyroTimer status:",
                             "Break started.",
                             QSystemTrayIcon::Information,2200);
    ui->progressBar->setMaximum(timeValue.minute()*60);
    ptimer->start(1000);
    if(Break)soundBreak->play();
    this->ui->timeLabel->setText(this->timeValue.toString());
    slotUpdateIcon(0,timeValue.minute());
    ui->progressBar->setValue(0);
    this->takeABreak->setVisible(false);
    this->skipABreak->setVisible(false);
    ui->start_stop->setText("Stop");
    ui->start_stop->setVisible(true);
    isABreak = true;
    slotUpdateIcon(1,TyroTimerDuration.minute());

    if(timerControlsInTrMenu)pactStartBreak->setVisible(false);
    if(timerControlsInTrMenu)pactStopBreak->setVisible(false);
    if(timerControlsInTrMenu)pactSkipBreak->setVisible(false);
    if(timerControlsInTrMenu)pactStopSession->setVisible(true);

}

void TyroTimer::slotSkipABreak()
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
    timeValue = TyroTimerDuration;
    ui->progressBar->setMaximum(timeValue.minute()*60);
    ui->progressBar->setValue(0);
    this->ui->timeLabel->setText(this->timeValue.toString());
    slotUpdateIcon(0,timeValue.minute());
}
void TyroTimer::slotSettings()
{
emit signalCheckSettings();
Settings->show();
Settings->raise();
}

void TyroTimer::slotReadSettings()
{
    QSettings* pst = App::theApp()->settings();
    pst->setValue("Language","en");
    if(pst->value("TyroTimerDuration").toInt()==0)
    {
        pst->setValue("TyroTimerDuration", (25*60));
    }

    if(pst->value("breakDuration").toInt()==0)
    {
        pst->setValue("breakDuration", (5*60));
    }

    TyroTimerDuration = QTime(0,0,0);
    breakDuration = QTime(0,0,0);
    TyroTimerDuration=TyroTimerDuration.addSecs(pst->value("TyroTimerDuration").toInt());
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

    keepJournal = pst->value("keepJournal").toBool();
    keepAlertAfterBreak = pst->value("keepAlertAfterBreak").toBool();
    keepWindowPos = pst->value("keepWindowPos").toBool();
    startMinimized = pst->value("startMinimized").toBool();
    timerControlsInTrMenu = pst->value("timerControlsInTrMenu").toBool();
    useTaskSystem = pst->value("useTaskSystem").toBool();

    pst->setValue("TyroTimerDuration", TyroTimerDuration.minute()*60);
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

    pst->setValue("keepJournal", keepJournal);
    pst->setValue("keepAlertAfterBreak", keepAlertAfterBreak);
    pst->setValue("keepWindowPos", keepWindowPos);
    pst->setValue("startMinimized", startMinimized);
    pst->setValue("timerControlsInTrMenu", timerControlsInTrMenu);
    pst->setValue("useTaskSystem", useTaskSystem);

    if(timerControlsInTrMenu)pactStartSession->setVisible(true);
    if(!timerControlsInTrMenu)pactStartSession->setVisible(false);
    if(!useTaskSystem) taskComboBox->setVisible(false);
    if(useTaskSystem && !tasks->empty())
    {
        taskComboBox->clear();
        taskComboBox->setVisible(true);
        for(int i = 0; i < tasks->size(); i++)
        {
            taskComboBox->addItem(tasks->at(i).taskTitle);
        }
        taskComboBox->setCurrentIndex(lastActiveTask);
    }

}

void TyroTimer::slotSaveWinPos()
{
    pst->setValue("xPos", xPos);
    pst->setValue("yPos", yPos);
     qDebug() << "xPos : " << xPos << "yPos"<< yPos;
}

void TyroTimer::moveEvent(QMoveEvent *event)
{
    qDebug() << (QString::number(x())+","+QString::number(y()));
    *lastPos = pos();
    xPos = x();
    yPos = y();
    haveLastPos = true;
    qDebug() << "Keep window pos : " << keepWindowPos;
}

void TyroTimer::slotAlertAfterBreak()
{
    if(keepAlertAfterBreak)
    {
    soundAlertAfterBreak->play();
    ptrayIcon->showMessage("TyroTimer says: ",
                           "You are slacking... GET BACK TO WORK!",
                            QSystemTrayIcon::Information,9200);
    }
}



void TyroTimer::slotReadTasks()
{
     QFile file("config/tasks");
      if (!file.open(QIODevice::ReadWrite))
      {
              QMessageBox::information(this, tr("Unable to open file"),
              file.errorString());
              return;
      }

       QDataStream in(&file);
       in.setVersion(QDataStream::Qt_5_9);
       in >> *tasks;
       qDebug() << "Inned tasks";
       qDebug() << "Tasks size now:  " <<tasks->size();
       file.close();

}

void TyroTimer::slotWriteTasks()
{
    QFile file("config/tasks");
    if (!file.open(QIODevice::WriteOnly))
    {
             QMessageBox::information(this, tr("Unable to open file"),
             file.errorString());
             return;
    }
     QDataStream out(&file);
     out.setVersion(QDataStream::Qt_5_9);
     out <<*tasks;
     qDebug() << "Outed tasks";
     qDebug() << "Tasks size now:  " << tasks->size();
     file.resize(file.pos());
     file.close();


}

void TyroTimer::slotQuit()
{
    if(ptimer->isActive())
    {
        slotStartStop();
    }
    slotSaveWinPos();
    delay();
    slotWriteTasks();
    qApp->quit();
}

void TyroTimer::delay()
{
    QTime dieTime= QTime::currentTime().addMSecs(700);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void TyroTimer::slotSaveLastActiveTask()
{
    pst->setValue("lastActiveTask", taskComboBox->currentIndex());
}
