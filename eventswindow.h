#ifndef EVENTSWINDOW_H
#define EVENTSWINDOW_H

#include <QMainWindow>
#include<QtSql>
#include<QSqlQuery>
#include<QtXlsx>
#include<QMessageBox>
#include <QFileDialog>
#include <xlsxwriter.h>
#include"eventsdialog.h"

namespace Ui {
class EventsWindow;
}

class EventsWindow : public QMainWindow
{
    Q_OBJECT

private:
    QSqlDatabase e_db;//子窗口数据库
    QSqlQueryModel *qryModel;//数据库模型
    QItemSelectionModel *theSelection;//选择模型

    void updateRecord(int recNo); //更新记录

    void getFieldNames();//获取字段名称,填充“排序字段”的comboBox

public:
    explicit EventsWindow(QSqlDatabase& db, QWidget *parent = nullptr);
    ~EventsWindow();

private slots:
    void on_actAddEvent_triggered();//添加项目

    void on_actModifyEvent_triggered();//修改项目信息

    void on_tableView_doubleClicked(const QModelIndex &index);//tableView上双击,编辑当前记录

    void on_actDeleteEvent_triggered();//删除项目

    void on_rBtnAscend_clicked();//升序

    void on_rBtnDescend_clicked();//降序

    void on_comboFilter_currentIndexChanged(int index);//选择字段进行排序

    void on_rBtnPersonal_clicked();//个人赛

    void on_rBtnTeam_clicked();//团体赛

    void on_rBtnBoth_clicked();//全显示

    void on_btnSearch_clicked();//项目搜索

    void on_actExportEvent_triggered();//导出项目信息

private:
    Ui::EventsWindow *ui;
};

#endif // EVENTSWINDOW_H
