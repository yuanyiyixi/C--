#include "mypic.h"
#include <QPixmap>//像素图
#include <QDebug>
#include <QDir>//目录类

#define FILEPATH "./files"

MyPic::MyPic(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);//this是子控件的父对象

    lbl_pic->setScaledContents(true);//设置显示内容按比例缩放
    //自动遍历目录，把图片文件名形成一个链表
    QDir dir(FILEPATH);
    filenames = dir.entryList(QStringList() << "*.jpg" << "*.png", QDir::Files | QDir::NoDotAndDotDot);
    //filenames << ":/images/files/1.jpg" << ":/images/files/2.jpg" << ":/images/files/3.jpg" << ":/images/files/4.jpg";
    pics = new QPixmap[filenames.size()];
    for (int i = 0; i < filenames.size(); ++i)
    {
       filenames[i] = QString(FILEPATH) + "/" + filenames[i];
    qDebug() << "filename: " << filenames[i];
}
    for (int i = 0; i < filenames.size(); ++i)
     pics[i].load(filenames[i]);
      // qDebug() << "filename: " << filenames[i];

   //QPixmap pic1(":/images/files/1.jpg");
    lbl_pic->setPixmap(pics[0]);
    connect(btn_next, SIGNAL(clicked()), this, SLOT(myslot()));
}

void MyPic::myslot()
{
    static int i = 1;
    //QPixmap pic(filenames[i%4]);

    lbl_pic->setPixmap(pics[i%filenames.size()]);
    i++;

}
