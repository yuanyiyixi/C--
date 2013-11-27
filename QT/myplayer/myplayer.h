#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QWidget>
#include <QProcess>
#include <QResizeEvent>
#include "ui_form.h"


class MyForm;


class MyPlayer : public QWidget, public Ui::Form
{
    Q_OBJECT
public:
    MyPlayer(QWidget *parent=0);

    void startMplayer(QString filename);//fork一个子进程，执行mplayer程序
    void send_cmd(QByteArray cmd);//给mplayer子进程发命令
    void switch_button();//切换按钮显示
    void catch_output();//捕获mplayer回答
    void contextMenuEvent ( QContextMenuEvent *event);
    MyForm *myform; //播放视频窗体

void resizeEvent(QResizeEvent *event);
public slots:
   void do_pause();//暂停
   void do_stop();//停止一首歌
   void do_test();//设定滚动条的位置
   void do_speed();//快进
   void do_back();//后退
   void do_change();//选择歌曲播放
   void do_update(int value);//滑动滚动条时可以控制进度
   void do_next();//下一首
   void do_before();//上一首
   void timerEvent(QTimerEvent *event);//定时器事件
   void do_add();//音量加
   void do_sub();//音量减
   void do_choose();//播放模式

   void do_default();//默认列表parent_win
   void do_native();//本地列表
   void myadd();//从文件夹添加
   void mydel();//从列表删除
   void myadd2();//添加到本地

private:
    QProcess *mp;//指向进程对象的指针
    int flag;//暂停状态
    QStringList filenames;//存储所有的播放文件
    QStringList filenames1;
    int i;//当前播放第几首
    int n;//播放模式
    int m;//列表 m=0,默认列表； m=1，本地列表
    int t;//正在播放的列表 t=0,
    int timerId;//时钟ID
    int timel;//文件总时间

};

#endif // MYPLAYER_H
