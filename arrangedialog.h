#ifndef ARRANGEDIALOG_H
#define ARRANGEDIALOG_H

#include <QDialog>
#include<QSqlRecord>
#include<QDebug>

namespace Ui {
class ArrangeDialog;
}

class ArrangeDialog : public QDialog
{
    Q_OBJECT

private:
    QSqlRecord mRecord; //保存一条记录的数据

public:
    explicit ArrangeDialog(QWidget *parent = 0);
    ~ArrangeDialog();

    void setUpdateRecord(QSqlRecord &recData); //更新记录
    void setAddRecord(QSqlRecord &recData); //添加记录
    QSqlRecord getRecordData();//获取录入的数据

private:
    Ui::ArrangeDialog *ui;
};

#endif // ARRANGEDIALOG_H
