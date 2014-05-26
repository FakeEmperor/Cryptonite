#ifndef SELECTFILE_H
#define SELECTFILE_H
#include <QWidget>
#include <QString>
#include <QFileDialog>
namespace Ui{
class selectfile;
}

class selectfile: public QWidget
{
    Q_OBJECT

public:
    explicit selectfile(QWidget *parent = 0);
    ~selectfile();
    QString getFilePath();
signals:
    void pathChanged();
private slots:
    void on_btnSelect_clicked();

private:
    Ui::selectfile *ui;
    QString filePath;
};

#endif // SELECTFILE_H
