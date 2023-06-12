#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("运动会管理系统");
    this->resize(1200,800);

    // 使用在 main.cpp 中建立的数据库连接
    db = QSqlDatabase::database();

//    this->setWindowState(Qt::WindowMaximized);//窗口最大化
    ui->tabWidget->setVisible(false);
    ui->tabWidget->clear();//清除所有页面
    ui->tabWidget->tabsClosable(); //Page有关闭按钮，可被关闭
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event){//退出确认
    QMessageBox::StandardButton result=QMessageBox::question(this, "确认", "确定退出本系统吗？",
                       QMessageBox::No|QMessageBox::Yes,QMessageBox::Yes);
    if(result==QMessageBox::Yes)
        event->accept();
    else
        event->ignore();
}

//void MainWindow::on_actConnect_triggered()//连接数据库
//{
//    db=QSqlDatabase::addDatabase("QMYSQL");//添加MYSQL数据库驱动
//    db.setHostName("127.0.0.1");
//    db.setDatabaseName("mydb");
//    db.setUserName("tunghsiao");
//    db.setPassword("12345678");

//    if(!db.open()){
//        QMessageBox::warning(this,"错误","连接失败！",QMessageBox::Ok,QMessageBox::NoButton);
//        qDebug() << "无法连接数据库：" << db.lastError().text();
//        return;
//    }
//    else{
//        QMessageBox::information(this, "提示", "连接成功！");

//        ui->actConnect->setEnabled(false);
//        ui->actApply->setEnabled(true);
//        ui->actWithdraw->setEnabled(true);
//        ui->actPlayerManage->setEnabled(true);
//        ui->actEventsManage->setEnabled(true);
//        ui->actJudgesManage->setEnabled(true);
//        ui->actArrangeManage->setEnabled(true);
//        ui->actResultManage->setEnabled(true);
//    }
//}

void MainWindow::on_actApply_triggered()//报名
{
    QString fileName=QFileDialog::getOpenFileName(this, tr("选择要导入的xlsx文件"), ".", tr("Excel 文件(*.xlsx)"));
    if(fileName.isEmpty())
        return;

    QXlsx::Document xlsx(fileName);//打开xlsx文件
    QXlsx::Worksheet *sheet = xlsx.currentWorksheet();//获取第1个sheet
    int rowCount=sheet->dimension().rowCount();
    int colCount=sheet->dimension().columnCount();

    QMap<QString, int> eventsCount; // 保存每个项目的报名人数
    QStringList errorEvents; // 保存超出人数上限的项目ID
    for(int i=2;i<=rowCount;++i){//第1行是标题栏，从第2行开始
        QVariantList rowData;
        for(int j=1;j<=colCount;++j)
            rowData<<sheet->read(i,j);

        QString eventsID = rowData.at(8).toString(); // 获取该学生报名的项目ID
        if (eventsCount.contains(eventsID)) { // 如果该项目已经有人报名
            eventsCount[eventsID]++; // 该项目报名人数加1
        } else {
            eventsCount.insert(eventsID, 1); // 该项目报名人数初始化为1
        }

        // 判断是否超出人数上限
        if ((eventsID == "E1" && eventsCount[eventsID] > 20)
            || (eventsID == "E2" && eventsCount[eventsID] > 15)
            || (eventsID == "E3" && eventsCount[eventsID] > 40)
            || (eventsID == "E4" && eventsCount[eventsID] > 96)
            || (eventsID == "E5" && eventsCount[eventsID] > 20)
            || (eventsID == "E6" && eventsCount[eventsID] > 15)
            || (eventsID == "E7" && eventsCount[eventsID] > 40)
            || (eventsID == "E8" && eventsCount[eventsID] > 96)) {
            errorEvents.append(eventsID); // 保存超出人数上限的项目ID
        }
    }

    // 如果有超出人数上限的项目，弹窗提示并返回
    if (!errorEvents.isEmpty()) {
        QString errorMsg = tr("以下项目已经超出人数上限：\n%1\n请修改报名信息后重新导入！").arg(errorEvents.join(", "));
        QMessageBox::warning(this, tr("报名失败"), errorMsg);
        return;
    }

    // 遍历完所有数据后，再将数据导入数据库
    QSqlQuery query(db);
    for(int i=2;i<=rowCount;++i){//第1行是标题栏，从第2行开始
        QVariantList rowData;
        for(int j=1;j<=colCount;++j)
            rowData<<sheet->read(i,j);

        query.prepare("INSERT INTO Students (StudentID, Name, Gender, Age, CollegeID, Major, Class, Phone)"
                      " VALUES (:StudentID, :Name, :Gender, :Age, :CollegeID, :Major, :Class, :Phone)");
        query.bindValue(":StudentID", rowData.at(0));
        query.bindValue(":Name", rowData.at(1));
        query.bindValue(":Gender", rowData.at(2));
        query.bindValue(":Age", rowData.at(3));
        query.bindValue(":CollegeID", rowData.at(4));
        query.bindValue(":Major", rowData.at(5));
        query.bindValue(":Class", rowData.at(6));
        query.bindValue(":Phone", rowData.at(7));
        query.exec();

        query.prepare("INSERT INTO Participants (StudentID, Team, EventsID) VALUES (:StudentID, :Team, :EventsID)");
        query.bindValue(":StudentID", rowData.at(0));
        query.bindValue(":Team",rowData.at(9));
        query.bindValue(":EventsID", rowData.at(8));
        query.exec();
    }
    QMessageBox::information(this, tr("提示"), tr("报名成功！"));
}

void MainWindow::on_actWithdraw_triggered()//退赛
{
    QString fileName=QFileDialog::getOpenFileName(this, tr("选择要导入的xlsx文件"), ".", tr("Excel 文件(*.xlsx)"));
    if(fileName.isEmpty())
        return;

    QXlsx::Document xlsx(fileName);//打开xlsx文件
    QXlsx::Worksheet *sheet = xlsx.currentWorksheet();//获取第1个sheet
    int rowCount=sheet->dimension().rowCount();
    int colCount=sheet->dimension().columnCount();

    QSqlQuery query(db);
    for(int i=2;i<=rowCount;++i){//第1行是标题栏，从第2行开始
        QVariantList rowData;
        for(int j=1;j<=colCount;++j)
            rowData<<sheet->read(i,j);

        QString studentID = rowData.at(0).toString();
        QString eventsID = rowData.at(1).toString();

        query.prepare("DELETE FROM Participants WHERE StudentID = ? AND EventsID = ?");//'?'避免SQL注入攻击
        query.addBindValue(studentID);
        query.addBindValue(eventsID);
        query.exec();

        query.prepare("DELETE FROM Students WHERE StudentID = ?");//'?'避免SQL注入攻击
        query.addBindValue(studentID);
        query.exec();
    }
    QMessageBox::information(this, tr("提示"), tr("退赛成功！"));
}

void MainWindow::on_actPlayerManage_triggered()//选手管理窗口
{
    PlayerWindow *playerWin=new PlayerWindow(db,this);
    playerWin->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动删除
    ui->tabWidget->addTab(playerWin,QString::asprintf("选手管理"));//Tab标题
    ui->tabWidget->setVisible(true);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)//关闭Tab
{
    if (index<0)
        return;

    QWidget *widget = ui->tabWidget->widget(index);
    if (widget != nullptr) {
        widget->close();
    }
}

void MainWindow::on_actEventsManage_triggered()//项目管理窗口
{
    EventsWindow *eventsWin=new EventsWindow(db,this);
    eventsWin->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动删除
    ui->tabWidget->addTab(eventsWin,QString::asprintf("项目管理"));
    ui->tabWidget->setVisible(true);
}

void MainWindow::on_actJudgesManage_triggered()//裁判管理窗口
{
    JudgesWindow *judgesWin=new JudgesWindow(db,this);
    judgesWin->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动删除
    ui->tabWidget->addTab(judgesWin,QString::asprintf("裁判管理"));
    ui->tabWidget->setVisible(true);
}

void MainWindow::on_actArrangeManage_triggered()//赛程管理窗口
{
    ArrangeWindow *arrangeWin=new ArrangeWindow(db,this);
    arrangeWin->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动删除
    ui->tabWidget->addTab(arrangeWin,QString::asprintf("赛程管理"));
    ui->tabWidget->setVisible(true);
}

void MainWindow::on_actResultManage_triggered()//成绩管理窗口
{
    ResultWindow *resultWin=new ResultWindow(db,this);
    resultWin->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动删除
    ui->tabWidget->addTab(resultWin,QString::asprintf("成绩管理"));
    ui->tabWidget->setVisible(true);
}
