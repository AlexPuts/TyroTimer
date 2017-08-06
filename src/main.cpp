#include "src/wtimer.h"
#include <QApplication>
#include <QtWidgets>
#include "src/app.h"

int main(int argc, char *argv[])
{
    App a(argc, argv,"My company", "My App");
    WTimer pom;
    QApplication::setQuitOnLastWindowClosed(false);

    return a.exec();
}
