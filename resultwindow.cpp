#include "resultwindow.h"
#include "ui_resultwindow.h"

void ResultWindow::updateRecord(int recNo)//更新记录
{
    QSqlRecord curRec = qryModel->record(recNo); //获取当前记录
    QString eventsID = curRec.value("EventsID").toString();
    QString status = curRec.value("Status").toString();
    QString studentID = curRec.value("StudentID").toString();

    QSqlQuery query(r_db);
    query.prepare("SELECT p.EventsID, p.Status, p.StudentID, p.Score, p.Ranks, p.Team, p.BrokeSchoolRecord, s.Name "
                  "FROM Participants p "
                  "INNER JOIN Students s ON p.StudentID = s.StudentID "
                  "WHERE p.EventsID = :eventsID AND p.Status = :status AND p.StudentID = :studentID");
    query.bindValue(":eventsID", eventsID);
    query.bindValue(":status", status);
    query.bindValue(":studentID", studentID);
    query.exec();
    query.first();

//    if (!query.isValid()) //是否为有效记录
//        return;

    curRec=query.record();//获取当前记录的数据

    ResultDialog *resultDlg=new ResultDialog(this);

    Qt::WindowFlags flags=resultDlg->windowFlags();
    resultDlg->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //设置对话框固定大小
    resultDlg->setUpdateRecord(curRec);//调用对话框函数更新数据和界面

    int ret=resultDlg->exec();// 以模态方式显示对话框

    if (ret == QDialog::Accepted) {
        QSqlRecord recData = resultDlg->getRecordData();// 获取对话框返回的记录数据

        query.prepare("UPDATE Participants SET Score=:Score WHERE EventsID = :e_ID AND Status = :s AND StudentID = :s_ID");
        query.bindValue(":Score",recData.value("Score"));
        query.bindValue(":e_ID", recData.value("EventsID"));
        query.bindValue(":s", recData.value("Status"));
        query.bindValue(":s_ID", recData.value("StudentID"));
        if (!query.exec()) {
            QMessageBox::critical(this, "错误", "记录更新错误\n" + query.lastError().text(),
            QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }

        qryModel->query().exec();//数据模型重新查询数据，更新 tableView 显示
    }
    delete resultDlg;
}

void ResultWindow::getComboStatus()//获取比赛状态,填充comboStatus
{
    QSqlQuery query;
    query.exec("SELECT DISTINCT Status FROM Participants");//执行SQL语句，获取所有不同的Status值
    while(query.next()) {//遍历结果集
       QString statusValue = query.value(0).toString();
       ui->comboStatus->addItem(statusValue);//将Status值添加到comboBox中
    }
}

void ResultWindow::getComboEventsID()//获取项目编号,填充two comboEventsID
{
    QSqlQuery query;
    query.exec("SELECT DISTINCT EventsID FROM Participants");//执行SQL语句，获取所有不同的Status值
    while(query.next()) {//遍历结果集
       QString eventsIDValue = query.value(0).toString();
       ui->comboEventsRank->addItem(eventsIDValue);//将EventsID值添加到comboBox中
       ui->comboEventsPublish->addItem(eventsIDValue);
       ui->comboEventsID->addItem(eventsIDValue);
    }
}

ResultWindow::ResultWindow(QSqlDatabase& db, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ResultWindow)
    , r_db(db)
{
    ui->setupUi(this);
//    this->setCentralWidget(ui->tableView);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setAlternatingRowColors(true);

    qryModel=new QSqlQueryModel(this);
    theSelection=new QItemSelectionModel(qryModel);

    QString query = "SELECT p.EventsID, p.Status, p.StudentID, s.Name, p.Score, p.Ranks, p.Team, p.BrokeSchoolRecord "
                    "FROM Participants p "
                    "INNER JOIN Students s ON p.StudentID = s.StudentID";
    qryModel->setQuery(query);
    if (qryModel->lastError().isValid())
    {
        QMessageBox::information(this, "错误", "数据表查询错误,错误信息\n"+qryModel->lastError().text(),
        QMessageBox::Ok,QMessageBox::NoButton);
        return;
    }

    qryModel->setHeaderData(0,Qt::Horizontal,"项目编号");
    qryModel->setHeaderData(1,Qt::Horizontal,"比赛状态");
    qryModel->setHeaderData(2,Qt::Horizontal,"学号");
    qryModel->setHeaderData(3,Qt::Horizontal,"姓名");
    qryModel->setHeaderData(4,Qt::Horizontal,"比赛成绩");
    qryModel->setHeaderData(5,Qt::Horizontal,"比赛排名");
    qryModel->setHeaderData(6,Qt::Horizontal,"比赛队伍");
    qryModel->setHeaderData(7,Qt::Horizontal,"是否破纪录");

    ui->tableView->setModel(qryModel);
    ui->tableView->setSelectionModel(theSelection);

    getComboStatus();//获取比赛状态,填充comboStatus
    getComboEventsID();//获取项目编号,填充two comboEventsID

    if (theSelection->selectedIndexes().isEmpty()) {//无法选中
        ui->actEditResult->setEnabled(false);
        ui->actDeleteResult->setEnabled(false);
    }

    connect(theSelection, &QItemSelectionModel::selectionChanged, this, [=](const QItemSelection &selected, const QItemSelection &deselected){
        if (selected.isEmpty()) {
            ui->actEditResult->setEnabled(false);
            ui->actDeleteResult->setEnabled(false);
        } else {
            ui->actEditResult->setEnabled(true);
            ui->actDeleteResult->setEnabled(true);
        }
    });
}

ResultWindow::~ResultWindow()
{
    delete ui;
}

void ResultWindow::on_actEditResult_triggered()//编辑成绩
{
    int curRecNo=theSelection->currentIndex().row();
    updateRecord(curRecNo);
}

void ResultWindow::on_tableView_doubleClicked(const QModelIndex &index)//tableView上双击,编辑当前记录
{
    int curRecNo=index.row();
    updateRecord(curRecNo);
}

void ResultWindow::on_actCheckNewRecord_triggered()//确认破纪录
{
    QSqlQuery query(r_db);

    // 查询并更新 Participants 表
    query.prepare("SELECT Participants.StudentID, Participants.EventsID, Participants.Score, Participants.Team, Events.SchoolRecord "
                  "FROM Participants "
                  "INNER JOIN Events ON Participants.EventsID = Events.EventsID");
    query.exec();

    while (query.next()) {
        int studentID = query.value(0).toInt();
        QString eventID = query.value(1).toString();
        float score = query.value(2).toFloat();
        QString team = query.value(3).toString();
        float schoolRecord = query.value(4).toFloat();

        bool brokeSchoolRecord = false;

        // Check conditions for breaking school record
        if ((eventID == "E1" || eventID == "E5") && score > 0 && score < schoolRecord) {
            brokeSchoolRecord = true;
        } else if ((eventID == "E2" || eventID == "E6") && score > schoolRecord) {
            brokeSchoolRecord = true;
        } else if ((eventID == "E3" || eventID == "E7")) {
            // Check for team record breaking
            QSqlQuery teamQuery;
            teamQuery.prepare("SELECT SUM(Score) "
                              "FROM Participants "
                              "WHERE Team = :team AND EventsID = :eventID");
            teamQuery.bindValue(":team", team);
            teamQuery.bindValue(":eventID", eventID);
            if (teamQuery.exec() && teamQuery.next()) {
                int teamScoreSum = teamQuery.value(0).toInt();
                if (teamScoreSum > 0 && teamScoreSum < schoolRecord) {
                    brokeSchoolRecord = true;
                }
            } else {
                // Error handling for team record check
                qDebug() << "Failed to check team record.";
                return;
            }
        }

        // Update the brokeSchoolRecord field in the database
        QSqlQuery updateQuery(r_db);
        updateQuery.prepare("UPDATE Participants SET brokeSchoolRecord = :brokeSchoolRecord "
                            "WHERE StudentID = :studentID AND EventsID = :eventID");
        updateQuery.bindValue(":brokeSchoolRecord", brokeSchoolRecord ? 1 : 0);
        updateQuery.bindValue(":studentID", studentID);
        updateQuery.bindValue(":eventID", eventID);
        if (!updateQuery.exec()) {
            // Error handling
            qDebug() << "Failed to update brokeSchoolRecord.";
            return;
        }
    }

    // 刷新界面
    qryModel->setQuery("SELECT p.EventsID, p.Status, p.StudentID, s.Name, p.Score, p.Ranks, p.Team, p.BrokeSchoolRecord "
                       "FROM Participants p "
                       "INNER JOIN Students s ON p.StudentID = s.StudentID");

    // 查询打破校纪录的学生名单
    query.prepare("SELECT StudentID FROM Participants WHERE BrokeSchoolRecord = 1");
    query.exec();
    QString str;
    if (query.next()) {
        str = "打破校纪录的学生学号：\n";
        do {
            str += query.value(0).toString() + '\n';
        } while (query.next());
    }
    else {
        str = "没有学生打破校纪录。";
    }
    QMessageBox::information(this, "破纪录学生名单", str, QMessageBox::Ok, QMessageBox::NoButton);
}

void ResultWindow::on_rBtnP_clicked()//初赛P
{
    QSqlQuery query(r_db);
    QString genderFilter = "P";
    QString eventsFilter = ui->comboEventsID->currentText(); // 获取选择的EventsID
    QString sql = QString("SELECT p.EventsID, p.Status, p.StudentID, s.Name, p.Score, p.Ranks, p.Team, p.BrokeSchoolRecord "
                          "FROM Participants p "
                          "INNER JOIN Students s ON p.StudentID = s.StudentID "
                          "WHERE p.Status LIKE '%%1%' AND p.EventsID = '%2'")
                      .arg(genderFilter)
                      .arg(eventsFilter);
    if (!query.exec(sql)) {
        QMessageBox::critical(this, "错误", "查询失败\n" + query.lastError().text(), QMessageBox::Ok, QMessageBox::NoButton);
    } else {
        // 使用 QSqlQueryModel 对象显示查询结果
        qryModel->setQuery(query);
        ui->tableView->setModel(qryModel);
    }
}

void ResultWindow::on_rBtnR_clicked() // 复赛R
{
    QString genderFilter = "R";
    QString eventsFilter = ui->comboEventsID->currentText(); // 获取选择的EventsID

    QString sql = QString("SELECT p.EventsID, p.Status, p.StudentID, s.Name, p.Score, p.Ranks, p.Team, p.BrokeSchoolRecord "
                          "FROM Participants p "
                          "INNER JOIN Students s ON p.StudentID = s.StudentID "
                          "WHERE p.Status LIKE '%%1%' AND p.EventsID = '%2'")
                  .arg(genderFilter)
                  .arg(eventsFilter);

    QSqlQuery query(r_db);
    if (!query.exec(sql)) {
       QMessageBox::critical(this, "错误", "查询失败\n" + query.lastError().text(),
                             QMessageBox::Ok, QMessageBox::NoButton);
    } else {
       // 使用 QSqlQueryModel 对象显示查询结果
       qryModel->setQuery(query);
       ui->tableView->setModel(qryModel);
    }
}

void ResultWindow::on_rBtnF_clicked() // 决赛F
{
    QString genderFilter = "F";
    QString eventsFilter = ui->comboEventsID->currentText(); // 获取选择的EventsID

    QString sql = QString("SELECT p.EventsID, p.Status, p.StudentID, s.Name, p.Score, p.Ranks, p.Team, p.BrokeSchoolRecord "
                          "FROM Participants p "
                          "INNER JOIN Students s ON p.StudentID = s.StudentID "
                          "WHERE p.Status LIKE '%%1%' AND p.EventsID = '%2'")
                  .arg(genderFilter)
                  .arg(eventsFilter);

    QSqlQuery query(r_db);
    if (!query.exec(sql)) {
       QMessageBox::critical(this, "错误", "查询失败\n" + query.lastError().text(),
                             QMessageBox::Ok, QMessageBox::NoButton);
    } else {
       // 使用 QSqlQueryModel 对象显示查询结果
       qryModel->setQuery(query);
       ui->tableView->setModel(qryModel);
    }
}

void ResultWindow::on_btnSearch_clicked()//项目搜索
{
    // 根据搜索条件进行查询
    QString studentID = ui->editStudentID->text();
    QSqlQuery query(r_db);
    QString sql = QString("SELECT p.EventsID, p.Status, p.StudentID, s.Name, p.Score, p.Ranks, p.Team, p.BrokeSchoolRecord \
                          FROM Participants p \
                          INNER JOIN Students s ON p.StudentID = s.StudentID \
                          WHERE p.StudentID=:studentID");
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

void ResultWindow::on_BtnRank_clicked()//确认排名
{
    QString status = ui->comboStatus->currentText();
    QString eventID = ui->comboEventsRank->currentText();

    if (status.isEmpty() || eventID.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择参赛状态和比赛项目！");
        return;
    }

    bool isAscend = true; // 是否为升序排名

    if (eventID == "E1" || eventID == "E5"){
        QString sql = QString("SELECT p.EventsID, p.Status, p.StudentID, s.Name, p.Score, p.Ranks, p.Team, p.BrokeSchoolRecord "
                              "FROM Participants p "
                              "INNER JOIN Students s ON p.StudentID = s.StudentID "
                              "WHERE p.EventsID = '%1' AND p.Status = '%2' "
                              "ORDER BY p.Score %3").arg(eventID).arg(status).arg(isAscend ? "ASC" : "DESC");

        QSqlQuery query(sql);
        // 开始事务
        QSqlDatabase::database().transaction();

        // 更新参赛选手的排名
        int rank = 1;
        while (query.next()) {
            QString studentID = query.value(2).toString();
            QSqlQuery updateQuery;
            updateQuery.prepare("UPDATE Participants SET Ranks = :ranks WHERE EventsID = :eventID AND Status = :status AND StudentID = :studentID");
            updateQuery.bindValue(":ranks", rank);
            updateQuery.bindValue(":eventID", eventID);
            updateQuery.bindValue(":status", status);
            updateQuery.bindValue(":studentID", studentID);
            updateQuery.exec();
            rank++;
        }

        // 提交事务
        QSqlDatabase::database().commit();

        QMessageBox::information(this, "提示", "排名已确认！");
    }
    else if (eventID == "E2" || eventID == "E6"){
        isAscend = false;

        QString sql = QString("SELECT p.EventsID, p.Status, p.StudentID, s.Name, p.Score, p.Ranks, p.Team, p.BrokeSchoolRecord "
                              "FROM Participants p "
                              "INNER JOIN Students s ON p.StudentID = s.StudentID "
                              "WHERE p.EventsID = '%1' AND p.Status = '%2' "
                              "ORDER BY p.Score %3").arg(eventID).arg(status).arg(isAscend ? "ASC" : "DESC");

        QSqlQuery query(sql);

        // 开始事务
        QSqlDatabase::database().transaction();

        // 更新参赛选手的排名
        int rank = 1;
        while (query.next()) {
            QString studentID = query.value(2).toString();
            QSqlQuery updateQuery;
            updateQuery.prepare("UPDATE Participants SET Ranks = :ranks WHERE EventsID = :eventID AND Status = :status AND StudentID = :studentID");
            updateQuery.bindValue(":ranks", rank);
            updateQuery.bindValue(":eventID", eventID);
            updateQuery.bindValue(":status", status);
            updateQuery.bindValue(":studentID", studentID);
            updateQuery.exec();
            rank++;
        }

        // 提交事务
        QSqlDatabase::database().commit();

        QMessageBox::information(this, "提示", "排名已确认！");
    }
    else if (eventID == "E3" || eventID == "E7") {
        // 队伍内参赛者按Score升序排名，同一队伍内的参赛者排名相同
        QSqlQuery teamQuery;
        teamQuery.prepare("SELECT p.Team, SUM(p.Score) AS TotalScore "
                          "FROM Participants p "
                          "WHERE p.EventsID = :eventID AND p.Status = :status "
                          "GROUP BY p.Team "
                          "ORDER BY TotalScore ASC");
        teamQuery.bindValue(":eventID", eventID);
        teamQuery.bindValue(":status", status);

        if (teamQuery.exec()) {
            // 开始事务
            QSqlDatabase::database().transaction();

            int rank = 1;
            while (teamQuery.next()) {
                QString team = teamQuery.value(0).toString();

                QSqlQuery updateQuery;
                updateQuery.prepare("UPDATE Participants "
                                    "SET Ranks = :ranks "
                                    "WHERE EventsID = :eventID AND Status = :status AND Team = :team");
                updateQuery.bindValue(":ranks", rank);
                updateQuery.bindValue(":eventID", eventID);
                updateQuery.bindValue(":status", status);
                updateQuery.bindValue(":team", team);

                if (!updateQuery.exec()) {
                    // 错误处理
                    qDebug() << "Failed to update ranks.";
                    QSqlDatabase::database().rollback();
                    return;
                }
                rank++;
            }

            // 提交事务
            QSqlDatabase::database().commit();

            QMessageBox::information(this, "提示", "排名已确认！");
        }
    }
    else if (eventID == "E4" || eventID == "E8") {
        // 队伍内参赛者按Score降序排名，同一队伍内的参赛者排名相同
        QSqlQuery teamQuery;
        teamQuery.prepare("SELECT p.Team, SUM(p.Score) AS TotalScore "
                          "FROM Participants p "
                          "WHERE p.EventsID = :eventID AND p.Status = :status "
                          "GROUP BY p.Team "
                          "ORDER BY TotalScore DESC");
        teamQuery.bindValue(":eventID", eventID);
        teamQuery.bindValue(":status", status);

        if (teamQuery.exec()) {
            // 开始事务
            QSqlDatabase::database().transaction();

            int rank = 1;
            while (teamQuery.next()) {
                QString team = teamQuery.value(0).toString();

                QSqlQuery updateQuery;
                updateQuery.prepare("UPDATE Participants "
                                    "SET Ranks = :ranks "
                                    "WHERE EventsID = :eventID AND Status = :status AND Team = :team");
                updateQuery.bindValue(":ranks", rank);
                updateQuery.bindValue(":eventID", eventID);
                updateQuery.bindValue(":status", status);
                updateQuery.bindValue(":team", team);

                if (!updateQuery.exec()) {
                    // 错误处理
                    qDebug() << "Failed to update ranks.";
                    QSqlDatabase::database().rollback();
                    return;
                }

                rank++;
            }

            // 提交事务
            QSqlDatabase::database().commit();

            QMessageBox::information(this, "提示", "排名已确认！");
        }
    }
    else {
        // 错误处理
        qDebug() << "Failed to retrieve team scores.";
        return;
    }


    // 更新QSqlQueryModel对象
    QString sortedSql = QString("SELECT p.EventsID, p.Status, p.StudentID, s.Name, p.Score, p.Ranks, p.Team, p.BrokeSchoolRecord "
                                "FROM Participants p "
                                "INNER JOIN Students s ON p.StudentID = s.StudentID "
                                "WHERE p.EventsID = '%1' AND p.Status = '%2' "
                                "ORDER BY p.Ranks ASC").arg(eventID).arg(status);

    qryModel->setQuery(sortedSql);
    ui->tableView->setModel(qryModel);
}

void ResultWindow::on_comboEventsID_currentIndexChanged(int index)//检索项目
{
    Q_UNUSED(index);
    QString selectedEventsID = ui->comboEventsID->currentText(); // 获取选择的EventsID

    QString statusFilter;
    if (ui->rBtnP->isChecked())
        statusFilter = "P";
    else if (ui->rBtnR->isChecked())
        statusFilter = "R";
    else if (ui->rBtnF->isChecked())
        statusFilter = "F";
    else
        statusFilter = "";// 如果没有按钮被选中，则不应用任何Status过滤

    QSqlQuery query(r_db);
    QString sql = QString("SELECT p.EventsID, p.Status, p.StudentID, s.Name, p.Score, p.Ranks, p.Team, p.BrokeSchoolRecord "
                          "FROM Participants p "
                          "INNER JOIN Students s ON p.StudentID = s.StudentID "
                          "WHERE p.EventsID = '%1' AND p.Status LIKE '%%2%'")
                      .arg(selectedEventsID)
                      .arg(statusFilter);

    if (!query.exec(sql)) {
        QMessageBox::critical(this, "错误", "查询失败\n" + query.lastError().text(), QMessageBox::Ok, QMessageBox::NoButton);
    } else {
        // 使用 QSqlQueryModel 对象显示查询结果
        qryModel->setQuery(query);
        ui->tableView->setModel(qryModel);
    }
}

void ResultWindow::on_rBtnAll_clicked()//所有赛程
{
    QSqlQuery query(r_db);
    QString genderFilter = "";
    QString eventsFilter = ui->comboEventsID->currentText(); // 获取选择的EventsID
    QString sql = QString("SELECT p.EventsID, p.Status, p.StudentID, s.Name, p.Score, p.Ranks, p.Team, p.BrokeSchoolRecord "
                          "FROM Participants p "
                          "INNER JOIN Students s ON p.StudentID = s.StudentID "
                          "WHERE p.Status LIKE '%%1%' AND p.EventsID = '%2'")
                      .arg(genderFilter)
                      .arg(eventsFilter);
    if (!query.exec(sql)) {
        QMessageBox::critical(this, "错误", "查询失败\n" + query.lastError().text(), QMessageBox::Ok, QMessageBox::NoButton);
    } else {
        // 使用 QSqlQueryModel 对象显示查询结果
        qryModel->setQuery(query);
        ui->tableView->setModel(qryModel);
    }
}

void ResultWindow::on_actExportResult_triggered()//导出成绩信息
{
    QString filePath = QFileDialog::getSaveFileName(this, "保存文件", "", "Excel 文件 (*.xlsx)");
    if (filePath.isEmpty())
        return;

    lxw_workbook *workbook = workbook_new(filePath.toUtf8().constData());
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

    // 设置表头
    QStringList headers;
    headers << "项目编号" << "比赛状态" << "学号" << "姓名" << "比赛成绩" << "比赛排名" << "比赛队伍" << "是否破纪录";
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

void ResultWindow::on_BtnR_clicked() {//发布复赛
    QString eventsID = ui->comboEventsPublish->currentText();  // 获取项目编号EventsID

//    // 第一步：判断当前项目下是否存在参赛状态为R的参赛者
//    QString selectQuery = "SELECT COUNT(*) FROM Participants WHERE EventsID = :eventID AND Status = 'R'";
//    QSqlQuery query;
//    query.prepare(selectQuery);
//    query.bindValue(":eventID", eventsID);
//    query.exec();

    // 第二步：获取当前项目下的所有学生信息
    QString studentsQueryStr;
    if (eventsID == "E1" || eventsID == "E5") {
        studentsQueryStr = "SELECT StudentID, Team FROM Participants WHERE EventsID = :eventID AND Ranks BETWEEN 1 AND 12 AND Status = 'P'";
    } else if (eventsID == "E2" || eventsID == "E6") {
        studentsQueryStr = "SELECT StudentID, Team FROM Participants WHERE EventsID = :eventID AND Ranks BETWEEN 1 AND 10 AND Status = 'P'";
    } else if (eventsID == "E3" || eventsID == "E7") {
        studentsQueryStr = "SELECT StudentID, Team FROM Participants WHERE EventsID = :eventID AND Ranks BETWEEN 1 AND 6 AND Status = 'P'";
    } else if (eventsID == "E4" || eventsID == "E8") {
        studentsQueryStr = "SELECT StudentID, Team FROM Participants WHERE EventsID = :eventID AND Ranks BETWEEN 1 AND 4 AND Status = 'P'";
    } else {
        QMessageBox::warning(this, "警告", "无效的项目编号!");
        return;
    }

    QSqlQuery studentsQuery;
    studentsQuery.prepare(studentsQueryStr);
    studentsQuery.bindValue(":eventID", eventsID);
    studentsQuery.exec();

    // 遍历每个学生并插入满足条件的参赛者到Participants表
    while (studentsQuery.next()) {
        int studentID = studentsQuery.value(0).toInt();
        QString team = studentsQuery.value(1).toString();

        // 满足条件时执行插入操作
        QString insertQueryStr = "INSERT INTO Participants (EventsID, Status, StudentID, Team) "
                                  "SELECT :eventID, 'R', :studentID, :team "
                                  "WHERE NOT EXISTS "
                                  "(SELECT 1 FROM Participants WHERE EventsID = :eventID AND Status = 'R' "
                                  "AND StudentID = :studentID AND Team = :team)";
        QSqlQuery insertQuery(r_db);
        insertQuery.prepare(insertQueryStr);
        insertQuery.bindValue(":eventID", eventsID);
        insertQuery.bindValue(":studentID", studentID);
        insertQuery.bindValue(":team", team);
        insertQuery.exec();
    }
    QMessageBox::information(this, "提示", "复赛发布成功!");
}


void ResultWindow::on_BtnF_clicked()//发布决赛
{
    QString eventsID = ui->comboEventsPublish->currentText();  // 获取项目编号EventsID

    //第二步：获取当前项目下的所有学生信息
    QString studentsQueryStr;
    if (eventsID == "E1" || eventsID == "E5") {
        studentsQueryStr = "SELECT StudentID, Team FROM Participants WHERE EventsID = :eventID AND Ranks BETWEEN 1 AND 8 AND Status = 'R'";
    } else if (eventsID == "E2" || eventsID == "E6") {
        studentsQueryStr = "SELECT StudentID, Team FROM Participants WHERE EventsID = :eventID AND Ranks BETWEEN 1 AND 5 AND Status = 'R'";
    } else if (eventsID == "E3" || eventsID == "E7") {
        studentsQueryStr = "SELECT StudentID, Team FROM Participants WHERE EventsID = :eventID AND Ranks BETWEEN 1 AND 4 AND Status = 'R'";
    } else if (eventsID == "E4" || eventsID == "E8") {
        studentsQueryStr = "SELECT StudentID, Team FROM Participants WHERE EventsID = :eventID AND Ranks BETWEEN 1 AND 2 AND Status = 'R'";
    } else {
        QMessageBox::warning(this, "警告", "无效的项目编号!");
        return;
    }

    QSqlQuery studentsQuery;
    studentsQuery.prepare(studentsQueryStr);
    studentsQuery.bindValue(":eventID", eventsID);
    studentsQuery.exec();

    // 遍历每个学生并插入满足条件的参赛者到Participants表
    while (studentsQuery.next()) {
        int studentID = studentsQuery.value(0).toInt();
        QString team = studentsQuery.value(1).toString();

        // 满足条件时执行插入操作
        QString insertQueryStr = "INSERT INTO Participants (EventsID, Status, StudentID, Team) "
                                  "SELECT :eventID, 'F', :studentID, :team "
                                  "WHERE NOT EXISTS "
                                  "(SELECT 1 FROM Participants WHERE EventsID = :eventID AND Status = 'F' "
                                  "AND StudentID = :studentID AND Team = :team)";
        QSqlQuery insertQuery(r_db);
        insertQuery.prepare(insertQueryStr);
        insertQuery.bindValue(":eventID", eventsID);
        insertQuery.bindValue(":studentID", studentID);
        insertQuery.bindValue(":team", team);
        insertQuery.exec();
    }
    QMessageBox::information(this, "提示", "决赛发布成功!");
}

void ResultWindow::on_actDeleteResult_triggered()//删除成绩
{
    int curRecNo=theSelection->currentIndex().row();
    QSqlRecord curRec=qryModel->record(curRecNo); //获取当前记录
    if (curRec.isEmpty()) //当前为空记录
        return;

    QSqlQuery query(r_db); //查询出当前记录的所有字段
    // 删除 Participants 表中的记录
    query.prepare("DELETE FROM Participants WHERE EventsID = ? AND Status = ? AND StudentID = ?");
    query.addBindValue(curRec.value("EventsID"));
    query.addBindValue(curRec.value("Status"));
    query.addBindValue(curRec.value("StudentID"));
    if (!query.exec())
        QMessageBox::critical(this, "错误", "删除记录错误\n" + query.lastError().text(),
        QMessageBox::Ok, QMessageBox::NoButton);

    // 删除完记录后需要重新设置 SQL 语句查询
    qryModel->setQuery(qryModel->query().executedQuery()); // 重新查询数据
}
