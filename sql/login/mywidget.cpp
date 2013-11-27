#include "mywidget.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    connect(btn_login, SIGNAL(clicked()), this, SLOT(do_login()));

}
void MyWidget::do_login()
{

#if 1
    QSqlQuery query;

    //有占位符要用prepare
    query.prepare("select * from user where name=:name and passward=:passward");
    query.bindValue(0, let_name->text());
    query.bindValue(1, let_passward->text());
#else
    QSqlQuery query("select * from user where name = 'akaedu' and passward = 'akaedu'");
    while(query.next())
    {
        qDebug() << query.value(0).toInt() << ","
                << query.value(1).toString() << ","
                << query.value(2).toString();
    }
#endif
    query.exec();
    qDebug() << "query.size = " << query.size();
    //qDebug() << query.next();是错误的

    if (query.next())//返回记录条数
    {
        QMessageBox::information(this, tr("login"), "login successfully!");
    }
    else
    {
        QMessageBox::information(this, tr("login"), "login failed!");
    }
}
