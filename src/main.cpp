#include "src/wtimer.h"
#include <QApplication>
#include <QtWidgets>
#include "src/app.h"


int main(int argc, char * argv[])
{
  int result = 0;

  do
  {
     App a(argc, argv,"AlexPutz", "WTimer");
     WTimer wtimer;
     QApplication::setQuitOnLastWindowClosed(false);
     QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support
     result = a.exec();
  } while( result == 10000 );

  return result;
}
