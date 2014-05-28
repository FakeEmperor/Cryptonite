#ifndef LOGUI_H
#define LOGUI_H

#include <QWidget>
#include <QStringList>

namespace Ui {
class logui;
}

class logui : public QWidget
{
    Q_OBJECT

public:
    explicit logui(QWidget *parent = 0,const QStringList &lst = QStringList());
    ~logui();

private slots:
    void on_closeBtn_clicked();


private:
    Ui::logui *ui;
};

#endif // LOGUI_H
