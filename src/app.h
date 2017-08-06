#ifndef APP_H
#define APP_H
#include <QApplication>
#include <QSettings>

class App : public QApplication
{
Q_OBJECT
private:
    QSettings* m_pSettings;
public:
    App(int& argc,
        char** argv,
        const QString& strOrg,
        const QString& strAppName):
        QApplication(argc,argv),
        m_pSettings(0)
    {
        setOrganizationName(strOrg);
        setApplicationName(strAppName);
        m_pSettings = new QSettings(strOrg, strAppName,this);
    }
    static App* theApp()
    {
        return (App*)qApp;
    }
    QSettings* settings()
    {
        return m_pSettings;
    }
};

#endif // APP_H
