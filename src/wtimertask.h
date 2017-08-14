#ifndef WTIMERTASK_H
#define WTIMERTASK_H

#include <QObject>
#include <QDate>

class WtimerTask
{

public:
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


};

inline QDataStream& operator<<( QDataStream &out, const WtimerTask& t )
{
    out<<t.taskTitle;
    out<<t.timeLast;
    out<<QString::number(t.hoursOnTask);
    out<<QString::number(t.hoursOnBreak);
    out<<t.timeOnTask;
    out<<t.timeOnBreak;
    out<<t.sessionsOnTask;
    out<<t.breaksOnTask;

    return out;
}

inline QDataStream& operator>>( QDataStream &in, WtimerTask& t)
{
    in>>t.taskTitle;
    in>>t.timeLast;

    QString hoursOnTask, hoursOnBreak, sessionsOnTask, breaksOnTask;
    in>>hoursOnTask;
    t.hoursOnTask = hoursOnTask.toInt();
    in>>hoursOnBreak;
    t.hoursOnBreak = hoursOnBreak.toInt();
    in>>t.timeOnTask;
    in>>t.timeOnBreak;
    in>>sessionsOnTask;
    t.sessionsOnTask = sessionsOnTask.toInt();
    in>>breaksOnTask;
    t.breaksOnTask = breaksOnTask.toInt();

    return in;
}

#endif // WTIMERTASK_H
