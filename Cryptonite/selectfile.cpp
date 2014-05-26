#include "selectfile.h"
#include "ui_selectfile.h"

selectfile::selectfile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::selectfile)
{
    ui->setupUi(this);
}

selectfile::~selectfile()
{
    delete ui;
}
QString selectfile::getFilePath(){
    return this->filePath;
}

void selectfile::on_btnSelect_clicked()
{
    QString selFilter = "All files (*.*)";
    this->filePath = QFileDialog::getSaveFileName(this,"Set output file path",
                                                  QString(),
                                                  selFilter, &selFilter);
    this->ui->edtPath->setText(this->filePath);
    emit this->pathChanged();
}
