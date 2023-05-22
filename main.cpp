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
    if(dlgLogin->exec()==QDlgLogin::Accepted){
        MainWindow w;
        w.show();
        return a.exec();
    }
    else
        return 0;
}
