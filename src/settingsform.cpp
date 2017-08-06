#include "src/settingsform.h"
#include "ui_settingsform.h"
#include "src/app.h"
#include <QSettings>

settingsForm::settingsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingsForm)
{
    ui->setupUi(this);

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

    ui->sessionDurationComboBox->addItems(lstDuration);
    ui->breakDurationComboBox->addItems(lstBreak);
    slotCheckSettings();


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


    pst->setValue("WTimerDuration", cDuration);
    pst->setValue("breakDuration", cBreak);
    pst->setValue("soundInProcess", sProcess);
    pst->setValue("soundInBreak", sBreak);
    pst->setValue("soundInComplete", sSessionComplete);
    pst->setValue("soundInBComplete", sBreakComplete);
    pst->setValue("Notification", sNotification);
    pst->setValue("PopUp", sPopUp);
    pst->setValue("Frameless", sFrameless);


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

    ui->sessionDurationComboBox->setCurrentIndex((((pst->value("WTimerDuration").toInt())/60)/5)-1);
    ui->breakDurationComboBox->setCurrentIndex((((pst->value("breakDuration").toInt())/60)/5)-1);
    if((pst->value("soundInProcess").toBool())) ui->processSoundChk->setCheckState(Qt::Checked);
    if((pst->value("soundInBreak").toBool())) ui->breakSoundChk->setCheckState(Qt::Checked);
    if((pst->value("soundInComplete").toBool())) ui->completeSoundChk->setCheckState(Qt::Checked);
    if((pst->value("soundInBComplete").toBool())) ui->bcompleteSoundChk->setCheckState(Qt::Checked);
    if((pst->value("Notification").toBool())) ui->notificationChk->setCheckState(Qt::Checked);

    if((pst->value("Frameless").toBool())) ui->frameLessChk->setCheckState(Qt::Checked);
    if((pst->value("PopUp").toBool())) ui->popUpChk->setCheckState(Qt::Checked);
}

settingsForm::~settingsForm()
{
    delete ui;
}