#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include "src/app.h"
#include "src/wtimertask.h"

namespace Ui {
class statisticsDialog;
}

class statisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit statisticsDialog(QWidget *parent = 0);
    QSettings* pst;

    double breakHrs;
    int breaks;
    double hours;
    int sessions;


    ~statisticsDialog();

private:
    Ui::statisticsDialog *ui;

public slots:
    void slotCheckStatistics();
};

#endif // STATISTICSDIALOG_H
