#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QtSql>
#include<QSqlQuery>
#include<QFileDialog>
#include<QMessageBox>
#include<QDebug>
#include<QSqlError>
#include<QLabel>
#include<QFont>
#include<QString>
#include<QCloseEvent>
//#include <QAxObject>//Windows平台使用
#include<QtXlsx>
#include"playerwindow.h"
#include"eventswindow.h"
#include"judgeswindow.h"
#include"arrangewindow.h"
#include"resultwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QLabel *LabInfo;//statusBar信息
    QSqlDatabase db;//数据库

    void closeEvent(QCloseEvent *event);//退出确认

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
//    void on_actConnect_triggered();//连接数据库

    void on_actApply_triggered();//报名

    void on_actWithdraw_triggered();//退赛

    void on_actPlayerManage_triggered();//选手管理窗口

    void on_tabWidget_tabCloseRequested(int index);//关闭Tab

    void on_actEventsManage_triggered();//项目管理窗口

    void on_actJudgesManage_triggered();//裁判管理窗口

    void on_actArrangeManage_triggered();//赛程管理窗口

    void on_actResultManage_triggered();//成绩管理窗口

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
