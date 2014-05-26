#ifndef ENCRYPTUI_H
#define ENCRYPTUI_H
#include <QWidget>
#include "worker.h"
#include <QThread>
#include <QMessageBox>
namespace Ui{
class encryptUI;
}
/**
 * @brief The WatcherWorker class
 * Seriously, last try. I spent 6 hours figuring out wtf is happening.
 * Dirty workaraound. Just hope it'll work.
 */
class WatcherWorker:public QObject{
    Q_OBJECT
         QThread workerThread;

     public slots:
         void doWork(Worker *w);

     signals:
         void workDone();
         void updateParams();
};
class encryptUI: public QWidget
{
    Q_OBJECT
    Worker worker;
    QThread watcher_thread;

public:
    explicit encryptUI(QWidget *parent = 0, const std::vector<Task> &tasks = std::vector<Task>());
    ~encryptUI();
    Worker& operator -> ();

public slots:
    void updateWork();
    void workDone();
private slots:
    void on_btnCancel_clicked();

    void on_btnLog_clicked();

    void on_btnStart_clicked();

    void on_logui_destroyed();
signals:
    void watcherStart(Worker*);
    void workDiiddilyDone();
private:
    Ui::encryptUI *ui;

    void updateSpeedNProgress();
};

#endif // ENCRYPTUI_H
