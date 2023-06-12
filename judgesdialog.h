#ifndef JUDGESDIALOG_H
#define JUDGESDIALOG_H

#include <QDialog>
#include<QSqlRecord>
#include<QDebug>

namespace Ui {
class JudgesDialog;
}

class JudgesDialog : public QDialog
{
    Q_OBJECT

private:
    QSqlRecord mRecord; //保存一条记录的数据

public:
    explicit JudgesDialog(QWidget *parent = 0);
    ~JudgesDialog();

    void setUpdateRecord(QSqlRecord &recData); //更新记录
    void setAddRecord(QSqlRecord &recData); //添加记录
    QSqlRecord getRecordData();//获取录入的数据

private:
    Ui::JudgesDialog *ui;
};

#endif // JUDGESDIALOG_H
