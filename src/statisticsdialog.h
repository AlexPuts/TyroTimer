#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include "src/app.h"
#include "src/tyrotimertask.h"

namespace Ui {
class statisticsDialog;
}

class statisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit statisticsDialog(QWidget *parent = 0);
    QSettings* pst;

    qint32 breakHrs;
    qint32 breaks;
    qint32 hours;
    qint32 sessions;

    QString taskTitle;
    QDate timeLast;
    qint64 timeOnTask;
    qint64 timeOnBreak;

    ~statisticsDialog();

private:
    Ui::statisticsDialog *ui;

public slots:
    void slotCheckStatistics();
};

#endif // STATISTICSDIALOG_H
