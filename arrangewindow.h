#ifndef ARRANGEWINDOW_H
#define ARRANGEWINDOW_H

#include <QMainWindow>
#include<QtSql>
#include<QSqlQuery>
#include<QMessageBox>
#include <QFileDialog>
#include <xlsxwriter.h>
#include"arrangedialog.h"

namespace Ui {
class ArrangeWindow;
}

class ArrangeWindow : public QMainWindow
{
    Q_OBJECT

private:
    QSqlDatabase a_db;//子窗口数据库
    QSqlQueryModel *qryModel;//数据库模型
    QItemSelectionModel *theSelection;//选择模型

    void updateRecord(int recNo); //更新记录

    void getFieldNames();//获取字段名称,填充“排序字段”的comboBox

public:
    explicit ArrangeWindow(QSqlDatabase& db, QWidget *parent = nullptr);
    ~ArrangeWindow();

private slots:
    void on_actAddArrange_triggered();//添加赛程

    void on_actModifyArrange_triggered();//修改赛程

    void on_tableView_doubleClicked(const QModelIndex &index);//tableView上双击,编辑当前记录

    void on_actDeleteArrange_triggered();//删除赛程

    void on_rBtnAscend_clicked();//升序

    void on_rBtnDescend_clicked();//降序

    void on_comboFilter_currentIndexChanged(int index);//选择字段进行排序

    void on_rBtnP_clicked();//初赛P

    void on_rBtnR_clicked();//复赛R

    void on_rBtnF_clicked();//决赛

    void on_rBtnBoth_clicked();//全显示

    void on_btnSearch_clicked();//项目搜索

    void on_actExportArrange_triggered();//导出赛程信息

private:
    Ui::ArrangeWindow *ui;
};

#endif // ARRANGEWINDOW_H
