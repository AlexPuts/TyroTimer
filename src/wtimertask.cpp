#include "wtimertask.h"

WtimerTask::WtimerTask(QString s_taskTitle)
{

    taskTitle = s_taskTitle;
    timeLast = timeLast.currentDate();
    timeOnTask = QTime(0,0,0);
    timeOnBreak = QTime(0,0,0);
    hoursOnTask = 0;
    hoursOnBreak = 0;
    sessionsOnTask = 0;
    breaksOnTask = 0;



}

WtimerTask::WtimerTask()
{

    taskTitle = "Default";
    timeLast = timeLast.currentDate();
    timeOnTask = QTime(0,0,0);
    timeOnBreak = QTime(0,0,0);
    hoursOnTask = 0;
    hoursOnBreak = 0;
    sessionsOnTask = 0;
    breaksOnTask = 0;
}
