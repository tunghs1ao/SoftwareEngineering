#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

#include <QMainWindow>
#include<QtSql>
#include<QSqlQuery>
#include<QMessageBox>
#include <QFileDialog>
#include <xlsxwriter.h>
#include"resultdialog.h"

namespace Ui {
class ResultWindow;
}

class ResultWindow : public QMainWindow
{
    Q_OBJECT

private:
    QSqlDatabase r_db;//子窗口数据库
    QSqlQueryModel *qryModel;//数据库模型
    QItemSelectionModel *theSelection;//选择模型

    void updateRecord(int recNo); //更新记录

    void getComboStatus();//获取比赛状态,填充comboStatus

    void getComboEventsID();//获取项目编号,填充two comboEventsID

public:
    explicit ResultWindow(QSqlDatabase& db, QWidget *parent = nullptr);
    ~ResultWindow();

private slots:
    void on_actEditResult_triggered();//编辑成绩

    void on_tableView_doubleClicked(const QModelIndex &index);//tableView上双击,编辑当前记录

    void on_actCheckNewRecord_triggered();//确认破纪录

    void on_rBtnP_clicked();//初赛P

    void on_rBtnR_clicked();//复赛R

    void on_rBtnF_clicked();//决赛F

    void on_btnSearch_clicked();//项目搜索

    void on_BtnRank_clicked();//确认排名

    void on_comboEventsID_currentIndexChanged(int index);//检索项目

    void on_rBtnAll_clicked();//所有赛程

    void on_actExportResult_triggered();//导出成绩信息

    void on_BtnR_clicked();//发布复赛

    void on_BtnF_clicked();//发布决赛

    void on_actDeleteResult_triggered();//删除成绩

private:
    Ui::ResultWindow *ui;
};

#endif // RESULTWINDOW_H
