#include "arrangewindow.h"
#include "ui_arrangewindow.h"

void ArrangeWindow::updateRecord(int recNo)//更新记录
{
    QSqlRecord curRec=qryModel->record(recNo); //获取当前记录
    QString eventsID=curRec.value("EventsID").toString();

    QSqlQuery query(a_db); //查询出当前记录的所有字段
    query.prepare("SELECT EventsID, Status, Time, Place "
                  "FROM Arrange "
                  "WHERE EventsID = :ID");
    query.bindValue(":ID", eventsID);
    query.exec();
    query.first();

    if (!query.isValid()) //是否为有效记录
        return;

     curRec=query.record();//获取当前记录的数据

     ArrangeDialog *arrangeDlg=new ArrangeDialog(this);
     Qt::WindowFlags flags=arrangeDlg->windowFlags();
     arrangeDlg->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //设置对话框固定大小
     arrangeDlg->setUpdateRecord(curRec);//调用对话框函数更新数据和界面
     int ret=arrangeDlg->exec();// 以模态方式显示对话框

     if (ret == QDialog::Accepted) {
         QSqlRecord recData = arrangeDlg->getRecordData();// 获取对话框返回的记录数据
         query.prepare("UPDATE Arrange SET Time=:Time, Place=:Place WHERE EventsID=:EventsID AND Status=:Status");
         query.bindValue(":Status", recData.value("Status"));
         query.bindValue(":Time", recData.value("Time"));
         query.bindValue(":Place", recData.value("Place"));
         query.bindValue(":EventsID", recData.value("EventsID"));
         if (!query.exec()) {
             QMessageBox::critical(this, "错误", "记录更新错误\n" + query.lastError().text(),
             QMessageBox::Ok, QMessageBox::NoButton);
             return;
         }

        qryModel->query().exec();//数据模型重新查询数据，更新 tableView 显示
    }
     delete arrangeDlg;
}

void ArrangeWindow::getFieldNames()//获取字段名称,填充“排序字段”的comboBox
{
    QSqlRecord record = qryModel->record();
    for(int i = 0; i < record.count(); i++) {
        QString fieldName = record.fieldName(i);
        ui->comboFilter->addItem(fieldName);
    }
}

ArrangeWindow::ArrangeWindow(QSqlDatabase& db, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ArrangeWindow)
    , a_db(db)
{
    ui->setupUi(this);
//    this->setCentralWidget(ui->tableView);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setAlternatingRowColors(true);

    qryModel=new QSqlQueryModel(this);
    theSelection=new QItemSelectionModel(qryModel);

    QString query = "SELECT EventsID, Status, Time, Place FROM Arrange";
    qryModel->setQuery(query);
    if (qryModel->lastError().isValid())
    {
        QMessageBox::information(this, "错误", "数据表查询错误,错误信息\n"+qryModel->lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);
        return;
    }

    qryModel->setHeaderData(0,Qt::Horizontal,"项目编号");
    qryModel->setHeaderData(1,Qt::Horizontal,"比赛状态");
    qryModel->setHeaderData(2,Qt::Horizontal,"比赛时间");
    qryModel->setHeaderData(3,Qt::Horizontal,"比赛地点");

    ui->tableView->setModel(qryModel);
    ui->tableView->setSelectionModel(theSelection);

    getFieldNames();//获取字段名称,填充“排序字段”的comboBox

    if (theSelection->selectedIndexes().isEmpty()) {//无法选中
        ui->actModifyArrange->setEnabled(false);
        ui->actDeleteArrange->setEnabled(false);
    }

    connect(theSelection, &QItemSelectionModel::selectionChanged, this, [=](const QItemSelection &selected, const QItemSelection &deselected){
        if (selected.isEmpty()) {
            ui->actModifyArrange->setEnabled(false);
            ui->actDeleteArrange->setEnabled(false);
        } else {
            ui->actModifyArrange->setEnabled(true);
            ui->actDeleteArrange->setEnabled(true);
        }
    });
}

ArrangeWindow::~ArrangeWindow()
{
    delete ui;
}

void ArrangeWindow::on_actAddArrange_triggered()//添加赛程
{
    QSqlQuery query(a_db);
    query.exec("select * from Arrange where EventsID = -1"); //实际不查询出记录，只查询字段信息
    QSqlRecord curRec=query.record();//获取当前记录,实际为空记录

    ArrangeDialog *arrangeDlg=new ArrangeDialog(this);
    Qt::WindowFlags flags=arrangeDlg->windowFlags();
    arrangeDlg->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //设置对话框固定大小
    arrangeDlg->setAddRecord(curRec);//调用对话框函数更新数据和界面
    int ret=arrangeDlg->exec();// 以模态方式显示对话框

    if (ret==QDialog::Accepted){//OK键被按下
        QSqlRecord recData=arrangeDlg->getRecordData(); //获得对话框返回的记录

        // 向 Arrange 表中插入记录
        query.prepare("INSERT INTO Arrange (EventsID, Status, Time, Place) \
                       VALUES (:EventsID, :Status, :Time, :Place)");
        query.bindValue(":EventsID", recData.value("EventsID"));
        query.bindValue(":Status", recData.value("Status"));
        query.bindValue(":Time", recData.value("Time"));
        query.bindValue(":Place", recData.value("Place"));
        if (!query.exec())
            QMessageBox::critical(this, "错误", "添加安排错误\n" + query.lastError().text(),
                                    QMessageBox::Ok, QMessageBox::NoButton);

        // 插入、删除记录后需要重新设置SQL语句查询
        qryModel->setQuery(qryModel->query().executedQuery()); // 重新查询数据
    }
    delete arrangeDlg;
}

void ArrangeWindow::on_actModifyArrange_triggered()//修改赛程
{
    int curRecNo=theSelection->currentIndex().row();
    updateRecord(curRecNo);
}

void ArrangeWindow::on_tableView_doubleClicked(const QModelIndex &index)//tableView上双击,编辑当前记录
{
    int curRecNo=index.row();
    updateRecord(curRecNo);
}

void ArrangeWindow::on_actDeleteArrange_triggered()//删除赛程
{
    int curRecNo=theSelection->currentIndex().row();
    QSqlRecord curRec=qryModel->record(curRecNo); //获取当前记录
    if (curRec.isEmpty()) //当前为空记录
        return;

    QSqlQuery query(a_db); //查询出当前记录的所有字段
    // 删除 Judges 表中的记录
    query.prepare("DELETE FROM Arrange WHERE EventsID = ? AND Status = ?");
    query.addBindValue(curRec.value("EventsID"));
    query.addBindValue(curRec.value("Status"));
    if (!query.exec())
        QMessageBox::critical(this, "错误", "删除记录错误\n" + query.lastError().text(),
        QMessageBox::Ok, QMessageBox::NoButton);

    // 删除完记录后需要重新设置 SQL 语句查询
    qryModel->setQuery(qryModel->query().executedQuery()); // 重新查询数据
}

void ArrangeWindow::on_rBtnAscend_clicked()//升序
{
    //使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(a_db);
    QString fieldName = ui->comboFilter->currentText();
    QString sortOrder = "ASC"; // 默认升序排列
    QString sql = QString("SELECT EventsID, Status, Time, Place FROM Arrange "
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

void ArrangeWindow::on_rBtnDescend_clicked()//降序
{
    //使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(a_db);
    QString fieldName = ui->comboFilter->currentText();
    QString sortOrder = "DESC"; // 默认升序排列
    QString sql = QString("SELECT EventsID, Status, Time, Place FROM Arrange "
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

void ArrangeWindow::on_comboFilter_currentIndexChanged(int index)//选择字段进行排序
{
    Q_UNUSED(index);
    //获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())//如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    //使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(a_db);
    QString fieldName = ui->comboFilter->currentText();
    QString sql = QString("SELECT EventsID, Status, Time, Place FROM Arrange "
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

void ArrangeWindow::on_rBtnP_clicked()//初赛P
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(a_db);
    QString fieldName = ui->comboFilter->currentText();
    QString genderFilter = "P";
    QString sql = QString("SELECT EventsID, Status, Time, Place FROM Arrange \
                          WHERE Status LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
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

void ArrangeWindow::on_rBtnR_clicked()//复赛R
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(a_db);
    QString fieldName = ui->comboFilter->currentText();
    QString genderFilter = "R";
    QString sql = QString("SELECT EventsID, Status, Time, Place FROM Arrange \
                          WHERE Status LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
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

void ArrangeWindow::on_rBtnF_clicked()//决赛F
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(a_db);
    QString fieldName = ui->comboFilter->currentText();
    QString genderFilter = "F";
    QString sql = QString("SELECT EventsID, Status, Time, Place FROM Arrange \
                          WHERE Status LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
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

void ArrangeWindow::on_rBtnBoth_clicked()//全显示
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(a_db);
    QString fieldName = ui->comboFilter->currentText();
    QString genderFilter = "";
    QString sql = QString("SELECT EventsID, Status, Time, Place FROM Arrange \
                          WHERE Status LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
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

void ArrangeWindow::on_btnSearch_clicked()//项目搜索
{
    // 根据搜索条件进行查询
    QString eventsID = ui->editEventsID->text();
    QSqlQuery query(a_db);
    QString sql = QString("SELECT EventsID, Status, Time, Place FROM Arrange \
                          WHERE EventsID=:EventsID");
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

void ArrangeWindow::on_actExportArrange_triggered()//导出赛程信息
{
    QString filePath = QFileDialog::getSaveFileName(this, "保存文件", "", "Excel 文件 (*.xlsx)");
    if (filePath.isEmpty())
        return;

    lxw_workbook *workbook = workbook_new(filePath.toUtf8().constData());
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

    // 设置表头
    QStringList headers;
    headers << "赛程编号" << "项目编号" << "开始时间" << "结束时间" << "地点";
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
