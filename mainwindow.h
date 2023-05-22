#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QtSql>
#include<QFileDialog>
#include<QMessageBox>
#include<QDebug>
#include<QSqlError>
#include<QLabel>
#include<QFont>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QLabel *LabInfo;//statusBar信息
    QSqlDatabase db;//数据库
    QSqlQueryModel *qryModel;//数据库模型
    QItemSelectionModel *theSelection;//选择模型

    void openTable();//打开表

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actConnect_triggered();//连接数据库

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
