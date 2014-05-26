#include "logui.h"
#include "ui_logui.h"

logui::logui(QWidget *parent,const QStringList &lst) :
    QWidget(parent),
    ui(new Ui::logui)
{
    ui->setupUi(this);
    ui->listLog->addItems(lst);
}

logui::~logui()
{
    delete ui;
}

void logui::on_closeBtn_clicked()
{
   this->destroy();
   emit this->destroyed();
}
