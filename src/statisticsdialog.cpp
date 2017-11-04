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
    ui->breaksHrsLbl->setText("Break hours total:  " + QString::number(breakHrs));
    ui->breaksLbl->setText("Breaks total:  " + QString::number(breaks));

    ui->hoursLbl->setText("Working hours total:  " + QString::number(hours));
    ui->sessionsLbl->setText("Sessions total:  " + QString::number(sessions));
}
