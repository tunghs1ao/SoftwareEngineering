#include "judgesdialog.h"
#include "ui_judgesdialog.h"

JudgesDialog::JudgesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JudgesDialog)
{
    ui->setupUi(this);
}

JudgesDialog::~JudgesDialog()
{
    delete ui;
}

void JudgesDialog::setUpdateRecord(QSqlRecord &recData)//更新记录
{
    mRecord=recData;
    ui->editJudgeID->setEnabled(false);
    setWindowTitle("更新记录");

    ui->editJudgeID->setText(recData.value("JudgeID").toString());
    ui->editJname->setText(recData.value("Jname").toString());
    ui->comboJgender->setCurrentText(recData.value("Jgender").toString());
    ui->editJage->setText(recData.value("Jage").toString());
    ui->editJphone->setText(recData.value("Jphone").toString());
}

void JudgesDialog::setAddRecord(QSqlRecord &recData)//添加记录
{
    mRecord=recData; //保存recData到内部变量
    ui->editJudgeID->setEnabled(true);
    setWindowTitle("添加记录");
}

QSqlRecord JudgesDialog::getRecordData()//获取录入的数据
{
    mRecord.setValue("JudgeID",ui->editJudgeID->text());
    mRecord.setValue("Jname",ui->editJname->text());
    mRecord.setValue("Jgender",ui->comboJgender->currentText());
    mRecord.setValue("Jage",ui->editJage->text().toInt());
    mRecord.setValue("Jphone",ui->editJphone->text());

    return mRecord;//以记录作为返回值
}
