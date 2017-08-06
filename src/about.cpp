#include "src/about.h"
#include "ui_about.h"
const QString URL = "<a href = ""https://github.com/AlexPutz/WTimer"">https://github.com/AlexPutz/WTimer</a>";
About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    connect(ui->OKpushButton,SIGNAL(clicked()), this, SLOT(hide()));
    ui->label_2->setText(URL);
}

About::~About()
{
    delete ui;
}
