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
    if(pst->value("breakHrs").toDouble()==0)
    {
        pst->setValue("breakHrs", 0.00);
    }
    if(pst->value("breaks").toInt()==0)
    {
        pst->setValue("breaks", 0);
    }
    if(pst->value("hours").toDouble()==0)
    {
        pst->setValue("hours", 0.00);
    }
    if(pst->value("sessions").toInt()==0)
    {
        pst->setValue("sessions", 0);
    }
}

statisticsDialog::~statisticsDialog()
{
    delete ui;
}
void statisticsDialog::slotCheckStatistics()
{
    breakHrs = pst->value("breakHrs").toDouble();
    breaks = pst->value("breaks").toInt();
    hours = pst->value("hours").toDouble();
    sessions = pst->value("sessions").toInt();

    ui->breaksHrsLbl->setText("Break hours total:  " + QString::number(breakHrs));
    ui->breaksLbl->setText("Breaks total:  " + QString::number(breaks));

    ui->hoursLbl->setText("Working hours total:  " + QString::number(hours));
    ui->sessionsLbl->setText("Sessions total:  " + QString::number(sessions));
}
