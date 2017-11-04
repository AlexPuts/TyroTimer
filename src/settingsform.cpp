#include "src/settingsform.h"
#include "ui_settingsform.h"
#include "src/app.h"
#include "src/wtimertask.h"

#include <QSettings>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QDebug>

const QString URL = "<a href = ""https://github.com/AlexPutz/WTimer"">https://github.com/AlexPutz/WTimer</a>";

settingsForm::settingsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingsForm)
{
    ui->setupUi(this);
    ui->UrlLabel->setText(URL);
    Statistics = new  statisticsDialog(0);
    Statistics->hide();
    pst = App::theApp()->settings();
    ui->sessionDurationComboBox->clear();
    ui->breakDurationComboBox->clear();
    QStringList lstDuration, lstBreak;
    lstDuration << "5 minutes"<<"10 minutes"<<
            "15 minutes"<< "20 minutes"<<
            "25 minutes"<< "30 minutes"<<
            "35 minutes"<<"40 minutes"<<
            "45 minutes"<<"50 minutes";
    lstBreak << "5 minutes"<<"10 minutes"<<
            "15 minutes"<< "20 minutes";

    connect(ui->saveSettingspushButton,SIGNAL(clicked()),this,SLOT(slotSendSettings()));
    connect(ui->cancelSettingspushButton,SIGNAL(clicked()),this,SLOT(hide()));
    connect(ui->addTaskButton,SIGNAL(clicked()), this,SLOT(slotAddTask()));
    connect(ui->deleteTaskButton,SIGNAL(clicked()), this,SLOT(slotDeleteTask()));
    connect(ui->openJournalFolder,SIGNAL(clicked()), this, SLOT(slotOpenJournalFolder()));
    connect(ui->statisticsPushButton,SIGNAL(clicked()),this,SLOT(slotShowStatistics()));
    ui->sessionDurationComboBox->addItems(lstDuration);
    ui->breakDurationComboBox->addItems(lstBreak);
    slotCheckSettings();
    journalPlText = ui->journalPlnTxt;
    tasks = new QVector <WtimerTask>;

    slotReadTasks();
    refreshTaskList();

}
void settingsForm::slotSendSettings()
{

    cDuration = (ui->sessionDurationComboBox->currentIndex()+1)*60*5;
    cBreak = (ui->breakDurationComboBox->currentIndex()+1)*60*5;
    sProcess = ui->processSoundChk->isChecked();
    sBreak = ui->breakSoundChk->isChecked();
    sSessionComplete = ui->completeSoundChk->isChecked();
    sBreakComplete = ui->bcompleteSoundChk->isChecked();
    sNotification = ui->notificationChk->isChecked();
    sPopUp = ui->popUpChk->isChecked();
    sFrameless = ui->frameLessChk->isChecked();

    sAlertBubbleStart = ui->bubbleAtStartChk->isChecked();
    sAlertBubbleEnd = ui->bubbleAtEndChk->isChecked();


    skeepJournal = ui->keepJournalChk->isChecked();
    skeepAlertAfterBreak = ui->alertAftBreakChk->isChecked();
    skeepWindowPos = ui->keepWinPosChk->isChecked();
    sstartMinimized = ui->startMinChk->isChecked();

    stimerControlsInTrMenu = ui->timerControlsInTrMenuChk->isChecked();
    suseTaskSystem = ui->useTasksChk->isChecked();

    pst->setValue("WTimerDuration", cDuration);
    pst->setValue("breakDuration", cBreak);
    pst->setValue("soundInProcess", sProcess);
    pst->setValue("soundInBreak", sBreak);
    pst->setValue("soundInComplete", sSessionComplete);
    pst->setValue("soundInBComplete", sBreakComplete);
    pst->setValue("Notification", sNotification);
    pst->setValue("PopUp", sPopUp);
    pst->setValue("Frameless", sFrameless);

    pst->setValue("AlertBubbleStart", sAlertBubbleStart);
    pst->setValue("AlertBubbleEnd", sAlertBubbleEnd);


    pst->setValue("keepJournal", skeepJournal);
    pst->setValue("keepAlertAfterBreak", skeepAlertAfterBreak);
    pst->setValue("keepWindowPos", skeepWindowPos);
    pst->setValue("startMinimized", sstartMinimized);

    pst->setValue("timerControlsInTrMenu",stimerControlsInTrMenu);

    pst->setValue("useTaskSystem",suseTaskSystem);

    //qApp->exit(10000);
    ui->labelStatus->clear();
    emit saveSettings();
    this->hide();

}
void settingsForm::slotCheckSettings()
{
    ui->processSoundChk->setCheckState(Qt::Unchecked);
    ui->breakSoundChk->setCheckState(Qt::Unchecked);
    ui->completeSoundChk->setCheckState(Qt::Unchecked);
    ui->bcompleteSoundChk->setCheckState(Qt::Unchecked);
    ui->notificationChk->setCheckState(Qt::Unchecked);

    ui->bubbleAtStartChk->setCheckState(Qt::Unchecked);
    ui->bubbleAtEndChk->setCheckState(Qt::Unchecked);

    ui->keepJournalChk->setCheckState(Qt::Unchecked);
    ui->alertAftBreakChk->setCheckState(Qt::Unchecked);
    ui->keepWinPosChk->setCheckState(Qt::Unchecked);
    ui->startMinChk->setCheckState(Qt::Unchecked);

    ui->timerControlsInTrMenuChk->setCheckState(Qt::Unchecked);
    ui->useTasksChk->setCheckState(Qt::Unchecked);



    ui->sessionDurationComboBox->setCurrentIndex((((pst->value("WTimerDuration").toInt())/60)/5)-1);
    ui->breakDurationComboBox->setCurrentIndex((((pst->value("breakDuration").toInt())/60)/5)-1);
    if((pst->value("soundInProcess").toBool())) ui->processSoundChk->setCheckState(Qt::Checked);
    if((pst->value("soundInBreak").toBool())) ui->breakSoundChk->setCheckState(Qt::Checked);
    if((pst->value("soundInComplete").toBool())) ui->completeSoundChk->setCheckState(Qt::Checked);
    if((pst->value("soundInBComplete").toBool())) ui->bcompleteSoundChk->setCheckState(Qt::Checked);
    if((pst->value("Notification").toBool())) ui->notificationChk->setCheckState(Qt::Checked);
    if((pst->value("Frameless").toBool())) ui->frameLessChk->setCheckState(Qt::Checked);
    if((pst->value("PopUp").toBool())) ui->popUpChk->setCheckState(Qt::Checked);

    if((pst->value("AlertBubbleStart").toBool())) ui->bubbleAtStartChk->setCheckState(Qt::Checked);
    if((pst->value("AlertBubbleEnd").toBool())) ui->bubbleAtEndChk->setCheckState(Qt::Checked);

    if((pst->value("keepJournal").toBool())) ui->keepJournalChk->setCheckState(Qt::Checked);
    if((pst->value("keepAlertAfterBreak").toBool())) ui->alertAftBreakChk->setCheckState(Qt::Checked);
    if((pst->value("keepWindowPos").toBool())) ui->keepWinPosChk->setCheckState(Qt::Checked);
    if((pst->value("startMinimized").toBool())) ui->startMinChk->setCheckState(Qt::Checked);
    if((pst->value("timerControlsInTrMenu").toBool())) ui->timerControlsInTrMenuChk->setCheckState(Qt::Checked);
    if((pst->value("useTaskSystem",suseTaskSystem).toBool())) ui->useTasksChk->setCheckState(Qt::Checked);
}

settingsForm::~settingsForm()
{
    delete ui;
}

void settingsForm::slotAddTask()
{
    bool found = false;
    if(ui->newTaskLedit->text().length() < 3)
    {
        ui->labelStatus->setText("The name of task have to be at least 3 characters long");
        return;
    }
    for(int i = ui->taskList->count() - 1; i >= 0; --i)
    {
        if(ui->taskList->item(i)->text() == ui->newTaskLedit->text())
        {
            found = true;
            ui->labelStatus->setText("The task has a duplicate and wasn't added");
            break;
        }
    }
    if(found == false)
    {
    ui->taskList->addItem(ui->newTaskLedit->text());
    ui->labelStatus->setText("The task have been added...");
    createTask(ui->newTaskLedit->text());
    ui->newTaskLedit->clear();
    }


}
void settingsForm::slotDeleteTask()
{
    if(ui->taskList->currentItem())
    {
     QMessageBox::StandardButton reply;
     reply = QMessageBox::question(this, "Confirm deletion", "Are you sure you want to delete "
                                   + ui->taskList->currentItem()->text() + " task?",
                                   QMessageBox::Yes|QMessageBox::No);
     if (reply == QMessageBox::Yes)
     {

       if(tasks->size()==1) ui->labelStatus->setText("Cannot delete the last task...");
       else if(tasks->at(ui->taskList->row(ui->taskList->currentItem())).taskTitle == "Default")
       {
           ui->labelStatus->setText("Cannot delete the default task...");
       }
       else
       {
       ui->taskList->takeItem(ui->taskList->row(ui->taskList->currentItem()));
       ui->labelStatus->setText("The task have been deleted...");
       deleteTask();
       slotWriteTasks();
       refreshTaskList();
       }
     }
    }
}

void settingsForm::slotOpenJournalFolder()
{
    QDir dir("journal");
    QDesktopServices::openUrl(dir.path());
}



void settingsForm::slotReadTasks()
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
       if(tasks->size() == 0)
       {
           tasks->push_back(WtimerTask("Default"));
       }
       file.close();

}

void settingsForm::slotWriteTasks()
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

void settingsForm::createTask(QString s_taskTitle)
{
    WtimerTask newtask(s_taskTitle);
    tasks->push_back(newtask);
    qDebug() << "Added new task : " << newtask.taskTitle;
    qDebug() << "Tasks total quantity : " << tasks->size();
    slotWriteTasks();
    refreshTaskList();
}


void settingsForm::deleteTask()
{
    if(tasks->size()==1) return;
    QListWidgetItem *selected = ui->taskList->selectedItems().first();
    if(selected)
    {
    int row = ui->taskList->row(selected);
    tasks->erase(tasks->begin()+row);
    qDebug() << "Deleted a task : ";
    qDebug() << "Tasks total quantity : " << tasks->size();
    slotWriteTasks();
    refreshTaskList();
    }
}
void settingsForm::refreshTaskList()
{
    ui->taskList->clear();
    if(!tasks->isEmpty())
    {
    for(int i = 0; i < tasks->size(); i++)
    {
        ui->taskList->addItem(tasks->at(i).taskTitle);
    }
    }
}
void settingsForm::slotShowStatistics()
{
    /*
    breakHrs = pst->value("breakHrs").toDouble();
    breaks = pst->value("breaks").toInt();
    hours = pst->value("hours").toDouble();
    sessions = pst->value("sessions").toInt();

*/
    /*    double breakHrs;
    int breaks;
    double hours;
    int sessions;

      explicit WtimerTask(QString s_taskTitle);
      explicit WtimerTask();
      QString taskTitle;
      QDate timeLast;
      qint32 hoursOnTask;
      qint32 hoursOnBreak;
      QTime timeOnTask;
      QTime timeOnBreak;
      qint32 sessionsOnTask;
      qint32 breaksOnTask;



*/
    //qint32 index = ui->taskList->currentRow();
    WtimerTask stats_task = (tasks->at(ui->taskList->currentRow()));
    Statistics->hours = stats_task.hoursOnTask;
    Statistics->sessions = stats_task.sessionsOnTask;
    Statistics->breaks = stats_task.breaksOnTask;
    Statistics->breakHrs = stats_task.hoursOnBreak;
    Statistics->slotCheckStatistics();
    //Statistics->hours = stats_task.hoursOnTask;

    Statistics->show();

}
