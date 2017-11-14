#include "tyrotimertask.h"

TyroTimerTask::TyroTimerTask(QString s_taskTitle)
{

    taskTitle = s_taskTitle;
    timeLast = timeLast.currentDate();
    timeOnTask = 0;
    timeOnBreak = 0;
    hoursOnTask = 0;
    hoursOnBreak = 0;
    sessionsOnTask = 0;
    breaksOnTask = 0;



}

TyroTimerTask::TyroTimerTask()
{

    taskTitle = "Default";
    timeLast = timeLast.currentDate();
    timeOnTask = 0;
    timeOnBreak = 0;
    hoursOnTask = 0;
    hoursOnBreak = 0;
    sessionsOnTask = 0;
    breaksOnTask = 0;
}
