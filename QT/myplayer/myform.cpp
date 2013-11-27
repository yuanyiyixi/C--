#include "myform.h"
#include <QDebug>
#include <QTimer>

MyForm::MyForm(QWidget *parent) :
    QWidget(parent)
{
    flag = 0;
    timer = new QTimer(this);
    parent_win = dynamic_cast<MyPlayer*>(parent);
    connect(timer, SIGNAL(timeout()), this, SLOT(mouseClick()));
}

void MyForm::mouseDoubleClickEvent(QMouseEvent *)
{
    qDebug() << "Double Clicked";
    timer->stop();
    //恢复原来大小
    //parent_win->setGeometry(0, 0, 600, 450);
    if(flag == 0)
    {
        flag = 1;
        parent_win->showFullScreen();
       this->setGeometry(0,0,parent_win->width(),parent_win->height());
        parent_win->list->hide();
    }
    else if(flag == 1)
    {
        flag = 0;
    parent_win->showNormal();
   this->setGeometry(0, 0, width() - parent_win->list->width() - 20, parent_win->height() - 82);
   //this->setGeometry(0, 0, width() - parent_win->list->width() - 20, parent_win->height() - 82);
   parent_win->list->show();
    }
}

void MyForm::mousePressEvent(QMouseEvent *event)
{
    //if (event->button() == 0x00000001)
    if (event->button() == Qt::LeftButton)
        timer->start(200);
}

void MyForm::mouseClick()
{
    parent_win->do_pause();
    timer->stop();
}
