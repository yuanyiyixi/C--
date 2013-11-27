#include "myform.h"
#include <QMessageBox>

MyWidget::MyWidget(QWidget *parent)
    :QWidget(parent)
{
    setupUi(this);//创建窗体上所有子对象
    connect(btn_quit, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(btn_print, SIGNAL(clicked()), this, SLOT(myslot()));
}

void MyWidget::myslot()
{
    QString gender;
    if (rbtn_male->isChecked())
        gender = QString::fromUtf8("男");
    else if(rbtn_female->isChecked())
        gender = QString::fromUtf8("女");

    QString msg;
    msg = "Name: " + let_name->text() + "\n" +
           "Adddress: " + let_adr->text() + "\n" +
           "Gender: " + gender;
    QMessageBox::information(this, "user info", msg);
}

void MyWidget::on_btn_out_clicked()
{

}
