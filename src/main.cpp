#include "src/wtimer.h"
#include <QApplication>
#include <QtWidgets>
#include "src/app.h"


int main(int argc, char * argv[])
{
  int result = 0;

  do
  {
     App a(argc, argv,"My company", "My App");
     WTimer wtimer;
     QApplication::setQuitOnLastWindowClosed(false);
     result = a.exec();
  } while( result == 1337 );

  return result;
}
