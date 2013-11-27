#include "myform.h"
#include <QMessageBox>

MyWidget::MyWidget(QWidget *parent)
    :QWidget(parent), ui(new Ui::Form)
{
    ui->setupUi(this);//创建窗体上所有子对象
    connect(ui->btn_quit, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(ui->btn_print, SIGNAL(clicked()), this, SLOT(myslot()));
    QButtonGroup *grp1 = new QButtonGroup(this);
    QButtonGroup *grp2 = new QButtonGroup(this);
    grp1->addButton(ui->rbtn_male);
    grp1->addButton(ui->rbtn_female);
    grp2->addButton(ui->rbtn_zr);
    grp2->addButton(ui->rbtn_ky);

}

void MyWidget::myslot()
{
    QString gender;
    if (ui->rbtn_male->isChecked())
        gender = QString::fromUtf8("男");
    else if(ui->rbtn_female->isChecked())
        gender = QString::fromUtf8("女");

    QString msg;
    msg = "Name: " + ui->let_name->text() + "\n" +
           "Adddress: " + ui->let_adr->text() + "\n" +
           "Gender: " + gender;
    QMessageBox::information(this, "user info", msg);
}

void MyWidget::on_btn_out_clicked()
{
    QString gender;
    if (ui->rbtn_male->isChecked())
        gender = QString::fromUtf8("男");
    else if(ui->rbtn_female->isChecked())
        gender = QString::fromUtf8("女");

    QString msg;
    msg = "Name: " + ui->let_name->text() + "\n" +
           "Adddress: " + ui->let_adr->text() + "\n" +
           "Gender: " + gender;
    QMessageBox::information(this, "user info", msg);
}
