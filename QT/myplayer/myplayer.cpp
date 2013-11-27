#include <QProcess>
#include <QPixmap>
#include <QDebug>
#include <QIcon>
#include <QTimer>
#include <QTime>
#include <QDir>
#include <QMenu>
#include <QFileDialog>
#include "myplayer.h"
#include "myform.h"
#include <time.h>

#define FILEPATH "./video/"

MyPlayer::MyPlayer(QWidget *parent)
    :QWidget(parent)
{
    setupUi(this);

    myform = new MyForm(this);
    myform->setGeometry(0, 0, width() - 170, 330);

    mp = new QProcess(this);//创建子进程对象

    //设置滚动条
    hSlider->setRange(0, 100);
    hSlider->setValue(0);
    flag = 0;
    i = 0;
    n = 0;
    m = 0;
    t = 0;

     //设置定时器
    timerId = startTimer(1000);


    //设置按钮图标
    btn_add->setStyleSheet("border-radius:10px;""background-color: MediumOrchid");
    btn_sub->setStyleSheet("border-radius:10px;""background-color: MediumOrchid");
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

    btn_default->setFlat(true);
    btn_default->setStyleSheet("border: 1px solid lightcyan;");
    btn_native->setFlat(true);
    btn_native->setStyleSheet("border: 1px solid lightcyan;");
    QDir dir(FILEPATH);

    QFile f1("tmp.txt");
    f1.open(QFile::ReadOnly);
    while(!f1.atEnd())
    {
        QString tmp = f1.readLine();
        filenames1.append(tmp.mid(0, tmp.length() - 1));
    }
    f1.close();

    filenames = dir.entryList(QStringList() << "*.mp3" << "*.flv" << "*.mp4", QDir::AllEntries | QDir::NoDotAndDotDot);
    list->addItems(filenames);
    connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(do_change()));
    list->setStyleSheet("color: gray;"
                        "background-color: transparent;"
                        "selection-color: Blue;"
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
    connect(btn_default, SIGNAL(clicked()), this, SLOT(do_default()));
    connect(btn_native, SIGNAL(clicked()), this, SLOT(do_native()));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), list, SLOT(Mylist::contextMenuEvent(QContextMenuEvent *)));
}

void MyPlayer::do_default()
{
    m = 0;
    list->clear();
    list->addItems(filenames);
}
void MyPlayer::do_native()
{
    m = 1;
    list->clear();
    list->addItems(filenames1);
}

void MyPlayer::do_update(int value)
{
    QTime it = it.addSecs(value + 1); //这里设置秒数
    lbl_1->setText(it.toString("hh:mm:ss"));
    QString str = "seek " + QString::number((value * 100)/timel) + " 1\n";
    send_cmd(str.toAscii());
    catch_output();
}

void MyPlayer::do_test()
{
    //send_cmd("get_percent_pos\n");

    send_cmd("get_time_length\n");
    send_cmd("get_time_pos\n");
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
}

void MyPlayer::do_back()
{
    send_cmd("seek -1 0\n");
    do_test();
}

void MyPlayer::do_change()
{
    i = list->currentRow();
    hSlider->setValue(0);
    lbl_1->setText("00:00:00");
    lbl_2->setText("00:00:00");
    //QString str = FILEPATH + filenames[list->currentRow()];
   // qDebug() << str;
    mp->close();
    //startMplayer(str);
    qDebug() << "listname:" << list->item(i)->text();
    startMplayer(FILEPATH + list->item(i)->text());
    if (t == 0)
    t = 1;
    else t = 0;
}

void MyPlayer::catch_output()
{
    QString msg = mp->readAllStandardOutput();
    //qDebug() << msg;
    QString tmp1 =msg.section("\n",0, 0);
    QString tmp2 = msg.section("\n",1,1);
    qDebug() << tmp2;
    QStringList t1 = tmp1.split("=");
    QStringList t2 = tmp2.split("=");

    QTime it;
    if (t1.size() > 1)
    {
        t1[1]= t1[1].mid(0, t1[1].length() - 3);
        qDebug() << "Current position:" << t1.at(1).toInt() ;//<< "%" << tmp[1].length();
        if (timel != t1.at(1).toInt())
        {
        timel = t1.at(1).toInt();
        hSlider->setRange(0, timel);
        it = it.addSecs(timel); //这里设置秒数
        lbl_2->setText(it.toString("hh:mm:ss"));
        }
    }
    if (t2.size() > 1)
    {
        t2[1]= t2[1].mid(0, t2[1].length() - 2);
        int tmp = t2.at(1).toInt();
        if (tmp != 0)
        {
         hSlider->setValue(tmp + 1);

         it = it.addSecs(tmp + 1); //这里设置秒数

         lbl_1->setText(it.toString("hh:mm:ss"));
        }
        if ((hSlider->value() == timel) && (n == 1))
        {
            do_next();
        }
        else if((hSlider->value() == timel) && (n == 2))
        {
            mp->close();
            if (t == 0)
            startMplayer(FILEPATH + filenames[i]);
         else
             startMplayer(FILEPATH + filenames1[i]);
        }
        else if((hSlider->value() == timel) && (n == 3))
        {
            srand(time(NULL));
            mp->close();
            if (t == 0)
            {
                 i = rand() % filenames.size();
               startMplayer(FILEPATH + filenames[i]);
           }
         else
            {
              i = rand() % filenames.size();
             startMplayer(FILEPATH + filenames1[i]);
           }
        }
        else if ((hSlider->value() == timel) && (n == 0))
        {
             hSlider->setValue(0);
             lbl_1->setText("00:00:00");
             lbl_2->setText("00:00:00");
         }
    }
#if 0
    if(tmp.size() > 1)
    {
        tmp[1] = tmp[1].mid(0, tmp[1].length() - 1);
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
             if (t == 0)
             startMplayer(FILEPATH + filenames[i]);
          else
              startMplayer(FILEPATH + filenames1[i]);
         }
         else if((hSlider->value() >= 97) && (n == 3))
         {
             sleep(5);
             srand(time(NULL));
             mp->close();
             if (t == 0)
             {
                  i = rand() % filenames.size();
                startMplayer(FILEPATH + filenames[i]);
            }
          else
             {
               i = rand() % filenames.size();
              startMplayer(FILEPATH + filenames1[i]);
            }
         }
    }
#endif
}

void MyPlayer::do_next()
{
    hSlider->setValue(0);
    lbl_1->setText("00:00:00");
    lbl_2->setText("00:00:00");
    //if (i == filenames.size() - 1)
    if (i == list->count() - 1)
        i = 0;
    else
       i++;
    list->setCurrentRow(i);
    mp->close();
    if (t == 0)
    startMplayer(FILEPATH + filenames[i]);
    else
        startMplayer(FILEPATH + filenames1[i]);
    //startMplayer(FILEPATH + list->item(i)->text());
}

void MyPlayer::do_before()
{
    hSlider->setValue(0);
    lbl_1->setText("00:00:00");
    lbl_2->setText("00:00:00");

    if (i == 0)
        i = list->count() ;
    i--;
   list->setCurrentRow(i);
   mp->close();
   if (t == 0)
   startMplayer(FILEPATH + filenames[i%filenames.size()]);
   else
       startMplayer(FILEPATH + filenames1[i%filenames1.size()]);
   //startMplayer(FILEPATH + list->item(i)->text());
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
        n = 3;
        btn_choose->setIcon(QIcon("./files/choose4.png"));
    }
    else if(n == 3)//随机
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
    lbl_1->setText("00:00:00");
    lbl_2->setText("00:00:00");
    hSlider->setValue(0);
    lbl_1->setText("00:00:00");
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
   // qDebug() << "name : " << filename.mid(0, filename.lastIndexOf("."));
    if (filename.split(".").last() == "mp3")
    {
        myform->hide();
    }
    else
    {
        myform->show();
    }
   // startMplayer(filename.mid(0, filename.lastIndexOf(".")) + ".txt");
    mp->start("mplayer", args);//启动程序
}

void MyPlayer::timerEvent(QTimerEvent *event)
{
    if (timerId == event->timerId())
        do_test();
}

void MyPlayer::myadd()
{

    QString FileName = QFileDialog::getOpenFileName(this, "open file",
                    FILEPATH,
                    "All Files(*.*);;*.mp4,*.flv,*.mp3(*.mp4 *.flv *.mp3)");
    FileName = FileName.split("/").last();
    if (FileName == NULL)
        return;
    list->addItem(FileName);
   if (m == 0)
    filenames.append(FileName);
    else
   {
    filenames1.append(FileName);
    QFile f1("tmp.txt");
    f1.open(QFile::WriteOnly | QFile::Append);
    f1.write(FileName.toUtf8() + "\n");
    f1.close();
    }
}

void MyPlayer::myadd2()
{
    QString FileName = filenames.at(list->currentRow());
    filenames1.append(FileName);
    QFile f1("tmp.txt");
    f1.open(QFile::WriteOnly | QFile::Append);
    f1.write(FileName.toUtf8() + "\n");
    f1.close();
}

void MyPlayer::mydel()
{
    qDebug() << "delete";
    if (i > list->currentRow())
        i--;
    if (m == 0)
    filenames.removeAt(list->currentRow());
    else
    {
        filenames1.removeAt(list->currentRow());
        QFile f1("tmp.txt");
        f1.open(QFile::WriteOnly | QFile::Truncate);
        for (int j = 0; j < filenames1.size(); j++)
        {
            f1.write(filenames1[j].toUtf8() + "\n");
        }
        f1.close();
    }
    qDebug() << "delete curent row:" << list->currentRow();
    QListWidgetItem *item = list->takeItem(list->currentRow());
    list->removeItemWidget(item);
    delete item;
}

void MyPlayer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    QSize oldsize, newsize;
    oldsize = event->oldSize();//老的窗体大小
    newsize = event->size();   //新的窗体大小

    //获取mainframe窗体长宽变化
    int rW, rH;
    rW = newsize.width() - oldsize.width();
    rH = newsize.height() - oldsize.height();

    //开始按照mainframe窗体的长宽变化，
    //来改变里面的所有子对象大小
    if(oldsize.width() != -1)
    {
        myform->setGeometry(0, 0, myform->width()+rW, myform->height()+rH);
    }
}

void MyPlayer::contextMenuEvent ( QContextMenuEvent *)
{
    QMenu* popMenu = new QMenu(this);
    QAction *add = new QAction(QString::fromUtf8("添加"), this);
    QAction *addtonative = new QAction(QString::fromUtf8("添加到本地"), this);
    QAction *del = new QAction(QString::fromUtf8("删除"), this);

    popMenu->addAction(add);
    if(m == 0)
    popMenu->addAction(addtonative);

    //if(this->childAt(QCursor::pos()) != NULL)
        popMenu->addAction(del);

    connect(add , SIGNAL(triggered()) , this , SLOT(myadd()));    //右键菜单事件
    connect(addtonative , SIGNAL(triggered()) , this , SLOT(myadd2()));
    connect(del , SIGNAL(triggered()) , this , SLOT(mydel()));
    popMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
    delete popMenu;
    delete addtonative;
    delete add;
    delete del;
}
