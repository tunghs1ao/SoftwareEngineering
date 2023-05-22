#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("运动会管理系统");
    this->resize(1200,800);
//    this->setWindowState(Qt::WindowMaximized);//窗口最大化
    this->setCentralWidget(ui->tableView);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//指定用户在表格中选择时应该选择整行
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);//择单个单元格或单个行
    ui->tableView->setAlternatingRowColors(true);//表格中的交替行启用交替颜色

    LabInfo=new QLabel("记录条数",this);
    LabInfo->setMinimumWidth(200);
    ui->statusBar->addWidget(LabInfo);//增加状态栏
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::on_actConnect_triggered()//连接数据库
//{
//    db=QSqlDatabase::addDatabase("QSQLITE");//添加SQLITE数据库驱动
//    db.setDatabaseName("demodb.db3");//设置数据库名称
//    if(!db.open()){
//        QMessageBox::warning(this,"错误","连接数据库失败",QMessageBox::Ok,QMessageBox::NoButton);
//        return;
//    }
//    else{
//        QMessageBox::information(this,"Information","成功");
//    }
//    ui->actConnect->setEnabled(false);
//}

void MainWindow::on_actConnect_triggered()//连接数据库
{
    db=QSqlDatabase::addDatabase("QMYSQL");//添加MYSQL数据库驱动
    db.setHostName("127.0.0.1");
    db.setDatabaseName("mydb");
    db.setUserName("tunghsiao");
    db.setPassword("12345678");

    if(!db.open()){
        QMessageBox::warning(this,"错误","连接数据库失败",QMessageBox::Ok,QMessageBox::NoButton);
        qDebug() << "无法连接数据库：" << db.lastError().text();
        return;
    }

    openTable();//打开表
}

void MainWindow::openTable(){//打开表
    qryModel=new QSqlQueryModel(this);
    theSelection=new QItemSelectionModel(qryModel);
    QString query="SELECT id,name,age,gender FROM users ORDER BY id;";
    qryModel->setQuery(query);//设置查询

    if(qryModel->lastError().isValid()){//查询出错
        QMessageBox::information(this,"错误","数据表查询错误,错误信息\n"+qryModel->lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);
        return;
    }

    qryModel->setHeaderData(0,Qt::Horizontal,"编号");
    qryModel->setHeaderData(1,Qt::Horizontal,"姓名");
    qryModel->setHeaderData(2,Qt::Horizontal,"年龄");
    qryModel->setHeaderData(3,Qt::Horizontal,"性别");

    ui->tableView->setModel(qryModel);
    ui->tableView->setSelectionModel(theSelection);

    ui->actConnect->setEnabled(false);//
}
