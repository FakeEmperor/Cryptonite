#include "logui.h"
#include "ui_logui.h"

logui::logui(QWidget *parent,const QStringList &lst) :
    QWidget(parent, Qt::FramelessWindowHint),
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
void logui::mousePressEvent(QMouseEvent *evt)
{
    oldPos = evt->globalPos();
}

void logui::mouseMoveEvent(QMouseEvent *evt)
{
   const QPoint delta = evt->globalPos() - oldPos;
   move(x()+delta.x(), y()+delta.y());
   oldPos = evt->globalPos();
}
