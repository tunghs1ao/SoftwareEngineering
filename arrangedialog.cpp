#include "arrangedialog.h"
#include "ui_arrangedialog.h"

ArrangeDialog::ArrangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArrangeDialog)
{
    ui->setupUi(this);
}

ArrangeDialog::~ArrangeDialog()
{
    delete ui;
}

void ArrangeDialog::setUpdateRecord(QSqlRecord &recData)//更新记录
{
    mRecord=recData;
    setWindowTitle("更新记录");

    ui->editEventsID->setText(recData.value("EventsID").toString());
    ui->comboStatus->setCurrentText(recData.value("Status").toString());
    ui->dateTimeEdit->setDateTime(recData.value("Time").toDateTime());
    ui->editPlace->setText(recData.value("Place").toString());
}

void ArrangeDialog::setAddRecord(QSqlRecord &recData)//添加记录
{
    mRecord=recData; //保存recData到内部变量
    setWindowTitle("添加记录");
}

QSqlRecord ArrangeDialog::getRecordData()//获取录入的数据
{
    mRecord.setValue("EventsID", ui->editEventsID->text());
    mRecord.setValue("Status", ui->comboStatus->currentText());
    mRecord.setValue("Time", ui->dateTimeEdit->dateTime());
    mRecord.setValue("Place", ui->editPlace->text());

    return mRecord;//以记录作为返回值
}
