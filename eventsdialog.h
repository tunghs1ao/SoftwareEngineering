#ifndef EVENTSDIALOG_H
#define EVENTSDIALOG_H

#include <QDialog>
#include<QSqlRecord>
#include<QDebug>

namespace Ui {
class EventsDialog;
}

class EventsDialog : public QDialog
{
    Q_OBJECT

private:
    QSqlRecord mRecord; //保存一条记录的数据

public:
    explicit EventsDialog(QWidget *parent = 0);
    ~EventsDialog();

    void setUpdateRecord(QSqlRecord &recData); //更新记录
    void setAddRecord(QSqlRecord &recData); //添加记录
    QSqlRecord getRecordData();//获取录入的数据

private:
    Ui::EventsDialog *ui;
};

#endif // EVENTSDIALOG_H
