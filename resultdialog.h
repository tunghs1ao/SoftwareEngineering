#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>
#include<QSqlRecord>
#include<QDebug>

namespace Ui {
class ResultDialog;
}

class ResultDialog : public QDialog
{
    Q_OBJECT

private:
    QSqlRecord mRecord; //保存一条记录的数据

public:
    explicit ResultDialog(QWidget *parent = 0);
    ~ResultDialog();

    void setUpdateRecord(QSqlRecord &recData); //更新记录
    QSqlRecord getRecordData();//获取录入的数据

private:
    Ui::ResultDialog *ui;
};

#endif // RESULTDIALOG_H
