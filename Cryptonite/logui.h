#ifndef LOGUI_H
#define LOGUI_H

#include <QWidget>
#include <QStringList>
#include <QMouseEvent>

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

protected:
    void mouseMoveEvent(QMouseEvent *evt);
    void mousePressEvent(QMouseEvent *evt);
private:
    Ui::logui *ui;
    QPoint oldPos;
};

#endif // LOGUI_H
