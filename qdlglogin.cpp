#include "qdlglogin.h"
#include "ui_qdlglogin.h"

QDlgLogin::QDlgLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QDlgLogin)
{
    ui->setupUi(this);
    setFixedSize(600,350);

    ui->editPassword->setEchoMode(QLineEdit::Password);//密码输入模式
    this->setAttribute(Qt::WA_DeleteOnClose);//关闭时删除
    this->setWindowFlags(Qt::SplashScreen);//窗口无边框,不在任务栏显示

    readSetting();//从注册表读取设置
}

QDlgLogin::~QDlgLogin()
{
    delete ui;
}

void QDlgLogin::readSetting(){//从注册表读取设置
    QString organization="WWB-Qt";
    QString appName="qt-project-SpoCompeManaSys";
    QSettings settings(organization,appName);

    bool saved=settings.value("saved",false).toBool();//读取saved键的值
    //1
    myAccount=settings.value("UserAccount","user").toString();//Account缺省
    QString defaultPassword=encrypt("123");//缺省密码加密
    myPassword=settings.value("UserPassword",defaultPassword).toString();//Password缺省
    //2
    myAccount1=settings.value("UserAccount1","user1").toString();//Account缺省
    defaultPassword=encrypt("1234");//缺省密码加密
    myPassword1=settings.value("UserPassword1",defaultPassword).toString();//Password缺省

    if(saved)
        ui->editAccount->setText(myAccount);

    ui->chkBoxSave->setChecked(saved);
}

void QDlgLogin::writeSetting(){//从注册表写入设置
    QSettings settings("WWB-Qt","qt-project-SpoCompeManaSys");//注册表键组
    settings.setValue("Account",myAccount);
    settings.setValue("Password",myPassword);
    settings.setValue("Account1",myAccount1);
    settings.setValue("Password1",myPassword1);
    settings.setValue("saved",ui->chkBoxSave->isChecked());
}

QString QDlgLogin::encrypt(const QString &str){//加密
    QByteArray btArray;
    btArray.append(str);
    QCryptographicHash hash(QCryptographicHash::Md5);//Md5加密算法
    hash.addData(btArray);//添加数据到加密hash值
    QByteArray resultArray=hash.result();//返回最终的哈希值
    QString md5=resultArray.toHex();//转换为16进制字符串

    return md5;
}

void QDlgLogin::on_btnLogin_clicked()//Login
{
    QString account=ui->editAccount->text().trimmed();
    QString password=ui->editPassword->text().trimmed();
    QString encryptPassword=encrypt(password);//对输入密码进行加密

    if((account==myAccount)&&(encryptPassword==myPassword)){
        myAccount = account; // 选择第一组帐号
        writeSetting();//保存设置
        this->accept();
    }
    else if((account==myAccount1)&&(encryptPassword==myPassword1)){
        myAccount1 = account; // 选择第二组帐号
        writeSetting();//保存设置
        this->accept();
    }
    else{
        ++tryCount;
        if(tryCount<=3)
            QMessageBox::warning(this,"错误提示","帐号或密码错误");
        else{
            QMessageBox::critical(this,"错误","输入错误次数太多，强行退出");
            this->reject();
        }
    }
}

void QDlgLogin::mousePressEvent(QMouseEvent *event){//Press
    if(event->button()==Qt::LeftButton){
        moving=true;
        lastPos=event->globalPos()-pos();
    }
    return QDialog::mousePressEvent(event);
}

void QDlgLogin::mouseMoveEvent(QMouseEvent *event){//Move
    //通过事件event->globalPos()知道鼠标坐标，鼠标坐标减去鼠标相对于窗口位置，就是窗口在整个屏幕的坐标
    if(moving&&(event->buttons()&&Qt::LeftButton)&&(event->globalPos()-lastPos).manhattanLength()
            >QApplication::startDragDistance()){
        move(event->globalPos()-lastPos);
        lastPos=event->globalPos()-pos();
    }
    return QDialog::mouseMoveEvent(event);
}

void QDlgLogin::mouseReleaseEvent(QMouseEvent *event){//Release
    Q_UNUSED(event);
    moving=false;
}

void QDlgLogin::on_btnChange_clicked()//修改密码
{
    ModifyPWDialog *modifyPWDlg=new ModifyPWDialog(this);
    Qt::WindowFlags flags=modifyPWDlg->windowFlags();
    modifyPWDlg->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //设置对话框固定大小

    int ret=modifyPWDlg->exec();// 以模态方式显示对话框
    if (ret==QDialog::Accepted)//OK键被按下
        delete modifyPWDlg;
}
