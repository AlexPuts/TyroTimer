#include "src/tyrotimer.h"
#include <QApplication>
#include <QtWidgets>
#include "src/app.h"


int main(int argc, char * argv[])
{
  int result = 0;

  do
  {
     App a(argc, argv,"AlexPutz", "TyroTimer");
     TyroTimer tyrotimer;
     QApplication::setQuitOnLastWindowClosed(false);
     QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support
     result = a.exec();
  } while( result == 10000 );

  return result;
}
