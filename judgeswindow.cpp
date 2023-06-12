#include "judgeswindow.h"
#include "ui_judgeswindow.h"

void JudgesWindow::updateRecord(int recNo)//更新记录
{
    QSqlRecord curRec=qryModel->record(recNo); //获取当前记录
    QString judgeID=curRec.value("JudgeID").toString();

    QSqlQuery query(j_db); //查询出当前记录的所有字段
    query.prepare("SELECT JudgeID, Jname, Jgender, Jage, Jphone "
                  "FROM Judges "
                  "WHERE JudgeID = :ID");
    query.bindValue(":ID", judgeID);
    query.exec();
    query.first();

    if (!query.isValid()) //是否为有效记录
        return;

    curRec=query.record();//获取当前记录的数据

    JudgesDialog *judgesDlg=new JudgesDialog(this);
    Qt::WindowFlags flags=judgesDlg->windowFlags();
    judgesDlg->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //设置对话框固定大小
    judgesDlg->setUpdateRecord(curRec);//调用对话框函数更新数据和界面
    int ret=judgesDlg->exec();// 以模态方式显示对话框

    if (ret == QDialog::Accepted) {
        QSqlRecord recData = judgesDlg->getRecordData();// 获取对话框返回的记录数据

        query.prepare("UPDATE Judges SET Jname=:Jname, Jgender=:Jgender, Jage=:Jage, Jphone=:Jphone WHERE JudgeID=:ID");
        query.bindValue(":Jname", recData.value("Jname"));
        query.bindValue(":Jgender", recData.value("Jgender"));
        query.bindValue(":Jage", recData.value("Jage"));
        query.bindValue(":Jphone", recData.value("Jphone"));
        query.bindValue(":ID", recData.value("JudgeID"));
        if (!query.exec()) {
            QMessageBox::critical(this, "错误", "记录更新错误\n" + query.lastError().text(),
            QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }

        qryModel->query().exec();//数据模型重新查询数据，更新 tableView 显示
    }
    delete judgesDlg;
}

void JudgesWindow::getFieldNames()//获取字段名称,填充“排序字段”的comboBox
{
    QSqlRecord record = qryModel->record();
    for(int i = 0; i < record.count(); i++) {
        QString fieldName = record.fieldName(i);
        ui->comboFilter_2->addItem(fieldName);
    }
}

JudgesWindow::JudgesWindow(QSqlDatabase& db, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::JudgesWindow)
    , j_db(db)
{
    ui->setupUi(this);
//    this->setCentralWidget(ui->tableView);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setAlternatingRowColors(true);

    qryModel=new QSqlQueryModel(this);
    theSelection=new QItemSelectionModel(qryModel);

    QString query = "SELECT JudgeID, Jname, Jgender, Jage, Jphone "
                    "FROM Judges";
    qryModel->setQuery(query);
    if (qryModel->lastError().isValid())
    {
        QMessageBox::information(this, "错误", "数据表查询错误,错误信息\n"+qryModel->lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);
        return;
    }

    qryModel->setHeaderData(0,Qt::Horizontal,"裁判编号");
    qryModel->setHeaderData(1,Qt::Horizontal,"裁判姓名");
    qryModel->setHeaderData(2,Qt::Horizontal,"裁判性别");
    qryModel->setHeaderData(3,Qt::Horizontal,"裁判年龄");
    qryModel->setHeaderData(4,Qt::Horizontal,"裁判电话");

    ui->tableView->setModel(qryModel);
    ui->tableView->setSelectionModel(theSelection);

    getFieldNames();//获取字段名称,填充“排序字段”的comboBox

    if (theSelection->selectedIndexes().isEmpty()) {//无法选中
        ui->actModifyJudge->setEnabled(false);
        ui->actDeleteJudge->setEnabled(false);
    }

    connect(theSelection, &QItemSelectionModel::selectionChanged, this, [=](const QItemSelection &selected, const QItemSelection &deselected){
        if (selected.isEmpty()) {
            ui->actModifyJudge->setEnabled(false);
            ui->actDeleteJudge->setEnabled(false);
        } else {
            ui->actModifyJudge->setEnabled(true);
            ui->actDeleteJudge->setEnabled(true);
        }
    });
}

JudgesWindow::~JudgesWindow()
{
    delete ui;
}

void JudgesWindow::on_actAddJudge_triggered()//添加裁判
{
    QSqlQuery query(j_db);
    query.exec("select * from Judges where JudgeID = -1"); //实际不查询出记录，只查询字段信息
    QSqlRecord curRec=query.record();//获取当前记录,实际为空记录

    JudgesDialog *judgesDlg=new JudgesDialog(this);
    Qt::WindowFlags flags=judgesDlg->windowFlags();
    judgesDlg->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //设置对话框固定大小
    judgesDlg->setAddRecord(curRec);//调用对话框函数更新数据和界面
    int ret=judgesDlg->exec();// 以模态方式显示对话框

    if (ret==QDialog::Accepted){//OK键被按下
        QSqlRecord recData=judgesDlg->getRecordData(); //获得对话框返回的记录

        // 向 Judges 表中插入记录
        query.prepare("INSERT INTO Judges (JudgeID, Jname, Jgender, Jage, Jphone) \
                       VALUES (:JudgeID, :Jname, :Jgender, :Jage, :Jphone)");
        query.bindValue(":JudgeID", recData.value("JudgeID"));
        query.bindValue(":Jname", recData.value("Jname"));
        query.bindValue(":Jgender", recData.value("Jgender"));
        query.bindValue(":Jage", recData.value("Jage"));
        query.bindValue(":Jphone", recData.value("Jphone"));
        if (!query.exec())
            QMessageBox::critical(this, "错误", "添加选手错误\n" + query.lastError().text(),
                                    QMessageBox::Ok, QMessageBox::NoButton);

        // 插入、删除记录后需要重新设置SQL语句查询
        qryModel->setQuery(qryModel->query().executedQuery()); // 重新查询数据
    }
    delete judgesDlg;
}

void JudgesWindow::on_actModifyJudge_triggered()//修改裁判
{
    int curRecNo=theSelection->currentIndex().row();
    updateRecord(curRecNo);
}

void JudgesWindow::on_tableView_doubleClicked(const QModelIndex &index)//tableView上双击,编辑当前记录
{
    int curRecNo=index.row();
    updateRecord(curRecNo);
}

void JudgesWindow::on_actDeleteJudge_triggered()//删除裁判
{
    int curRecNo=theSelection->currentIndex().row();
    QSqlRecord curRec=qryModel->record(curRecNo); //获取当前记录
    if (curRec.isEmpty()) //当前为空记录
        return;

    QSqlQuery query(j_db); //查询出当前记录的所有字段
    // 删除 Judges 表中的记录
    query.prepare("DELETE FROM Judges WHERE JudgeID = ?");
    query.addBindValue(curRec.value("JudgeID"));
    if (!query.exec())
        QMessageBox::critical(this, "错误", "删除记录错误\n" + query.lastError().text(),
        QMessageBox::Ok, QMessageBox::NoButton);

    // 删除完记录后需要重新设置 SQL 语句查询
    qryModel->setQuery(qryModel->query().executedQuery()); // 重新查询数据
}

void JudgesWindow::on_rBtnAscend_2_clicked()//升序
{
    //使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(j_db);
    QString fieldName = ui->comboFilter_2->currentText();
    QString sortOrder = "ASC"; // 默认升序排列
    QString sql = QString("SELECT JudgeID, Jname, Jgender, Jage, Jphone "
                          "FROM Judges "
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

void JudgesWindow::on_rBtnDescend_2_clicked()//降序
{
    //使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(j_db);
    QString fieldName = ui->comboFilter_2->currentText();
    QString sortOrder = "DESC"; // 默认升序排列
    QString sql = QString("SELECT JudgeID, Jname, Jgender, Jage, Jphone "
                          "FROM Judges "
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

void JudgesWindow::on_comboFilter_2_currentIndexChanged(int index)//选择字段进行排序
{
    Q_UNUSED(index);
    //获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend_2->isChecked())//如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    //使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(j_db);
    QString fieldName = ui->comboFilter_2->currentText();
    QString sql = QString("SELECT JudgeID, Jname, Jgender, Jage, Jphone "
                          "FROM Judges "
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

void JudgesWindow::on_rBtnMan_2_clicked()//男
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend_2->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(j_db);
    QString fieldName = ui->comboFilter_2->currentText();
    QString genderFilter = "男";
    QString sql = QString("SELECT JudgeID, Jname, Jgender, Jage, Jphone "
                          "FROM Judges \
                          WHERE Jgender LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
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

void JudgesWindow::on_rBtnWoman_2_clicked()//女
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend_2->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(j_db);
    QString fieldName = ui->comboFilter_2->currentText();
    QString genderFilter = "女";
    QString sql = QString("SELECT JudgeID, Jname, Jgender, Jage, Jphone "
                          "FROM Judges \
                          WHERE Jgender LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
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

void JudgesWindow::on_rBtnBoth_2_clicked()//全显示
{
    // 获取用户选择的排序规则（升序或降序）
    QString sortOrder = "ASC"; // 默认升序排列
    if (ui->rBtnDescend_2->isChecked())// 如果选择了降序，则修改排序方式
        sortOrder = "DESC";

    // 使用 QSqlQuery 对象查询数据，并指定排序规则
    QSqlQuery query(j_db);
    QString fieldName = ui->comboFilter_2->currentText();
    QString genderFilter = "";
    QString sql = QString("SELECT JudgeID, Jname, Jgender, Jage, Jphone "
                          "FROM Judges \
                          WHERE Jgender LIKE '%%1%' ORDER BY %2 %3").arg(genderFilter).arg(fieldName).arg(sortOrder);
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

void JudgesWindow::on_btnSearch_2_clicked()//裁判搜索
{
    //根据搜索条件进行查询
    QString judgeID = ui->editJudgeID->text();
    QSqlQuery query(j_db);
    QString sql = QString("SELECT JudgeID, Jname, Jgender, Jage, Jphone "
                          "FROM Judges \
                          WHERE JudgeID = :JudgeID");
    query.prepare(sql);
    query.bindValue(":JudgeID", judgeID);
    if (!query.exec()) {
        QMessageBox::critical(this, "错误", "查询失败\n" + query.lastError().text(),
                                    QMessageBox::Ok, QMessageBox::NoButton);
    } else {
        // 更新QSqlQueryModel对象
        qryModel->setQuery(query);
        ui->tableView->setModel(qryModel);
    }
}
