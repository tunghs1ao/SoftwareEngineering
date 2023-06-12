#include "eventsdialog.h"
#include "ui_eventsdialog.h"

EventsDialog::EventsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventsDialog)
{
    ui->setupUi(this);
}

EventsDialog::~EventsDialog()
{
    delete ui;
}

void EventsDialog::setUpdateRecord(QSqlRecord &recData)//更新记录
{
    mRecord=recData;
    ui->editEventsID->setEnabled(false);
    setWindowTitle("更新记录");

    ui->editEventsID->setText(recData.value("EventsID").toString());
    ui->editEventsName->setText(recData.value("EventsName").toString());
    ui->comboType->setCurrentText(recData.value("Type").toString());
    ui->editMaxParticipants->setText(recData.value("MaxParticipants").toString());
    ui->editSchoolRecord->setText(recData.value("SchoolRecord").toString());
    ui->editJudgeID->setText(recData.value("JudgeID").toString());
}

void EventsDialog::setAddRecord(QSqlRecord &recData)//添加记录
{
    mRecord=recData; //保存recData到内部变量
    ui->editEventsID->setEnabled(true);
    setWindowTitle("添加记录");
}

QSqlRecord EventsDialog::getRecordData()//获取录入的数据
{
    mRecord.setValue("EventsID",ui->editEventsID->text());
    mRecord.setValue("EventsName",ui->editEventsName->text());
    mRecord.setValue("Type",ui->comboType->currentText());
    mRecord.setValue("MaxParticipants",ui->editMaxParticipants->text().toInt());
    mRecord.setValue("SchoolRecord",ui->editSchoolRecord->text().toFloat());
    mRecord.setValue("JudgeID",ui->editJudgeID->text());

    qDebug()<<"JudgeID:"<<mRecord.value("JudgeID");//添加调试输出

    return mRecord;//以记录作为返回值
}
