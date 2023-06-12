#ifndef PLAYERDIALOG_H
#define PLAYERDIALOG_H

#include <QDialog>
#include<QSqlRecord>
#include<QDebug>

namespace Ui {
class PlayerDialog;
}

class PlayerDialog : public QDialog
{
    Q_OBJECT

private:
    QSqlRecord mRecord; //保存一条记录的数据

public:
    explicit PlayerDialog(QWidget *parent = 0);
    ~PlayerDialog();

    void setUpdateRecord(QSqlRecord &recData); //更新记录
    void setAddRecord(QSqlRecord &recData); //添加记录
    QSqlRecord getRecordData();//获取录入的数据

private:
    Ui::PlayerDialog *ui;
};

#endif // PLAYERDIALOG_H
