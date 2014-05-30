#ifndef FASTENCRYPTUI_H
#define FASTENCRYPTUI_H

#include <QWidget>

namespace Ui {
class FastEncryptUI;
}

class FastEncryptUI : public QWidget
{
    Q_OBJECT

public:
    explicit FastEncryptUI(QWidget *parent = 0);
    ~FastEncryptUI();

private slots:
    void on_btnGo_clicked();

private:
    Ui::FastEncryptUI *ui;
};

#endif // FASTENCRYPTUI_H
