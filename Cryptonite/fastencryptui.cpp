#include "fastencryptui.h"
#include "selectfile.h"
#include "ui_fastencryptui.h"

FastEncryptUI::FastEncryptUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FastEncryptUI)
{
    ui->setupUi(this);
}

FastEncryptUI::~FastEncryptUI()
{
    delete ui;
}

void FastEncryptUI::on_btnGo_clicked()
{

}
