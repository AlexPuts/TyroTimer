#ifndef APP_H
#define APP_H
#include <QApplication>
#include <QSettings>

class App : public QApplication
{
Q_OBJECT
private:
    QSettings* pSettings;
public:
    App(int& argc,
        char** argv,
        const QString& strOrg,
        const QString& strAppName):
        QApplication(argc,argv),
        pSettings(0)
    {
        setOrganizationName(strOrg);
        setApplicationName(strAppName);
        pSettings = new QSettings(strOrg, strAppName,this);
    }
    static App* theApp()
    {
        return (App*)qApp;
    }
    QSettings* settings()
    {
        return pSettings;
    }
};

#endif // APP_H
