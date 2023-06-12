#include "modifypwdialog.h"
#include "ui_modifypwdialog.h"

ModifyPWDialog::ModifyPWDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModifyPWDialog)
{
    ui->setupUi(this);

    setWindowTitle("修改密码");

    ui->editMoPassword->setEchoMode(QLineEdit::Password);//密码输入模式
    ui->editNewPW->setEchoMode(QLineEdit::Password);
    ui->editCheckNewPW->setEchoMode(QLineEdit::Password);
}

ModifyPWDialog::~ModifyPWDialog()
{
    delete ui;
}

QString ModifyPWDialog::ModifyPWDialog::mo_encrypt(const QString &str)//加密
{
    QByteArray btArray;
    btArray.append(str);
    QCryptographicHash hash(QCryptographicHash::Md5);//Md5加密算法
    hash.addData(btArray);//添加数据到加密hash值
    QByteArray resultArray=hash.result();//返回最终的哈希值
    QString md5=resultArray.toHex();//转换为16进制字符串

    return md5;
}

void ModifyPWDialog::on_btnOK_clicked()//确定
{
    QString moAccount = ui->editMoAccount->text().trimmed();
    QString moPassword = ui->editMoPassword->text().trimmed();
    QString moEncryptedPassword = mo_encrypt(moPassword); // 对密码进行加密
    QString newPW=ui->editNewPW->text().trimmed();
    QString checkPW=ui->editCheckNewPW->text().trimmed();

    // 从注册表读取所有帐号密码
    QString organization = "WWB-Qt";
    QString appName = "qt-project-SpoCompeManaSys";
    QSettings settings(organization, appName);

    QString myAccount=settings.value("UserAccount","user").toString();//Account缺省
    QString defaultPassword=mo_encrypt("123");//缺省密码加密
    QString myPassword=settings.value("UserPassword",defaultPassword).toString();//Password缺省
    //2
    QString myAccount1=settings.value("UserAccount1","user1").toString();//Account缺省
    defaultPassword=mo_encrypt("1234");//缺省密码加密
    QString myPassword1=settings.value("UserPassword1",defaultPassword).toString();//Password缺省

    if ((moAccount == myAccount && moEncryptedPassword == myPassword) ||\
        (moAccount == myAccount1 && moEncryptedPassword == myPassword1)){
        if(!newPW.isEmpty()&&newPW==checkPW){
            // 根据moAccount的值来确定要更新的帐号和密码
            if (moAccount == myAccount) {
                // 生成新的键名
                QString newKey = "UserAccount_" + QString::number(settings.allKeys().count());
                // 将新的帐号和密码写入注册表
                settings.setValue(newKey, moAccount);
                settings.setValue("UserPassword", mo_encrypt(newPW));
            } else if (moAccount == myAccount1) {
                // 生成新的键名
                QString newKey = "UserAccount1_" + QString::number(settings.allKeys().count());
                // 将新的帐号和密码写入注册表
                settings.setValue(newKey, moAccount);
                settings.setValue("UserPassword1", mo_encrypt(newPW));
            }

            QMessageBox::information(this, "提示", "密码修改成功!");

            QDialog::accept();
        }
        else
            QMessageBox::warning(this, "提示", "密码不一致");
    }
    else
        QMessageBox::warning(this, "提示", "帐号或密码错误");\
}
