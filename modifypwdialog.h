#ifndef MODIFYPWDIALOG_H
#define MODIFYPWDIALOG_H

#include <QDialog>
#include<QSettings>
#include<QMessageBox>
#include<QByteArray>
#include<QCryptographicHash>
#include<QDebug>
#include"qdlglogin.h"

namespace Ui {
class ModifyPWDialog;
}

class ModifyPWDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyPWDialog(QWidget *parent = 0);
    ~ModifyPWDialog();

    QString mo_encrypt(const QString &str);//加密

private slots:
    void on_btnOK_clicked();//确定

private:
    Ui::ModifyPWDialog *ui;
};

#endif // MODIFYPWDIALOG_H
