#include "eventswindow.h"
#include "ui_eventswindow.h"

void EventsWindow::updateRecord(int recNo)//更新记录
{
    QSqlRecord curRec=qryModel->record(recNo); //获取当前记录
    QString eventsID=curRec.value("EventsID").toString();

    QSqlQuery query(e_db); //查询出当前记录的所有字段
    query.prepare("SELECT Events.EventsID, EventsName, Type, MaxParticipants, SchoolRecord, JudgeID "
                  "FROM Events "
                  "JOIN Control ON Events.EventsID = Control.EventsID "
                  "WHERE Events.EventsID = :ID");
    query.bindValue(":ID", eventsID);
    query.exec();
    query.first();

    if (!query.isValid()) //是否为有效记录
        return;

    curRec=query.record();//获取当前记录的数据

    EventsDialog *eventsDlg=new EventsDialog(this);
    Qt::WindowFlags flags=eventsDlg->windowFlags();
    eventsDlg->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //设置对话框固定大小
    eventsDlg->setUpdateRecord(curRec);//调用对话框函数更新数据和界面
    int ret=eventsDlg->exec();// 以模态方式显示对话框

    if (ret == QDialog::Accepted) {
        QSqlRecord recData = eventsDlg->getRecordData();// 获取对话框返回的记录数据

        // 更新 Events 表中的记录
        query.prepare("UPDATE Events SET EventsName=:EventsName, Type=:Type, \
            MaxParticipants=:MaxParticipants, SchoolRecord=:SchoolRecord WHERE EventsID=:ID");
        query.bindValue(":EventsName", recData.value("EventsName"));
        query.bindValue(":Type", recData.value("Type"));
        query.bindValue(":MaxParticipants", recData.value("MaxParticipants"));
        query.bindValue(":SchoolRecord", recData.value("SchoolRecord"));
        query.bindValue(":ID", recData.value("EventsID"));
        if (!query.exec()) {
            QMessageBox::critical(this, "错误", "记录更新错误\n" + query.lastError().text(),
            QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }

        // 更新 Control 表中的记录
        query.prepare("UPDATE Control SET JudgeID=:JudgeID WHERE EventsID=:EventsID");
        query.bindValue(":JudgeID", recData.value("JudgeID"));
        query.bindValue(":EventsID", recData.value("EventsID"));
        if (!query.exec()) {
            QMessageBox::critical(this, "错误", "记录更新错误\n" + query.lastError().text(),
            QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }

        qryModel->query().exec();//数据模型重新查询数据，更新 tableView 显示
    }
    delete eventsDlg;
}

void EventsWindow::getFieldNames()//获取字段名称,填充“排序字段”的comboBox
{
    QSqlRecord record = qryModel->record();
    for(int i = 0; i < record.count(); i++) {
        QString fieldName = record.fieldName(i);
        ui->comboFilter->addItem(fieldName);
    }
}

EventsWindow::EventsWindow(QSqlDatabase& db, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EventsWindow)
    , e_db(db)
{
    ui->setupUi(this);
//    this->setCentralWidget(ui->tableView);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setAlternatingRowColors(true);

    qryModel=new QSqlQueryModel(this);
    theSelection=new QItemSelectionModel(qryModel);

    QString query = "SELECT Events.EventsID, EventsName, Type, MaxParticipants, SchoolRecord, JudgeID "
                    "FROM Events "
                    "JOIN Control ON Events.EventsID = Control.EventsID";
    qryModel->setQuery(query);
    if (qryModel->lastError().isValid())
    {
        QMessageBox::information(this, "错误", "数据表查询错误,错误信息\n"+qryModel->lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);
        return;
    }

    qryModel->setHeaderData(0,Qt::Horizontal,"项目编号");
    qryModel->setHeaderData(1,Qt::Horizontal,"项目名称");
    qryModel->setHeaderData(2,Qt::Horizontal,"项目类型");
    qryModel->setHeaderData(3,Qt::Horizontal,"最大参与人数");
    qryModel->setHeaderData(4,Qt::Horizontal,"比赛纪录");
    qryModel->setHeaderData(5,Qt::Horizontal,"裁判编号");

    ui->tableView->setModel(qryModel);
    ui->tableView->setSelectionModel(theSelection);

    getFieldNames();//获取字段名称，填充“排序字段”comboBox

    if (theSelection->selectedIndexes().isEmpty()) {//无法选中
        ui->actModifyEvent->setEnabled(false);
        ui->actDeleteEvent->setEnabled(false);
    }

    connect(theSelection, &QItemSelectionModel::selectionChanged, this, [=](const QItemSelection &selected, const QItemSelection &deselected){
        if (selected.isEmpty()) {
            ui->actModifyEvent->setEnabled(false);
            ui->actDeleteEvent->setEnabled(false);
        } else {
            ui->actModifyEvent->setEnabled(true);
            ui->actDeleteEvent->setEnabled(true);
        }
    });
}

EventsWindow::~EventsWindow()
{
    delete ui;
}

void EventsWindow::on_actAddEvent_triggered()//添加项目
{
    QSqlQuery query(e_db);
    //字段缺失!
    query.exec("SELECT Events.*, Control.* "
               "FROM Events LEFT JOIN Control "
               "ON Events.EventsID = Control.EventsID "
               "WHERE Events.EventsID = -1");//实际不查询出记录，只查询字段信息
    QSqlRecord curRec=query.record();//获取当前记录,实际为空记录

    EventsDialog *eventsDlg=new EventsDialog(this);
    Qt::WindowFlags flags=eventsDlg->windowFlags();
    eventsDlg->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //设置对话框固定大小
    eventsDlg->setAddRecord(curRec);//调用对话框函数更新数据和界面
    int ret=eventsDlg->exec();// 以模态方式显示对话框

    if (ret==QDialog::Accepted){//OK键被按下
        QSqlRecord recData=eventsDlg->getRecordData(); //获得对话框返回的记录

        // 向 Events 表中插入记录
        query.prepare("INSERT INTO Events (EventsID, EventsName, Type, MaxParticipants, SchoolRecord) \
                       VALUES (:EventsID, :EventsName, :Type, :MaxParticipants, :SchoolRecord)");
        query.bindValue(":EventsID", recData.value("EventsID"));
        query.bindValue(":EventsName", recData.value("EventsName"));
        query.bindValue(":Type", recData.value("Type"));
        query.bindValue(":MaxParticipants", recData.value("MaxParticipants"));
        query.bindValue(":SchoolRecord", recData.value("SchoolRecord"));
        if (!query.exec())
            QMessageBox::critical(this, "错误", "添加选手错误\n" + query.lastError().text(),
                                    QMessageBox::Ok, QMessageBox::NoButton);

        // 向 Control 表中插入记录
        query.prepare("INSERT INTO Control (EventsID, JudgeID) VALUES (:EventsID, :JudgeID)");
        query.bindValue(":EventsID", recData.value("EventsID"));
        query.bindValue(":JudgeID", recData.value("JudgeID"));
        if (!query.exec())
            QMessageBox::critical(this, "错误", "添加选手错误\n" + query.lastError().text(),
                                    QMessageBox::Ok, QMessageBox::NoButton);

        // 插入、删除记录后需要重新设置SQL语句查询
        qryModel->setQuery(qryModel->query().executedQuery()); // 重新查询数据
    }
    delete eventsDlg;
}

void EventsWindow::on_actModifyEvent_triggered()//
{
    int curRecNo=theSelection->currentIndex().row();
    updateRecord(curRecNo);
}

void EventsWindow::on_tableView_doubleClicked(const QModelIndex &index)//tableView上双击,编辑当前记录
{
    int curRecNo=index.row();
    updateRecord(curRecNo);
}

void EventsWindow::on_actDeleteEvent_triggered()//删除项目
{
    int curRecNo=theSelection->currentIndex().row();
    QSqlRecord curRec=qryModel->record(curRecNo); //获取当前记录
    if (curRec.isEmpty()) //当前为空记录
        return;

    QSqlQuery query(e_db); //查询出当前记录的所有字段
    // 删除 Control 表中的记录
    query.prepare("DELETE FROM Control WHERE EventsID = ?");
    query.addBindValue(curRec.value("EventsID"));
    if (!query.exec())
        QMessageBox::critical(this, "错误", "删除记录错误\n" + query.lastError().text(),
        QMessageBox::Ok, QMessageBox::NoButton);

    // 删除 Participants 表中的记录
    query.prepare("DELETE FROM Participants WHERE EventsID = ?");
    query.addBindValue(curRec.value("EventsID"));
    if (!query.exec())
        QMessageBox::critical(this, "错误", "删除记录错误\n" + query.lastError().text(),
        QMessageBox::Ok, QMessageBox::NoButton);

    // 删除 Events 表中的记录
    query.prepare("DELETE FROM Events WHERE EventsID = ?");
    query.addBindValue(curRec.value("EventsID"));
    if (!query.exec())
        QMessageBox::critical(this, "错误", "删除记录错误\n" + query.lastError().text(),
        QMessageBox::Ok, QMessageBox::NoButton);

    // 删除完记录后需要重新设置 SQL 语句查询
    qryModel->setQuery(qryModel->query().executedQuery()); // 重新查询数据
}

void EventsWindow::on_rBtnAscend_clicked()//升序
{
    //使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(e_db);
    QString fieldName = ui->comboFilter->currentText();
    QString sortOrder = "ASC"; // 默认升序排列
    QString sql = QString("SELECT Events.EventsID, EventsName, Type, MaxParticipants, SchoolRecord, JudgeID "
                           "FROM Events JOIN Control ON Events.EventsID = Control.EventsID "
                           "ORDER BY %1 %2").arg(fieldName).arg(sortOrder);
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

void EventsWindow::on_rBtnDescend_clicked()//降序
{
    //使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(e_db);
    QString fieldName = ui->comboFilter->currentText();
    QString sortOrder = "DESC"; // 默认升序排列
    QString sql = QString("SELECT Events.EventsID, EventsName, Type, MaxParticipants, SchoolRecord, JudgeID "
                           "FROM Events JOIN Control ON Events.EventsID = Control.EventsID "
                           "ORDER BY %1 %2").arg(fieldName).arg(sortOrder);
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


void EventsWindow::on_comboFilter_currentIndexChanged(int index)//选择字段进行排序
{
    Q_UNUSED(index);
    //获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())//如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    //使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(e_db);
    QString fieldName = ui->comboFilter->currentText();
    QString sql = QString("SELECT Events.EventsID, EventsName, Type, MaxParticipants, SchoolRecord, JudgeID "
                           "FROM Events JOIN Control ON Events.EventsID = Control.EventsID "
                           "ORDER BY %1 %2").arg(fieldName).arg(sortOrder);
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

void EventsWindow::on_rBtnPersonal_clicked()//个人赛
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(e_db);
    QString fieldName = ui->comboFilter->currentText();
    QString genderFilter = "个人赛";
    QString sql = QString("SELECT Events.EventsID, EventsName, Type, MaxParticipants, SchoolRecord, JudgeID "
                          "FROM Events JOIN Control ON Events.EventsID = Control.EventsID \
                          WHERE Type LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
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

void EventsWindow::on_rBtnTeam_clicked()//团体赛
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(e_db);
    QString fieldName = ui->comboFilter->currentText();
    QString genderFilter = "团体赛";
    QString sql = QString("SELECT Events.EventsID, EventsName, Type, MaxParticipants, SchoolRecord, JudgeID "
                          "FROM Events JOIN Control ON Events.EventsID = Control.EventsID \
                          WHERE Type LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
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

void EventsWindow::on_rBtnBoth_clicked()//全显示
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(e_db);
    QString fieldName = ui->comboFilter->currentText();
    QString genderFilter = "";
    QString sql = QString("SELECT Events.EventsID, EventsName, Type, MaxParticipants, SchoolRecord, JudgeID "
                          "FROM Events JOIN Control ON Events.EventsID = Control.EventsID \
                          WHERE Type LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
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

void EventsWindow::on_btnSearch_clicked()//项目搜索
{
    // 根据搜索条件进行查询
    QString eventsID = ui->editEventsID->text();
    QSqlQuery query(e_db);
    QString sql = QString("SELECT Events.EventsID, EventsName, Type, MaxParticipants, SchoolRecord, JudgeID "
                          "FROM Events JOIN Control ON Events.EventsID = Control.EventsID \
                          WHERE Events.EventsID=:EventsID");
    query.prepare(sql);
    query.bindValue(":EventsID", eventsID);
    if (!query.exec()) {
        QMessageBox::critical(this, "错误", "查询失败\n" + query.lastError().text(),
                                    QMessageBox::Ok, QMessageBox::NoButton);
    } else {
        // 更新QSqlQueryModel对象
        qryModel->setQuery(query);
        ui->tableView->setModel(qryModel);
    }
}

void EventsWindow::on_actExportEvent_triggered()//导出项目信息
{
    QString filePath = QFileDialog::getSaveFileName(this, "保存文件", "", "Excel 文件 (*.xlsx)");
    if (filePath.isEmpty())
        return;

    lxw_workbook *workbook = workbook_new(filePath.toUtf8().constData());
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

    // 设置表头
    QStringList headers;
    headers << "项目编号" << "项目名称" << "项目类型" << "最大参与人数" << "比赛纪录" << "裁判编号";
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
