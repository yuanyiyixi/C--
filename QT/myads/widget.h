#ifndef WIDGET_H
#define WIDGET_H

#include <QtGui/QMainWindow>
#include <QLCDNumber>
#include <QFileInfoList>
#include <QLabel>
#include <QProcess>
#include <QSlider>
#include <QPushButton>
#include <QMenuBar>
#include <QKeyEvent>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

class Widget : public QMainWindow
{
    Q_OBJECT    //定义新的槽函数和信号要用到

private:
    QLCDNumber *lcdnumber;
    //QTimer * timer;
    QMovie * logo;
    int timerid[4];
QFileInfoList infos;
     QLabel * label3;
     QLabel * label4;
     QLabel * label_txt;
     int pos;
     int n;
      QProcess *mp;//指向进程对象的指针
      QSlider * hSlider;
      QPushButton * btn_pause;
      QWidget * myform;
      QStringList filenames;
      int flag;
      int time;
 QAction*  openAction;
    QStringList list_0;//记录文件的绝对路径
    QStringList list_1;//加载 lst 文件里的内容
    int m;
    QTcpServer *server;
    QTcpSocket *socket;

public:
    Widget(QWidget *parent = 0);//0,独立的框，构造函数主动调用，可传参
    ~Widget();//析构函数自动调用，没有机会调用，没有参数
    void choose_1();
    void choose_2();
    void choose_3();
    void choose_4();
    void startMplayer(QString filename);
    void send_cmd(QByteArray cmd);//给mplayer子进程发命令

signals:
    void accelerate(bool);  //发送信号
    void change(int);
    void ctl_speed(bool);
    void ctl_posadd();
    void ctl_possub();
    void ctl_pause();
    void ctl_stop();
    void ctl_voice(int);
    void ctl_slider();

public slots:
    void showTime();    //显示时间
    void onAcc(bool b); //速度的加减
    void change_pic(int i);
    void showpic();
    void showtxt();
    void do_pause();
    void do_speed(bool b);//视频进度
    void do_update(int value);
    void do_next();//下一首
    void do_before();//上一首
    void do_voice(int b);
    void voice();
    void openFile();
    void stop();
    void do_test();//设定滚动条的位置
    void catch_output();//捕获mplayer回答
    void slotnewConnect();
    void slotDataArrive(void);

protected:
    void keyPressEvent(QKeyEvent * event);  //按键加减控制速度
    void wheelEvent(QWheelEvent * event);   //鼠标滑轮控制速度
    void mousePressEvent(QMouseEvent *event);   //鼠标左右键控制速度
    void timerEvent(QTimerEvent *event);
    //void paintEvent(QPaintEvent * event);
    void contextMenuEvent(QContextMenuEvent * event);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *, QEvent *event);
};


#endif // WIDGET_H
