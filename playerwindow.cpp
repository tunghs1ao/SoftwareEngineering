#include "playerwindow.h"
#include"mainwindow.h"
#include"playerdialog.h"
#include "ui_playerwindow.h"

void PlayerWindow::updateRecord(int recNo)//更新记录
{
    QSqlRecord curRec=qryModel->record(recNo); //获取当前记录
    int studentID=curRec.value("StudentID").toInt();//获取StudentID

    QSqlQuery query(p_db); //查询出当前记录的所有字段
    query.prepare("SELECT Students.StudentID, Students.Name, Students.Gender, Students.Age, Students.CollegeID, \
        Students.Major, Students.Class, Students.Phone, Participants.EventsID, Participants.Team FROM Students \
        INNER JOIN Participants ON Students.StudentID = Participants.StudentID WHERE Students.StudentID = :ID");
    query.bindValue(":ID", studentID);
    query.exec();
    query.first();

    if (!query.isValid()) //是否为有效记录
        return;

    curRec=query.record();//获取当前记录的数据

    PlayerDialog *playerDialog=new PlayerDialog(this);
    Qt::WindowFlags flags=playerDialog->windowFlags();
    playerDialog->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //设置对话框固定大小
    playerDialog->setUpdateRecord(curRec);//调用对话框函数更新数据和界面
    int ret=playerDialog->exec();// 以模态方式显示对话框

    if (ret == QDialog::Accepted) {
        // 获取对话框返回的记录数据
        QSqlRecord recData = playerDialog->getRecordData();

        // 更新 Students 表中的记录
        query.prepare("UPDATE Students SET Name=:Name, Gender=:Gender, Age=:Age, CollegeID=:CollegeID, Major=:Major, Class=:Class, Phone=:Phone WHERE StudentID=:ID");
        query.bindValue(":Name", recData.value("Name"));
        query.bindValue(":Gender", recData.value("Gender"));
        query.bindValue(":Age", recData.value("Age"));
        query.bindValue(":CollegeID", recData.value("CollegeID"));
        query.bindValue(":Major", recData.value("Major"));
        query.bindValue(":Class", recData.value("Class"));
        query.bindValue(":Phone", recData.value("Phone"));
        query.bindValue(":ID", studentID);
        if (!query.exec()) {
            QMessageBox::critical(this, "错误", "记录更新错误\n" + query.lastError().text(),
                                     QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }

        // 更新 Participants 表中的记录
        query.prepare("UPDATE Participants SET EventsID=:EventsID, Team=:Team WHERE StudentID=:ID");
        query.bindValue(":EventsID", recData.value("EventsID"));
        query.bindValue(":Team", recData.value("Team"));
        query.bindValue(":ID", studentID);
        if (!query.exec()) {
            QMessageBox::critical(this, "错误", "记录更新错误\n" + query.lastError().text(),
                                     QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }

        // 数据模型重新查询数据，更新 tableView 显示
        qryModel->query().exec();
    }
    delete playerDialog;
}

void PlayerWindow::getFieldNames()//获取字段名称,填充“排序字段”的comboBox
{
    QSqlRecord record = qryModel->record();
    for(int i = 0; i < record.count(); i++) {
        QString fieldName = record.fieldName(i);
        ui->comboFilter->addItem(fieldName);
    }
}

PlayerWindow::PlayerWindow(QSqlDatabase& db, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PlayerWindow)
    , p_db(db)
{
    ui->setupUi(this);
//    this->setCentralWidget(ui->tableView);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setAlternatingRowColors(true);

    qryModel=new QSqlQueryModel(this);
    theSelection=new QItemSelectionModel(qryModel);

    QString query ="SELECT DISTINCT s.StudentID, s.Name, s.Gender, s.Age, s.CollegeID, s.Major, s.Class, s.phone, \
            p.EventsID, p.Team \
            FROM Students s \
            JOIN Participants p ON s.StudentID = p.StudentID;";
    qryModel->setQuery(query);

    if (qryModel->lastError().isValid()){
        QMessageBox::information(this, "错误", "数据表查询错误,错误信息\n"+qryModel->lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);
        return;
    }

    qryModel->setHeaderData(0,Qt::Horizontal,"学号");
    qryModel->setHeaderData(1,Qt::Horizontal,"姓名");
    qryModel->setHeaderData(2,Qt::Horizontal,"性别");
    qryModel->setHeaderData(3,Qt::Horizontal,"年龄");
    qryModel->setHeaderData(4,Qt::Horizontal,"学院编号");
    qryModel->setHeaderData(5,Qt::Horizontal,"专业");
    qryModel->setHeaderData(6,Qt::Horizontal,"班级");
    qryModel->setHeaderData(7,Qt::Horizontal,"电话");
    qryModel->setHeaderData(8,Qt::Horizontal,"项目编号");
    qryModel->setHeaderData(9,Qt::Horizontal,"比赛队伍");

    ui->tableView->setModel(qryModel);
    ui->tableView->setSelectionModel(theSelection);

    getFieldNames();//获取字段名称，填充“排序字段”comboBox

    if (theSelection->selectedIndexes().isEmpty()) {//无法选中
        ui->actDeletePlayer->setEnabled(false);
        ui->actModifyPlayer->setEnabled(false);
    }

    connect(theSelection, &QItemSelectionModel::selectionChanged, this, [=](const QItemSelection &selected, const QItemSelection &deselected){
        if (selected.isEmpty()) {
            ui->actDeletePlayer->setEnabled(false);
            ui->actModifyPlayer->setEnabled(false);
        } else {
            ui->actDeletePlayer->setEnabled(true);
            ui->actModifyPlayer->setEnabled(true);
        }
    });
}

PlayerWindow::~PlayerWindow()
{
    delete ui;
}

void PlayerWindow::on_actAddPlayer_triggered()//添加选手
{
    QSqlQuery query(p_db);
    query.exec("SELECT Students.*, Participants.* "
               "FROM Students LEFT JOIN Participants "
               "ON Students.StudentID = Participants.StudentID "
               "WHERE Students.StudentID = -1");//实际不查询出记录，只查询字段信息
    QSqlRecord curRec=query.record();//获取当前记录,实际为空记录

    PlayerDialog *playerDialog=new PlayerDialog(this);
    Qt::WindowFlags flags=playerDialog->windowFlags();
    playerDialog->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //设置对话框固定大小
    playerDialog->setAddRecord(curRec);

    int ret=playerDialog->exec();// 以模态方式显示对话框
    if (ret==QDialog::Accepted){//OK键被按下
        QSqlRecord recData=playerDialog->getRecordData(); //获得对话框返回的记录

        query.prepare("INSERT INTO Students (StudentID, Name, Gender, Age, CollegeID, Major, Class, Phone) \
                       VALUES (:StudentID, :Name, :Gender, :Age, :CollegeID, :Major, :Class, :Phone)");
        query.bindValue(":StudentID", recData.value("StudentID"));
        query.bindValue(":Name", recData.value("Name"));
        query.bindValue(":Gender", recData.value("Gender"));
        query.bindValue(":Age", recData.value("Age"));
        query.bindValue(":CollegeID", recData.value("CollegeID"));
        query.bindValue(":Major", recData.value("Major"));
        query.bindValue(":Class", recData.value("Class"));
        query.bindValue(":Phone", recData.value("Phone"));
        if (!query.exec())
            QMessageBox::critical(this, "错误", "添加选手错误\n"+query.lastError().text(),
                                     QMessageBox::Ok,QMessageBox::NoButton);

        query.prepare("INSERT INTO Participants (StudentID, Team, EventsID) VALUES (?, ?, ?)");
        query.addBindValue(recData.value("StudentID"));
        query.addBindValue(recData.value("Team"));
        query.addBindValue(recData.value("EventsID"));
        if (!query.exec())
            QMessageBox::critical(this, "错误", "添加选手错误\n"+query.lastError().text(),
                                     QMessageBox::Ok,QMessageBox::NoButton);

        else //插入，删除记录后需要重新设置SQL语句查询
            qryModel->setQuery(qryModel->query().executedQuery()); // 重新查询数据
    }
    delete playerDialog;
}

void PlayerWindow::on_actModifyPlayer_triggered()//修改选手信息
{
    int curRecNo=theSelection->currentIndex().row();
    updateRecord(curRecNo);
}

void PlayerWindow::on_actDeletePlayer_triggered()//删除选手
{
    int curRecNo=theSelection->currentIndex().row();
    QSqlRecord curRec=qryModel->record(curRecNo); //获取当前记录
    if (curRec.isEmpty()) //当前为空记录
        return;

    QSqlQuery query(p_db); //查询出当前记录的所有字段
//    query.prepare("DELETE FROM Participants WHERE StudentID = ? AND EventsID = ?");
//    query.addBindValue(curRec.value("StudentID"));
//    query.addBindValue(curRec.value("EventsID"));

    // 开始事务
    query.exec("BEGIN TRANSACTION");

    // 删除Participants表中符合条件的记录
    query.prepare("DELETE FROM Participants WHERE StudentID = ? AND EventsID = ?");
    query.addBindValue(curRec.value("StudentID"));
    query.addBindValue(curRec.value("EventsID"));
    query.exec();

    // 删除Students表中对应的记录
    query.prepare("DELETE FROM Students WHERE StudentID = ?");
    query.addBindValue(curRec.value("StudentID"));
    query.exec();

    // 提交事务
    query.exec("COMMIT");

    if (!query.exec())
        QMessageBox::critical(this, "错误", "删除选手出现错误\n"+query.lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);
    else //插入，删除记录后需要重新设置SQL语句查询
        qryModel->setQuery(qryModel->query().executedQuery()); // 重新查询数据
}

void PlayerWindow::on_tableView_doubleClicked(const QModelIndex &index)//tableView上双击,编辑当前记录
{
    int curRecNo=index.row();
    updateRecord(curRecNo);
}

void PlayerWindow::on_rBtnAscend_clicked()//升序
{
    //使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(p_db);
    QString fieldName = ui->comboFilter->currentText();
    QString sortOrder = "ASC"; // 默认升序排列
    QString sql = QString("SELECT s.StudentID, s.Name, s.Gender, s.Age, s.CollegeID, s.Major, s.Class, s.phone, \
                           p.EventsID, p.Team FROM Students s JOIN Participants p ON s.StudentID = p.StudentID \
                           ORDER BY %1 %2").arg(fieldName).arg(sortOrder);
    if (!query.exec(sql)) {
       QMessageBox::critical(this, "错误", "排序失败\n" + query.lastError().text(),
                                    QMessageBox::Ok, QMessageBox::NoButton);
    }
    else {
       // 使用 QSqlQueryModel 对象显示查询结果
       qryModel->setQuery(query);
       ui->tableView->setModel(qryModel);
    }
}

void PlayerWindow::on_rBtnDescend_clicked()//降序
{
    //使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(p_db);
    QString fieldName = ui->comboFilter->currentText();
    QString sortOrder = "DESC"; // 默认升序排列
    QString sql = QString("SELECT s.StudentID, s.Name, s.Gender, s.Age, s.CollegeID, s.Major, s.Class, s.phone, \
                           p.EventsID, p.Team FROM Students s JOIN Participants p ON s.StudentID = p.StudentID \
                           ORDER BY %1 %2").arg(fieldName).arg(sortOrder);
    if (!query.exec(sql)) {
       QMessageBox::critical(this, "错误", "排序失败\n" + query.lastError().text(),
                                    QMessageBox::Ok, QMessageBox::NoButton);
    }
    else {
       // 使用 QSqlQueryModel 对象显示查询结果
       qryModel->setQuery(query);
       ui->tableView->setModel(qryModel);
    }
}

void PlayerWindow::on_comboFilter_currentIndexChanged(int index)//选择字段进行排序
{
    Q_UNUSED(index);
    //获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())//如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    //使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(p_db);
    QString fieldName = ui->comboFilter->currentText();
    QString sql = QString("SELECT s.StudentID, s.Name, s.Gender, s.Age, s.CollegeID, s.Major, s.Class, s.phone, \
                           p.EventsID, p.Team FROM Students s JOIN Participants p ON s.StudentID = p.StudentID \
                           ORDER BY %1 %2").arg(fieldName).arg(sortOrder);
    if (!query.exec(sql)) {
       QMessageBox::critical(this, "错误", "排序失败\n" + query.lastError().text(),
                                    QMessageBox::Ok, QMessageBox::NoButton);
    }
    else {
       // 使用 QSqlQueryModel 对象显示查询结果
       qryModel->setQuery(query);
       ui->tableView->setModel(qryModel);
    }
}

void PlayerWindow::on_rBtnMan_clicked()//男
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(p_db);
    QString fieldName = ui->comboFilter->currentText();
    QString genderFilter = "男";
    QString sql = QString("SELECT s.StudentID, s.Name, s.Gender, s.Age, s.CollegeID, s.Major, s.Class, s.phone, \
                          p.EventsID, p.Team FROM Students s JOIN Participants p ON s.StudentID = p.StudentID \
                          WHERE Gender LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
    if (!query.exec(sql)) {
       QMessageBox::critical(this, "错误", "查询失败\n" + query.lastError().text(),
                             QMessageBox::Ok, QMessageBox::NoButton);
    }
    else {
       // 使用 QSqlQueryModel 对象显示查询结果
       qryModel->setQuery(query);
       ui->tableView->setModel(qryModel);
    }
}

void PlayerWindow::on_rBtnWoman_clicked()//女
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(p_db);
    QString fieldName = ui->comboFilter->currentText();
    QString genderFilter = "女";
    QString sql = QString("SELECT s.StudentID, s.Name, s.Gender, s.Age, s.CollegeID, s.Major, s.Class, s.phone, \
                          p.EventsID, p.Team FROM Students s JOIN Participants p ON s.StudentID = p.StudentID \
                          WHERE Gender LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
    if (!query.exec(sql)) {
       QMessageBox::critical(this, "错误", "查询失败\n" + query.lastError().text(),
                             QMessageBox::Ok, QMessageBox::NoButton);
    }
    else {
       // 使用 QSqlQueryModel 对象显示查询结果
       qryModel->setQuery(query);
       ui->tableView->setModel(qryModel);
    }
}

void PlayerWindow::on_rBtnBoth_clicked()//全显示
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(p_db);
    QString fieldName = ui->comboFilter->currentText();
    QString genderFilter = ""; // 不做性别过滤
    QString sql = QString("SELECT s.StudentID, s.Name, s.Gender, s.Age, s.CollegeID, s.Major, s.Class, s.phone, \
                          p.EventsID, p.Team FROM Students s JOIN Participants p ON s.StudentID = p.StudentID \
                          WHERE Gender LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
    if (!query.exec(sql)) {
       QMessageBox::critical(this, "错误", "查询失败\n" + query.lastError().text(),
                             QMessageBox::Ok, QMessageBox::NoButton);
    }
    else {
       // 使用 QSqlQueryModel 对象显示查询结果
       qryModel->setQuery(query);
       ui->tableView->setModel(qryModel);
    }
}

void PlayerWindow::on_btnSearch_clicked()//学号搜索
{
    // 根据搜索条件进行查询
    int studentID = ui->editStudentID->text().toInt();
    QSqlQuery query(p_db);
    QString sql = "SELECT s.StudentID, s.Name, s.Gender, s.Age, s.CollegeID, s.Major, s.Class, s.phone, \
                   p.EventsID, p.Team \
                   FROM Students s JOIN Participants p ON s.StudentID = p.StudentID \
                   WHERE s.StudentID = :studentID";
    query.prepare(sql);
    query.bindValue(":studentID", studentID);
    if (!query.exec()) {
        QMessageBox::critical(this, "错误", "查询失败\n" + query.lastError().text(),
                                    QMessageBox::Ok, QMessageBox::NoButton);
    } else {
        // 更新QSqlQueryModel对象
        qryModel->setQuery(query);
        ui->tableView->setModel(qryModel);
    }
}

void PlayerWindow::on_actExportPlayer_triggered()//导出选手信息
{
    QString filePath = QFileDialog::getSaveFileName(this, "保存文件", "", "Excel 文件 (*.xlsx)");
    if (filePath.isEmpty())
        return;

    lxw_workbook *workbook = workbook_new(filePath.toUtf8().constData());
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

    // 设置表头
    QStringList headers;
    headers << "学号" << "姓名" << "性别" << "年龄" << "学院编号" << "专业" << "班级" << "电话" << "项目编号" << "比赛队伍";
    lxw_format *headerFormat = workbook_add_format(workbook);
    format_set_bold(headerFormat);
    for (int i = 0; i < headers.size(); i++) {
        worksheet_write_string(worksheet, 0, i, headers[i].toUtf8().constData(), headerFormat);
    }

    // 获取表格数据
    int rowCount = qryModel->rowCount();
    int columnCount = qryModel->columnCount();
    for (int row = 0; row < rowCount; row++) {
        for (int col = 0; col < columnCount; col++) {
            QModelIndex index = qryModel->index(row, col);
            QString value = qryModel->data(index).toString();
            worksheet_write_string(worksheet, row + 1, col, value.toUtf8().constData(), NULL);
        }
    }

    // 保存文件
    if (workbook_close(workbook) == LXW_NO_ERROR) {
        QMessageBox::information(this, "导出成功", "表格已成功导出为Excel文件！");
    } else {
        QMessageBox::warning(this, "导出失败", "导出表格为Excel文件时发生错误！");
    }
}
