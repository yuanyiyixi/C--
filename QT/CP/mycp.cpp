#include "mycp.h"
#include <QFile>
#include <QMessageBox>
#include <QDebug>

MyCp::MyCp(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    proBar->setValue(0);
    connect(btn_copy, SIGNAL(clicked()), this, SLOT(myslot()));
    connect(btn_quit, SIGNAL(clicked()), qApp, SLOT(quit()));
}
void MyCp::myslot()
{
    QFile f1(let_src->text());
    QFile f2(let_dest->text());
    if (f1.open(QFile::ReadOnly) && f2.open(QFile::WriteOnly | QFile::Truncate))
    { 
        char buf[1024];
        int n;
        proBar->setRange(0, f1.size());

        int Size = 0;
        while(!f1.atEnd())
        {
            n = f1.read(buf, 1024);
             Size += n;
            f2.write(buf, n);
            proBar->setValue(Size);
        }
    }
    else
    {
        QMessageBox::information(this, "message", "copy faile!");
        return ;
    }
    f1.close();
    f2.close();
    QMessageBox::information(this, "message", "copy successful!");
    proBar->setValue(0);
    let_src->setText("");
    let_dest->setText("");
}
