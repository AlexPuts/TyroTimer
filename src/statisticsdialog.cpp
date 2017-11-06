#include "src/statisticsdialog.h"
#include "ui_statisticsdialog.h"
#include "src/app.h"
#include <QVariant>


statisticsDialog::statisticsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::statisticsDialog)
{
    ui->setupUi(this);
    pst = App::theApp()->settings();
    connect(ui->OKpushButton,SIGNAL(clicked()),this,SLOT(hide()));
    slotCheckStatistics();
}

statisticsDialog::~statisticsDialog()
{
    delete ui;
}
void statisticsDialog::slotCheckStatistics()
{
    ui->taskTitlelbl->setText("Task title: " + taskTitle);
    ui->timeLastlbl->setText("Task last accessed at: " +timeLast.toString("dd.MM.yyyy"));
    ui->timeOnTasklbl->setText("Time spent on this task: " +
                               QString::number(timeOnTask/3600) + ':'+ QString::number((timeOnTask%3600)/60).rightJustified(2, '0') + ':' +
                               QString::number(timeOnTask%60).rightJustified(2, '0'));
    ui->timeOnBreaklbl->setText("Time spent on breaks: " + QString::number(timeOnBreak/3600) + ':'
                                + QString::number((timeOnBreak%3600)/60).rightJustified(2, '0') + ':' +
                                QString::number(timeOnBreak%60).rightJustified(2, '0'));
    ui->breaksHrsLbl->setText("Break hours total:  " + QString::number(breakHrs));
    ui->breaksLbl->setText("Breaks total:  " + QString::number(breaks));
    ui->hoursLbl->setText("Working hours total:  " + QString::number(hours));
    ui->sessionsLbl->setText("Sessions total:  " + QString::number(sessions));



}
