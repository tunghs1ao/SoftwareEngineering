#include "mainwindow.h"
#include <QApplication>
#include<QPixmap>
#include<QSplashScreen>
#include<QTextCodec>
#include"qdlglogin.h"

int main(int argc, char *argv[])
{
    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    QApplication a(argc,argv);
    QDlgLogin *dlgLogin=new QDlgLogin;

    QSqlDatabase db;//数据库
    db=QSqlDatabase::addDatabase("QMYSQL");//添加MYSQL数据库驱动
    db.setHostName("127.0.0.1");
    db.setDatabaseName("mydb");
    db.setUserName("tunghsiao");
    db.setPassword("12345678");
    if(!db.open()){
        qDebug() << "无法连接数据库：" << db.lastError().text();
        return -1;
    }

    if(dlgLogin->exec()==QDlgLogin::Accepted){
        MainWindow w;
        w.show();
        return a.exec();
    }
    else
        return 0;
}
