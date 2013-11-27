#include "myplayer.h"
#include <QProcess>
#include <QPixmap>
#include <QDebug>
#include <QIcon>
#include <QTimer>
#include <QDir>
#include <QMenu>
#include <QFileDialog>

#define FILEPATH "./video/"

MyPlayer::MyPlayer(QWidget *parent)
    :QWidget(parent)
{
    setupUi(this);
    mp = new QProcess(this);//创建子进程对象

    //设置滚动条
    hSlider->setRange(0, 100);
    hSlider->setValue(0);
    flag = 0;
    i = 0;
    n = 0;

     //设置定时器
    timerId = startTimer(1000);

    btn_add->setStyleSheet("border-radius:10px;""background-color: MediumOrchid");
    btn_sub->setStyleSheet("border-radius:10px;""background-color: MediumOrchid");

    //设置按钮图标

    btn_pause->setIconSize(QSize(40, 40));
    btn_pause->setIcon(QIcon("./files/pause.png"));
    btn_pause->setStyleSheet("border-radius:10px;" "background-color: MediumOrchid");
    btn_next->setIconSize(QSize(40, 40));
    btn_next->setIcon(QIcon("./files/next.png"));
    btn_next->setStyleSheet("border-radius:10px;""background-color: MediumOrchid");
    btn_before->setIconSize(QSize(40, 40));
    btn_before->setIcon(QIcon("./files/before.png"));
    btn_before->setStyleSheet("border-radius:10px;""background-color: MediumOrchid");
    btn_stop->setIconSize(QSize(40, 40));
    btn_stop->setIcon(QIcon("./files/stop.png"));
    btn_stop->setStyleSheet("border-radius:10px;""background-color: MediumOrchid");
    btn_speed->setIconSize(QSize(40, 40));
    btn_speed->setIcon(QIcon("./files/speed.png"));
    btn_speed->setStyleSheet("border-radius:10px;""background-color: MediumOrchid");
    btn_back->setIconSize(QSize(40, 40));
    btn_back->setIcon(QIcon("./files/back.png"));
    btn_back->setStyleSheet("border-radius:10px;""background-color: MediumOrchid");
    btn_choose->setIconSize(QSize(40, 40));
    btn_choose->setIcon(QIcon("./files/1.png"));
    btn_choose->setStyleSheet("border-radius:10px;""background-color: MediumOrchid");
   btn_list->setFlat(true);

    QDir dir(FILEPATH);
    /*QFile f1("tmp.txt");
    f1.open(QFile::ReadOnly);
    while(!f1.atEnd())
    {
        QString tmp = f1.readLine();
        filenames.append(tmp.mid(0, tmp.length() - 1));
    }
    f1.close();
*/
    filenames = dir.entryList(QStringList() << "*", QDir::AllEntries | QDir::NoDotAndDotDot);
    list->addItems(filenames);
    connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(do_change()));
    list->setStyleSheet("color: lightcyan;"
                        "background-color: transparent;"
                        "selection-color: SlateBlue;"
                        "selection-background-color: lightcyan;");

    if (filenames.length() >= 1)
        startMplayer(FILEPATH + filenames[0]);
    list->setCurrentRow(0);
    connect(btn_pause, SIGNAL(clicked()), this, SLOT(do_pause()));
    connect(btn_stop, SIGNAL(clicked()), this, SLOT(do_stop()));
    connect(hSlider, SIGNAL(sliderMoved(int)), this, SLOT(do_update(int)));
    connect(btn_next, SIGNAL(clicked()), this, SLOT(do_next()));
    connect(btn_back, SIGNAL(clicked()), this, SLOT(do_back()));
    connect(btn_speed, SIGNAL(clicked()), this, SLOT(do_speed()));
    connect(btn_add, SIGNAL(clicked()), this, SLOT(do_add()));
    connect(btn_sub, SIGNAL(clicked()), this, SLOT(do_sub()));
    connect(btn_before, SIGNAL(clicked()), this, SLOT(do_before()));
    connect(btn_choose, SIGNAL(clicked()), this, SLOT(do_choose()));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), list, SLOT(contextMenuEvent(QContextMenuEvent *)));
}

void MyPlayer::do_update(int value)
{
    QString str = "seek " + QString::number(value) + " 1\n";
    send_cmd(str.toAscii());
    catch_output();
}

void MyPlayer::do_test()
{
    send_cmd("get_percent_pos\n");
    catch_output();
}

void MyPlayer::do_add()
{
    send_cmd("volume 1\n");
}

void MyPlayer::do_sub()
{
    send_cmd("volume 0\n");
}

void MyPlayer::do_speed()
{
    send_cmd("seek 1 0\n");
    do_test();
    send_cmd("vo_showFullScreen\n");

     //myform->setWindowFlags(Qt::Window);
     //myform->showFullScreen();
}

void MyPlayer::do_back()
{
    send_cmd("seek -1 0\n");
    do_test();
}

void MyPlayer::do_change()
{
    i = list->currentRow();

    QString str = FILEPATH + filenames[list->currentRow()];
    qDebug() << str;
    mp->close();
    startMplayer(str);
}

void MyPlayer::catch_output()
{
    QString msg = mp->readAllStandardOutput();
    QStringList tmp = msg.split("=");
    if(tmp.size() > 1)
    {
        tmp[1] = tmp [1].mid(0, tmp[1].length() - 1);
        qDebug() << "Current position:" << tmp.at(1) << "%" << tmp[1].length();
         hSlider->setValue(tmp.at(1).toInt() + 1);
         if ((hSlider->value() >= 97) && (n == 1))
         {
             sleep(5);
             do_next();
         }
         else if((hSlider->value() >= 97) && (n == 2))
         {
             sleep(5);
             mp->close();
             startMplayer(FILEPATH + filenames[i]);
         }
    }
}

void MyPlayer::do_next()
{
    if (i == filenames.size() - 1)
        i = 0;
    else
       i++;
    list->setCurrentRow(i);
    mp->close();
    startMplayer(FILEPATH + filenames[i]);
}

void MyPlayer::do_before()
{
    if (i == 0)
        i = filenames.size() ;
    i--;
   list->setCurrentRow(i);
   mp->close();
   startMplayer(FILEPATH + filenames[i%filenames.size()]);
}

void MyPlayer::send_cmd(QByteArray cmd)
{
    mp->write(cmd);//write函数会利用QProcess类的内建管道向子进程写入命令
}

void MyPlayer::do_choose()
{
    if (n == 0)//播放一首
    {
        n = 1;
        btn_choose->setIcon(QIcon("./files/choose1.png"));
    }
    else if(n == 1)//所有歌曲循环
    {
        n = 2;
        btn_choose->setIcon(QIcon("./files/choose2.png"));
    }
    else if(n == 2)//单曲循环
    {
        n = 0;
        btn_choose->setIcon(QIcon("./files/1.png"));
    }
}

void MyPlayer::switch_button()
{
    if (flag == 0)
    {
        flag = 1;
        btn_pause->setIcon(QIcon("./files/play.png"));
    }
    else if(flag == 1)
    {
        flag = 0;
        timerId = startTimer(1000);
        btn_pause->setIcon(QIcon("./files/pause.png"));
    }
}

void MyPlayer::do_pause()
{

     killTimer(timerId);
    send_cmd("pause\n");//程序里的命令要追加一个'\n'
    switch_button();
}

void MyPlayer::do_stop()
{
    send_cmd("stop\n");
    hSlider->setValue(0);
}

void MyPlayer::startMplayer(QString filename)
{
    QStringList args;
    args << "-slave" << "-quiet"
            << "-wid" << QString::number(myform->winId())//告诉要显示的Id
            << "-x" << QString::number(myform->width())
            << "-y" << QString::number(myform->height())
            << "-zoom"
            << filename;
    mp->start("mplayer", args);//启动程序
}

void MyPlayer::timerEvent(QTimerEvent *event)
{
    if (timerId == event->timerId())
        do_test();
}

void MyPlayer::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu* popMenu = new QMenu(this);
    QAction *add = new QAction(QString::fromUtf8("添加"), this);
    QAction *del = new QAction(QString::fromUtf8("删除"), this);

    popMenu->addAction(add);
    if(list->currentRow() >= 0)
        popMenu->addAction(del);

    connect(add , SIGNAL(triggered()) , this , SLOT(myadd()));    //右键菜单事件
    connect(del , SIGNAL(triggered()) , this , SLOT(mydel()));
    popMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
    delete popMenu;
    delete add;
    delete del;
}

void MyPlayer::myadd()
{
    QString FileName = QFileDialog::getOpenFileName(this, "open file",
                    FILEPATH,
                    "All Files(*.*);;*.mp4,*.flv(*.mp4, *.flv)");
    FileName = FileName.split("/").last();
    filenames.append(FileName);
    list->addItem(FileName);

   /* QFile f1("tmp.txt");
    f1.open(QFile::WriteOnly | QFile::Append);
    f1.write(FileName.toAscii() + "\n");
    f1.close();*/
}

void MyPlayer::mydel()
{
    i--;
    filenames.removeAt(list->currentRow());
    QListWidgetItem *item = list->takeItem(list->currentRow());
    list->removeItemWidget(item);
    delete item;

    /*QFile f1("tmp.txt");
    f1.open(QFile::WriteOnly | QFile::Truncate);
    for (int j = 0; j < filenames.size(); j++)
    {

      f1.write(filenames[j].toAscii().data());
    }
    f1.close();*/
}
