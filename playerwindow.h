#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QMainWindow>
#include<QtSql>
#include<QSqlQuery>
#include<QtXlsx>
#include <QFileDialog>
#include<xlsxwriter.h>

namespace Ui {
class PlayerWindow;
}

class PlayerWindow : public QMainWindow
{
    Q_OBJECT

private:
    QSqlDatabase p_db;//子窗口数据库
    QSqlQueryModel *qryModel;//数据库模型
    QItemSelectionModel *theSelection;//选择模型

    void updateRecord(int recNo); //更新记录

    void getFieldNames();//获取字段名称,填充“排序字段”的comboBox

public:
    explicit PlayerWindow(QSqlDatabase& db, QWidget *parent = nullptr);
    ~PlayerWindow();

private slots:
    void on_actAddPlayer_triggered();//添加选手

    void on_actModifyPlayer_triggered();//修改选手信息

    void on_actDeletePlayer_triggered();//删除选手

    void on_tableView_doubleClicked(const QModelIndex &index);//tableView上双击,编辑当前记录

    void on_rBtnAscend_clicked();//升序

    void on_rBtnDescend_clicked();//降序

    void on_comboFilter_currentIndexChanged(int index);//选择字段进行排序

    void on_rBtnMan_clicked();//男

    void on_rBtnWoman_clicked();//女

    void on_rBtnBoth_clicked();//全显示

    void on_btnSearch_clicked();//学号搜索

    void on_actExportPlayer_triggered();//导出选手信息

private:
    Ui::PlayerWindow *ui;
};

#endif // PLAYERWINDOW_H
