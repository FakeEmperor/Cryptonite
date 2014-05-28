#include "mainwindow.h"
#include "ui_mainwindow.h"




mainWindow::mainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainWindow)
{
    ui->setupUi(this);
    ui->lblActionsCompl->setText("Actions completed: 0/0");
    ui->fileList->setColumnWidth(6, 150);
}

mainWindow::~mainWindow()
{
    delete ui;
}
Task mainWindow::EncryptedFile2Task(Encrypter::EncryptedFile& ef){
    Task res;
    res.mode = !ef.isEncrypted();
    res.path = ef.getPath();
    res.bcm = ef.getBlockType();
    res.eac = ef.getEncType();
    res.ht = ef.getHashType();

    return res;
}
void mainWindow::syncTable()
{
    int row;
    size_t i,c,u,s,r;
    QComboBox *q = NULL;
    QSignalMapper *sign;
    QWidget *w= NULL;
    selectfile *sf = NULL;
    QTableWidgetItem *it = NULL;
    Task *t = NULL;
    r = this->ui->fileList->rowCount();
    s = this->current_tasks.size();
    for(i=r; i<s; ++i){
        t = &this->current_tasks[i];
         //add row
        row = this->ui->fileList->rowCount();
        this->ui->fileList->insertRow(row);
        //add path
        it = new QTableWidgetItem(QString::fromStdString(t->path));
        this->ui->fileList->setItem(row,0,it);
        it->setFlags(it->flags() ^ Qt::ItemIsEditable);
        //for encryption, blockcipher and hashing algorithm
        for(c=1; c<=3; ++c){
            QComboBox *q = new QComboBox();
            switch(c){
            case 1:
                //add select encryption algorithm
                for(u=1; u<=Encrypter::EncryptionAlgorithm::EA_MAX; ++u)
                    q->addItem(QString::fromStdString(
                                   Encrypter::WrapEncryption::stringFromType(
                                       (Encrypter::EncryptionAlgorithm)u)
                              ));
                if(!t->mode) //check if encrypted
                    q->setCurrentIndex(t->eac-1);
                else
                    t->eac = (Encrypter::EncryptionAlgorithm)1;
                break;
            case 3:
                //add select hashing algorithm
                for(u=1; u<=Encrypter::HashingType::HT_MAX; ++u)
                    q->addItem(QString::fromStdString(
                                   Encrypter::WrapHashing::stringFromType(
                                       (Encrypter::HashingType)u)
                              ));
                if(!t->mode) //check if encrypted
                    q->setCurrentIndex(t->ht-1);
                else
                    t->ht = (Encrypter::HashingType)1;
                break;
            case 2:
                //add select bloccipher algorithm
                for(u=1; u<=BlockCipher::Mode::BC_MAX; ++u)
                    q->addItem(QString::fromStdString(
                                   Encrypter::WrapBlockCipher::stringFromType(
                                       (BlockCipher::Mode)u)
                              ));
                if(!t->mode) //check if encrypted
                    q->setCurrentIndex(t->bcm-1);
                else
                    t->bcm = (BlockCipher::Mode)1;
                break;
            }
            if(!t->mode) //disable if encrypted
                q->setDisabled(true);
            //add signal mapper for changed events
            sign = new QSignalMapper();
            connect(q,SIGNAL(currentIndexChanged(int)),sign,SLOT(map()));
            sign->setMapping(q,(QObject*)(new QPoint(row,c)));
            connect(sign, SIGNAL(mapped(QObject*)),
                         this, SLOT(on_customCellTableChanged(QObject*)));
            //finally, add to the cell
            this->ui->fileList->setCellWidget(row,c,q);
        }
        //add action combobox
        q = new QComboBox();
        q->addItem("Decrypt");
        q->addItem("Encrypt");
        q->setDisabled(true);
        //add signal mapper for changed events
        sign = new QSignalMapper();
        connect(q,SIGNAL(currentIndexChanged(int)),sign,SLOT(map()));
        sign->setMapping(q,(QObject*)(new QPoint(row,c)));
        connect(sign, SIGNAL(mapped(QObject*)),
                     this, SLOT(on_customCellTableChanged(QObject*)));
        //finally, add to the cell
        this->ui->fileList->setCellWidget(row,c,q);
        //check if encrypted
        q->setCurrentIndex(t->mode?1:0);
        //add key
        it = new QTableWidgetItem(QString::fromStdString(t->key));
        this->ui->fileList->setItem(row,c+1,it);
        //add output path
        sf = new selectfile();
        //add signal mapper for changed events
        sign = new QSignalMapper();
        connect(sf,SIGNAL(pathChanged()),sign,SLOT(map()));
        sign->setMapping(sf,(QObject*)(new QPoint(row,c+2)));
        connect(sign, SIGNAL(mapped(QObject*)),
                     this, SLOT(on_customCellTableChanged(QObject*)));
        this->ui->fileList->setCellWidget(row,c+2, sf);


    }
}



void mainWindow::syncTask(const int row, const int column){
    //get at row
    int index;
    size_t s = this->current_tasks.size();
    if(row<s){
        Task &t = this->current_tasks[row];
        switch(column){
        //0 - file path
        case 0:
            t.path = this->ui->fileList->item(row,column)->text().toStdString();
            break;
        //1 - Encryption algorithm
        case 1:
            t.eac = (Encrypter::EncryptionAlgorithm)(qobject_cast<QComboBox*>
                                                     (this->ui->fileList->cellWidget(row,column))->currentIndex()+1);
            break;
        //3 - Hashing algorithm
        case 3:
            t.ht = (Encrypter::HashingType)(qobject_cast<QComboBox*>
                                                    (this->ui->fileList->cellWidget(row,column))->currentIndex()+1);
            break;
        //2 - BlockCipher Mode
        case 2:
            t.bcm = (BlockCipher::Mode)(qobject_cast<QComboBox*>
                                                    (this->ui->fileList->cellWidget(row,column))->currentIndex()+1);
            break;
        //4 - Action
        case 4:
            index = qobject_cast<QComboBox*>
                    (this->ui->fileList->cellWidget(row,column))->currentIndex();
            t.mode = index?true:false;

            break;
        //5 - Password
        case 5:
            t.key = this->ui->fileList->item(row,column)->text().toStdString();
            break;
        //5 - Output path
        case 6:
            t.out_path = (qobject_cast<selectfile*>
                          (this->ui->fileList->cellWidget(row,column)))->getFilePath().toStdString();
            break;

        }
    }
}

void mainWindow::on_btnScanFolder_clicked()
{
//create new dialog
    QString directory = QFileDialog::getExistingDirectory(this,"Select Folder for action");
    std::vector<Encrypter::EncryptedFile> buf;
    Encrypter::EncryptedFile::scan(directory.toUtf8().constData(),buf,true);
 //parse to task
    for(Encrypter::EncryptedFile &f: buf)
        this->current_tasks.push_back(mainWindow::EncryptedFile2Task(f));
//parse to rows
    this->syncTable();
}

void mainWindow::on_btnSelectFile_clicked()
{
    size_t i,s;
    QStringList files = QFileDialog::getOpenFileNames(this,"Select File(s) for action");
    s = files.size();
    for(i=0; i<files.size(); i++)
        this->current_tasks.push_back(mainWindow::EncryptedFile2Task(Encrypter::EncryptedFile(files[i].toUtf8().constData())));
    //parse to rows
        this->syncTable();
}

void mainWindow::updateActionLabel(){
    //set label text
    QString txt = ui->lblActionsCompl->text().mid(19);
    int completed =0,
        total = this->ui->listActions->count();
    size_t i;
    encryptUI *eu;
    for(i=0; i<total; i++){
        eu = dynamic_cast<encryptUI*>(this->ui->listActions->itemWidget(
                                          this->ui->listActions->item(i)));
        if(eu&&eu->operator ->().getProgress()==100)
            completed++;
    }
    ui->lblActionsCompl->setText("Actions completed: "+QString::number(completed)+"/"+QString::number(total));
}

void mainWindow::on_btnPushAction_clicked()
{
    //create task from this columns
    //ADDING ITEMS TO THE LIST!
    QListWidgetItem *item = new QListWidgetItem(ui->listActions);
    encryptUI *wdg = new encryptUI(0,this->current_tasks);
    ui->listActions->addItem(item);
    item->setSizeHint(QSize(100,150));
    ui->listActions->setItemWidget(item, dynamic_cast<QWidget*>(wdg));
    //connect encryptUI and this window
    connect(wdg,SIGNAL(workDiiddilyDone()),this,SLOT(updateActionLabel()));
    //clear current_task
    this->current_tasks.clear();
    //clear table
    while(this->ui->fileList->rowCount())
        this->ui->fileList->removeRow(0);
    //set disabled again
    this->ui->btnPushAction->setDisabled(true);
    //update text
    this->updateActionLabel();

}
bool mainWindow::checkConditions()
{
    for(Task &t:this->current_tasks){
        //check all conditions
        //check that all algorithms are not 0
        if(t.eac==Encrypter::EncryptionAlgorithm::EA_UNKNOWN||
           t.bcm==BlockCipher::Mode::BC_UNKNOWN||
           t.ht==Encrypter::HashingType::HT_UNKNOWN)
            return false;
        //check that path, key and output are not empty strings
        if(!t.path.length()||!t.out_path.length()||!t.key.length())
            return false;
    }
    return true;
}

void mainWindow::on_fileList_cellChanged(int row, int column)
{
//synchronise task
   this->syncTask(row,column);
//check conditions
   if(this->checkConditions())
    this->ui->btnPushAction->setDisabled(false);
}
void mainWindow::on_customCellTableChanged(QObject *o){
    QPoint* t = (QPoint*)o;
    //synchronise task
    syncTask(t->x(), t->y());
    //check conditions
       if(this->checkConditions())
        this->ui->btnPushAction->setDisabled(false);
}

