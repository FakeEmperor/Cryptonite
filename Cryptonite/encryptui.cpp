#include "encryptui.h"
#include "logui.h"
#include "ui_encryptui.h"


void WatcherWorker::doWork(Worker *w){
    using namespace Multithreading;
    DWORD res;
    bool needed = true;
    HANDLE h;
    size_t code,type;
    if(!w){
        QMessageBox::critical(0,"System Critical Error",
                              "System could not start a watch thread for the encryption thread. "
                              "Application doesn't know about the work result.",
                              QMessageBox::StandardButton::Ok
                              );
        return;
    }
    const Event &e = w->getEvent();
    h = e.getHandle();
    do{
        res = WaitForSingleObject(h,100);
        //if waited successfully
        if(res==WAIT_ABANDONED||res == WAIT_OBJECT_0){
            code = e.getCode();
            type = e.getType();
            if(type==EV_TRACING){
                if(code!=3)
                    emit this->updateParams();
                else
                    needed = false;
            }
        }
    }while(needed);
    emit this->workDone();

    return;
}

encryptUI::encryptUI(QWidget *parent, const std::vector<Task> &tasks) :
    QWidget(parent),
    ui(new Ui::encryptUI),
    worker(tasks)
{
    ui->setupUi(this);
    //enable start button
    ui->btnStart->setDisabled(false);
    ui->btnLog->setDisabled(false);
    //change texts
    ui->lblCurrent->setText( "Idle" );
    ui->lblSpeed->setText( "Speed: none");
}

encryptUI::~encryptUI()
{
    delete ui;
    watcher_thread.quit();
    watcher_thread.terminate(); //kill
}
Worker& encryptUI::operator -> (){
    return this->worker;
}

void encryptUI::updateSpeedNProgress(){
    size_t speed_bytes;
    long double act_speed;
    QString scale = "B";
    //set progress
    this->ui->progressTotal->setValue(
                this->worker.getProgress()
                );
    speed_bytes = this->worker.getSpeed();
    //set speed
    if(speed_bytes>=1048576) { //Megabyte
        act_speed = speed_bytes/1048576;
        scale = "Mb";
    }
    else if(speed_bytes>=1024) { //Kilobyte
        act_speed = speed_bytes/1024;
        scale = "Kb";
    }
    else
        act_speed = speed_bytes;
    this->ui->lblSpeed->setText("Speed: "+QString::number(act_speed,'f',1)+" "+scale+"/sec");

}

void encryptUI::updateWork(){

    size_t type = this->worker.getEvent().getCode();
    switch(type){
        case 1: //update speed and progres
            this->updateSpeedNProgress();
            break;
        case 2: //update work target
            this->ui->lblCurrent->setText(
                    "Current: "+QString::fromStdString(this->worker.getEvent().getStr()));
            break;
   }
}


void encryptUI::on_btnCancel_clicked()
{
    this->worker.stopWork();
    this->ui->btnCancel->setDisabled(true);
}
void disableRootParent(QWidget *obj, const bool dis){
    QWidget *temp,*par;
    if(!obj)
        return;
    temp = par = obj->parentWidget();
    while(temp){
        temp = par->parentWidget();
        if(temp)
            par=temp;
    }
    if(par) //disable parent
        par->setDisabled(dis);
}


void encryptUI::on_btnLog_clicked()
{
    std::vector<std::string> vec = this->worker.getLog();
    QStringList lst;
    for(std::string &s:vec)
        lst.append(QString::fromStdString(s));
    disableRootParent(this, true);
    logui *lg = new logui(0,lst);
    connect(lg,SIGNAL(destroyed()),this,SLOT(on_logui_destroyed()));
    this->ui->btnLog->setDisabled(true);
    lg->show();
}


void encryptUI::workDone(){
    this->ui->btnCancel->setDisabled(true);
    this->updateSpeedNProgress();
    this->ui->lblCurrent->setText("Completed all tasks.");
    //for mainWindow
    emit workDiiddilyDone();
    //if work was not completed - leave work
    if(this->ui->progressTotal->value()<100)
        this->ui->btnStart->setDisabled(false);
}

void encryptUI::on_btnStart_clicked()
{
    this->ui->btnStart->setDisabled(true);
    this->ui->btnCancel->setDisabled(false);

    //initialize watcher
    WatcherWorker *watcher = new WatcherWorker;
    watcher->moveToThread(&this->watcher_thread);
    connect(&this->watcher_thread, SIGNAL(finished()), watcher, SLOT(deleteLater()));
    connect(this, SIGNAL(watcherStart(Worker*)), watcher, SLOT(doWork(Worker*)));
    connect(watcher, SIGNAL(workDone()), this, SLOT(workDone()));
    connect(watcher,SIGNAL(updateParams()),this, SLOT(updateWork()));
    watcher_thread.start();
    //start work
    this->worker.startWork();
    //start watcher
    emit watcherStart(&this->worker);
}

void encryptUI::on_logui_destroyed(){
    disableRootParent(this, false);
    this->ui->btnLog->setDisabled(false);
}
