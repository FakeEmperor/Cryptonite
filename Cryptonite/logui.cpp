#include "logui.h"
#include "ui_logui.h"

logui::logui(QWidget *parent,const QStringList &lst) :
    QWidget(parent),
    ui(new Ui::logui)
{
    ui->setupUi(this);
    ui->listLog->addItems(lst);
    this->setAttribute(Qt::WA_DeleteOnClose);
}

logui::~logui()
{
    delete ui;
}

void logui::on_closeBtn_clicked()
{
   this->close();
}

