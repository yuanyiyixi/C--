#include "mytime.h"
#include <QTime>//时间类
#include <QTimer>
#include <QDebug>
#include <QPalette>//调色板类
#include <QLCDNumber>

MyTime::MyTime(QWidget *parent)
    :QWidget(parent), lcd(NULL)
{
    setGeometry(0, 0, 480, 300);//窗体的大小
    lcd = new QLCDNumber(this);
    lcd->setGeometry(30, 30, 200, 150);
    lcd->setNumDigits(8);//需显示8个字符
    lcd->setSegmentStyle(QLCDNumber::Flat);//用前景色填充段颜色
    QPalette plt = lcd->palette();
    //plt.setColor(QPalette::Foreground, QColor(0, 0, 255));
    plt.setColor(QPalette::Foreground, Qt::blue);
    lcd->setPalette(plt);
    lcd->setFrameStyle(QFrame::NoFrame);//

    //修改窗体背景颜色
    plt = this->palette();
    plt.setColor(QPalette::Background, Qt::green);
    setPalette(plt);
    QString curTime = QTime::currentTime().toString("hh:mm:ss");
    //qDebug << curTime;
    lcd->display(curTime);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);//设定1000毫秒时间间隔，启动定时器
}
void MyTime::showTime()
{
    static int i = 0;
    i++;
    QString curTime = QTime::currentTime().toString("hh:mm:ss");
    if (i%2 == 0)
        curTime[2] = ' ';//冒号去掉
    else
        curTime[5] = ' ';
    lcd->display(curTime);
}
