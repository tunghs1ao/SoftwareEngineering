#ifndef JUDGESWINDOW_H
#define JUDGESWINDOW_H

#include <QMainWindow>
#include<QtSql>
#include<QSqlQuery>
#include<QtXlsx>
#include<QMessageBox>
#include"judgesdialog.h"

namespace Ui {
class JudgesWindow;
}

class JudgesWindow : public QMainWindow
{
    Q_OBJECT

private:
    QSqlDatabase j_db;//子窗口数据库
    QSqlQueryModel *qryModel;//数据库模型
    QItemSelectionModel *theSelection;//选择模型

    void updateRecord(int recNo); //更新记录

    void getFieldNames();//获取字段名称,填充“排序字段”的comboBox

public:
    explicit JudgesWindow(QSqlDatabase& db, QWidget *parent = nullptr);
    ~JudgesWindow();

private slots:
    void on_actAddJudge_triggered();//添加裁判

    void on_actModifyJudge_triggered();//修改裁判

    void on_tableView_doubleClicked(const QModelIndex &index);//tableView上双击,编辑当前记录

    void on_actDeleteJudge_triggered();//删除裁判

    void on_rBtnAscend_2_clicked();//升序

    void on_rBtnDescend_2_clicked();//降序

    void on_comboFilter_2_currentIndexChanged(int index);//选择字段进行排序

    void on_rBtnMan_2_clicked();//男

    void on_rBtnWoman_2_clicked();//女

    void on_rBtnBoth_2_clicked();//全显示

    void on_btnSearch_2_clicked();//裁判搜索

private:
    Ui::JudgesWindow *ui;
};

#endif // JUDGESWINDOW_H
