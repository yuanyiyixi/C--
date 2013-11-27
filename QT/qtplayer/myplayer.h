#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QWidget>
#include <QProcess>
#include "ui_form.h"

class MyPlayer : public QWidget, public Ui::Form
{
    Q_OBJECT
public:
    MyPlayer(QWidget *parent=0);
    void startMplayer(QString filename);//fork一个子进程，执行mplayer程序
    void send_cmd(QByteArray cmd);//给mplayer子进程发命令
    void switch_button();//切换按钮显示
    void catch_output();//捕获mplayer回答

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
void contextMenuEvent ( QContextMenuEvent * event );
void myadd();
void mydel();
private:
    QProcess *mp;//指向进程对象的指针
    int flag;//暂停状态
    QStringList filenames;//存储所有的播放文件
    int i;//当前播放第几首
    int n;
    int timerId;//时钟ID
};
/*class ListWidget : public QListWidget
{
    void contextMenuEvent ( QContextMenuEvent * event );
};*/

#endif // MYPLAYER_H
