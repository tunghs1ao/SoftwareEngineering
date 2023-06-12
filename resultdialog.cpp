#include "resultdialog.h"
#include "ui_resultdialog.h"

ResultDialog::ResultDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultDialog)
{
    ui->setupUi(this);
}

ResultDialog::~ResultDialog()
{
    delete ui;
}

void ResultDialog::setUpdateRecord(QSqlRecord &recData)//更新记录
{
    mRecord=recData;
    setWindowTitle("编辑记录");

    ui->editEventsID->setEnabled(false);
    ui->editStatus->setEnabled(false);
    ui->editStudentID->setEnabled(false);
    ui->editName->setEnabled(false);

    ui->editEventsID->setText(recData.value("EventsID").toString());
    ui->editStatus->setText(recData.value("Status").toString());
    ui->editStudentID->setText(recData.value("StudentID").toString());
    ui->editName->setText(recData.value("Name").toString());
    ui->editScore->setText(recData.value("Score").toString());
}

QSqlRecord ResultDialog::getRecordData()//获取录入的数据
{
    mRecord.setValue("Score",ui->editScore->text().toFloat());

    return mRecord;//以记录作为返回值
}

