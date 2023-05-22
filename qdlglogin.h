#ifndef QDLGLOGIN_H
#define QDLGLOGIN_H

#include <QDialog>
#include<QSettings>
#include<QMessageBox>
#include<QByteArray>
#include<QCryptographicHash>
#include<QMouseEvent>

namespace Ui {
class QDlgLogin;
}

class QDlgLogin : public QDialog
{
    Q_OBJECT

private:
    QString myAccount="123";
    QString myPassword="abc";
    int tryCount=0;//试错次数
    bool moving=false;//表示窗口是否在鼠标操作下移动
    QPoint lastPos;//上一次的鼠标位置

    void readSetting();//从注册表读取设置
    void writeSetting();//从注册表写入设置
    QString encrypt(const QString& str);//字符串加密

protected:
    void mousePressEvent(QMouseEvent *event);//Press
    void mouseMoveEvent(QMouseEvent *event);//Move
    void mouseReleaseEvent(QMouseEvent *event);//Release

public:
    explicit QDlgLogin(QWidget *parent = 0);
    ~QDlgLogin();

private slots:
    void on_btnLogin_clicked();//Login

private:
    Ui::QDlgLogin *ui;
};

#endif // QDLGLOGIN_H
