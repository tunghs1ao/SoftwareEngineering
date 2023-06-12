#include "playerdialog.h"
#include "ui_playerdialog.h"

PlayerDialog::PlayerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayerDialog)
{
    ui->setupUi(this);
}

PlayerDialog::~PlayerDialog()
{
    delete ui;
}

void PlayerDialog::setUpdateRecord(QSqlRecord &recData)//更新记录
{
    mRecord=recData;
    ui->editStudentID->setEnabled(false);
    setWindowTitle("更新记录");

    ui->editStudentID->setText(recData.value("StudentID").toString());
    ui->editName->setText(recData.value("Name").toString());
    ui->comboGender->setCurrentText(recData.value("Gender").toString());
    ui->spinAge->setValue(recData.value("Age").toInt());
    ui->comboCollegeID->setCurrentText(recData.value("CollegeID").toString());
    ui->editMajor->setText(recData.value("Major").toString());
    ui->comboClass->setCurrentText(recData.value("Class").toString());
    ui->editPhone->setText(recData.value("phone").toString());
    ui->editEventsID->setText(recData.value("EventsID").toString());
    ui->editTeam->setText(recData.value("Team").toString());
}

void PlayerDialog::setAddRecord(QSqlRecord &recData)//添加记录
{//无需更新界面显示，但是要存储recData的字段结构
    mRecord=recData; //保存recData到内部变量
    ui->editStudentID->setEnabled(true);
    setWindowTitle("添加记录");
}

QSqlRecord PlayerDialog::getRecordData()//获取录入的数据
{
    mRecord.setValue("StudentID",ui->editStudentID->text().toInt());
    mRecord.setValue("Name",ui->editName->text());
    mRecord.setValue("Gender",ui->comboGender->currentText());
    mRecord.setValue("Age",ui->spinAge->value());
    mRecord.setValue("CollegeID",ui->comboCollegeID->currentText());
    mRecord.setValue("Major",ui->editMajor->text());
    mRecord.setValue("Class",ui->comboClass->currentText());
    mRecord.setValue("phone",ui->editPhone->text());
    mRecord.setValue("EventsID",ui->editEventsID->text());
    mRecord.setValue("Team",ui->editTeam->text());

    return mRecord; //以记录作为返回值
}
