#ifndef WTIMERTASK_H
#define WTIMERTASK_H

#include <QObject>
#include <QDate>
#include <QDataStream>

class TyroTimerTask
{

public:
      explicit TyroTimerTask(QString s_taskTitle);
      explicit TyroTimerTask();
      QString taskTitle;
      QDate timeLast;
      qint32 hoursOnTask;
      qint32 hoursOnBreak;
      qint32 timeOnTask;
      qint32 timeOnBreak;
      qint32 sessionsOnTask;
      qint32 breaksOnTask;


};

inline QDataStream& operator<<( QDataStream &out, const TyroTimerTask& t )
{
    out<<t.taskTitle;
    out<<t.timeLast;
    out<<t.hoursOnTask;
    out<<t.hoursOnBreak;
    out<<t.timeOnTask;
    out<<t.timeOnBreak;
    out<<t.sessionsOnTask;
    out<<t.breaksOnTask;

    return out;
}

inline QDataStream& operator>>( QDataStream &in, TyroTimerTask& t)
{
    in>>t.taskTitle;
    in>>t.timeLast;
    in>>t.hoursOnTask;
    in>>t.hoursOnBreak;
    in>>t.timeOnTask;
    in>>t.timeOnBreak;
    in>>t.sessionsOnTask;
    in>>t.breaksOnTask;


    return in;
}

#endif // WTIMERTASK_H
