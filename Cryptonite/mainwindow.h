#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once
#include <QMainWindow>
#include <QFileDialog>
#include <QSignalMapper>
#include <QComboBox>
#include <QMessageBox>
#include <QThread>

//for Encryption
#include "fileio.h"
#include "worker.h"
#include "utils.h"
#include "encrypter.h"
//for Encryption GUI
#include "encryptui.h"
#include "selectfile.h"
namespace Ui {
class mainWindow;
}

//update label text
/* QString txt = mw->ui->lblActionsCompl->text().mid(19);
int completed = txt.mid(0,txt.indexOf("/")).toInt()+1,
    total = mw->ui->listActions->count();
mw->ui->lblActionsCompl->setText(
            "Actions completed: "+
            QString::number(completed)+"/"+
            QString::number(total));
//remove that ui from the list*/
class mainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainWindow(QWidget *parent = 0);
    ~mainWindow();

private slots:
    void on_btnScanFolder_clicked();

    void on_customCellTableChanged(QObject*);

    void on_btnSelectFile_clicked();

    void on_btnPushAction_clicked();

    void on_fileList_cellChanged(int row, int column);

    void updateActionLabel();
public slots:
    void on_fast_encrypt_open();

private:
    Ui::mainWindow *ui;
    std::vector<Task> current_tasks;
    friend Multithreading::ThreadResult __stdcall watchFunction(Multithreading::Thread *thr, void *mainWindowPtr);
    static Task EncryptedFile2Task(Encrypter::EncryptedFile& ef);
    void syncTable();
    void syncTask(const int row, const int column);
    bool checkConditions();
};

#endif // MAINWINDOW_H
