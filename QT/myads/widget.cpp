#include "widget.h"
#include <QLabel>
#include <QPixmap>
#include <QTime>
#include <QPalette>
#include <QTimer>
#include <QMovie>
#include <QDebug>
#include <QKeyEvent>
#include <QDir>
#include <QFileInfoList>
#include <QProcess>
#include <QPushButton>
#include <QSlider>
#include <QMenu>
#include <QFileDialog>
#include <QAction>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QTextCodec>
//#include <QPainter>

Widget::Widget(QWidget *parent)
    : QMainWindow(parent), lcdnumber(0)
    //QWidget(parent)指定父类构造函数的参数 //初始化成员列表
{
    flag = pos = n = m = 0;

    QMenuBar * menubar = new QMenuBar(this);
    QMenu * file = new QMenu(tr("&File"), this);
    file->addAction(tr("&Open"), this, SLOT(openFile()));
    file->addAction(tr("E&xit"), this, SLOT(close()));
    menubar->addMenu(file);
    file = new QMenu(tr("&goto"), this);
    file->addAction(QString::fromUtf8("下一首"), this, SLOT(do_next()));
    file->addAction(QString::fromUtf8("上一首"), this, SLOT(do_before()));
    menubar->addMenu(file);
    this->setMenuBar(menubar);

    QAction * openAction = new QAction(QIcon("./files/1.jpg"), tr("open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing file"));
    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openFile()));
    QToolBar * toolbar = addToolBar(tr("File"));
    toolbar->addAction(openAction);

    openAction = new QAction(QIcon("./files/1.gif"), tr("exit"), this);
    openAction->setShortcuts(QKeySequence::Quit);
    openAction->setStatusTip(tr("Exit the application"));
    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(close()));
    toolbar->addAction(openAction);
    statusBar();

#if 1
    //视频
        mp = new QProcess(this);

    QDate date = QDate::currentDate();//=用的是普通构造函数
    //QDate date1(date);//拷贝构造函数
    //date1 = date;//赋值
    QString strTitle = date.toString(QString::fromUtf8("yyyy-MM-d ddd"));
    //strTitle += date.toString("ddd");
    this->setWindowTitle(strTitle);

    QDir dir("./files/");
    infos = dir.entryInfoList(QStringList() << "*.jpg" << "*.png" << "*.gif", QDir::Files,QDir::Name);

    QLabel * label = new QLabel(this);
    QPalette p = this->palette();
    p.setColor(QPalette::Foreground, Qt::green);
    this->setPalette(p);

    this->setFixedSize(800, 570);
    label->setScaledContents(true);//开始时自动适应窗体
    label->setGeometry(0, 70, 800, 480);
    QPixmap *pixmap = new QPixmap(QString("./files/6.gif"));
    label->setPixmap(*pixmap);

    //dynamic picture control
    logo = new QMovie(QString("./files/20.gif"));
    QLabel * label2 = new QLabel(this);
    label2->setGeometry(710, 70, 90, 50);
    label2->setScaledContents(true);
    label2->setMovie(logo);
    logo->start();
    connect(this, SIGNAL(accelerate(bool)), this, SLOT(onAcc(bool)));

    //time control
    lcdnumber = new QLCDNumber(8, this);
    lcdnumber->setFrameStyle(QFrame::NoFrame);
    lcdnumber->setSegmentStyle(QLCDNumber::Flat);
    lcdnumber->setGeometry(710, 70, 90, 50);
    QTime time = QTime::currentTime();
    QString strTime = time.toString("hh:mm:ss");
    lcdnumber->display(strTime);

    /*QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);//设定1000毫秒时间间隔，启动定时器
    label->setGeometry(0, 0, 710, 480);*/
    timerid[0] = startTimer(1000);//控制时钟
    timerid[1] = startTimer(10);//控制图片
    timerid[2] = startTimer(100);//控制滚动字幕

    connect(this, SIGNAL(change(int)), this, SLOT(change_pic(int)));


    label3 = new QLabel(this);
    label3->setGeometry(710, 120, 90, 190);
    label3->setScaledContents(true);
    *pixmap = QPixmap(QString(infos.at(0).absoluteFilePath()));
    label3->setPixmap(*pixmap);

    label4 = new QLabel(this);
     label4->setGeometry(710, 310, 90, 190);
     label4->setScaledContents(true);
     * pixmap = QPixmap(QString(infos.at(1).absoluteFilePath()));
     label4->setPixmap(*pixmap);

     label_txt = new QLabel(this);
     label_txt->setGeometry(0, 500, 800, 50);
     //label_txt->setScaledContents(true);
        //p.setColor(QPalette::Foreground, Qt::red);
        //label_txt->setPalette(p);
        label_txt->setStyleSheet("color: blue");
        label_txt->setFont(QFont("Times", 18, QFont::Bold));
        label_txt->setAlignment(Qt::AlignRight | Qt::AlignCenter);

       // label_txt->setTextFormat(Qt::RichText);
       //label_txt->setText("<u></u>");

        //暂停键
      btn_pause = new QPushButton("pause", this);
      btn_pause->setGeometry(0,460, 40, 40);

      connect(btn_pause, SIGNAL(clicked()), this, SLOT(do_pause()));
//滑块
      hSlider = new QSlider(Qt::Horizontal, this);
      hSlider->setGeometry(42, 470, 660, 18);
      hSlider->setRange(0, 100);
      hSlider->setValue(1);

        timerid[3] = startTimer(1000);//控制滑块位置更新
      connect(this, SIGNAL(ctl_speed(bool)), this, SLOT(do_speed(bool)));
      connect(this, SIGNAL(ctl_posadd()), this, SLOT(do_next()));
      connect(this, SIGNAL(ctl_possub()), this, SLOT(do_before()));
      connect(this, SIGNAL(ctl_pause()), this, SLOT(do_pause()));
      connect(this, SIGNAL(ctl_stop()), this, SLOT(stop()));
      connect(this, SIGNAL(ctl_voice(int)), this, SLOT(do_voice(int)));
      connect(mp, SIGNAL(readyReadStandardOutput()),
              this, SLOT(catch_output()));
      connect(hSlider, SIGNAL(sliderMoved(int)), this, SLOT(do_update(int)));

      myform = new QWidget(this);
      myform->setGeometry(0, 70, 710, 390);
      QDir dir1("./video/");
     filenames = dir1.entryList(QStringList() << "*.flv" << "*.mp4", QDir::AllEntries | QDir::NoDotAndDotDot);
      startMplayer("./video/" + filenames[0]);
      this->setFixedSize(800, 570);
#endif
      //网络监听
      server = new QTcpServer(this);
      server->listen(QHostAddress("0.0.0.0"), 8000);		//监听
      connect(server, SIGNAL(newConnection()), this, SLOT(slotnewConnect()));	//当有新连接时来时触发

  }

void Widget::onAcc(bool b)
{
    int speed = logo->speed();
    if (speed > 50 && speed <= 500)
    {
        if (b)
            logo->setSpeed(speed + 10);
        else
             logo->setSpeed(speed - 10);
    }
    else
        logo->setSpeed(100);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit accelerate(1);
    else if(event->button() == Qt::RightButton)
        emit accelerate(0);
}

void Widget::wheelEvent(QWheelEvent * event)
{
    if (event->delta() > 0)
        emit accelerate(1);
    else if(event->delta() < 0)
        emit accelerate(0);
}

void Widget::keyPressEvent( QKeyEvent * event )
{
    qDebug() << event->key();
    switch(event->key())
    {
    case Qt::Key_Plus:
        emit accelerate(1);
        break;
    case Qt::Key_Minus:
        emit accelerate(0);
        break;
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Up:
    case Qt::Key_Right:
                 return;

    case Qt::Key_Greater://(>)快进
        emit ctl_speed(1);
        break;
    case Qt::Key_Less://(<)后退
        emit ctl_speed(0);
        break;
    case Qt::Key_AsciiCircum://(^)音量+
        emit ctl_voice(1);
        break;
    case Qt::Key_QuoteLeft://(`)音量减
        emit ctl_voice(0);
        break;
    case Qt::Key_M://静音
        emit ctl_voice(2);
        break;
    case Qt::Key_N://下一曲
        emit ctl_posadd();
        break;
    case Qt::Key_P://play
        emit ctl_pause();
        break;
    case Qt::Key_S://stop
        emit ctl_stop();
        break;
    case Qt::Key_V://上一曲
       emit ctl_possub();
        break;
    default:
        break;
    }
    QWidget::keyPressEvent(event);//调用父类函数
}

void Widget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerid[0])
    {
        emit change(0);
    }
    else if(event->timerId() == timerid[1])
        emit change(1);
    else if(event->timerId() == timerid[2])
        emit change(2);
    else if (event->timerId() == timerid[3])
        emit change(3);
}

/*void Widget::paintEvent(QPaintEvent * )
{

    QString str_txt = QString::fromUtf8("你好！ hello！第几个胖嫂5递归平56飞的更");
    static int j = 0;
    label_txt->setText(label_txt->text() + str_txt.mid((j % str_txt.length()), 1));
     j++;

}*/

void Widget::change_pic(int i)
{
    if (i == 0)
    {
        showTime();
    }
    else if(i == 1)
        showpic();
    else if(i == 2)
        showtxt();
        //update();
    else if (i == 3)
    {
      do_test();
   }
}

void Widget::showpic()
{
    switch (n % 4)
    {
    case 0:choose_1();break;
    case 1:choose_3();break;
    case 2:choose_2();break;
    case 3:choose_4();break;
    }
}

void Widget::choose_1()
{
    static int i = 0;
    i++;
    if (label3->height() > 0)
    {
        label3->setGeometry(710, 120, 90, label3->height() - 10);
        label4->setGeometry(710, label3->height() + 120, 90, 380 - label3->height());
    }
    else if (i % 200 == 0)
    {
         n++;
         i = 0;
         QPixmap *pixmap = new QPixmap(QString(infos.at(n%infos.size()).absoluteFilePath()));
         label3->setPixmap(*pixmap);
         *pixmap =  QPixmap(QString(infos.at((n + 1)%infos.size()).absoluteFilePath()));
         label4->setPixmap(*pixmap);
         label3->setGeometry(710, 120, 90, 380);
         label4->setGeometry(710, 120, 0, 0);
    }
}

void Widget::choose_2()
{
    static int i = 0;
    i++;
    if (label3->height() > 0)
    {
        label3->setGeometry(710, 500 - label3->height() + 10, 90, label3->height() - 10);
        label4->setGeometry(710, 120, 90, 380 - label3->height());
    }
    else if (i % 200 == 0)
    {
         n++;
        i = 0;
        QPixmap *pixmap = new QPixmap(QString(infos.at(n%infos.size()).absoluteFilePath()));
        label3->setPixmap(*pixmap);
        *pixmap =  QPixmap(QString(infos.at((n + 1)%infos.size()).absoluteFilePath()));
        label4->setPixmap(*pixmap);
        label3->setGeometry(710, 120, 90, 380);
        label4->setGeometry(710, 120, 0, 0);
    }
}

void Widget::choose_3()
{
    static int i = 0;
    i++;
    if (label3->width() > 0)
    {
       label3->setGeometry(710 + label4->width() + 10, 120, label3->width() - 10, 380);
       label4->setGeometry(710, 120, 90 - label3->width(), 380);
    }
    else if (i % 200 == 0)
    {
        i = 0;
        n++;
        QPixmap *pixmap = new QPixmap(QString(infos.at(n++%infos.size()).absoluteFilePath()));
        label3->setPixmap(*pixmap);
        *pixmap =  QPixmap(QString(infos.at(n %infos.size()).absoluteFilePath()));
        label4->setPixmap(*pixmap);
        label3->setGeometry(710, 120, 90, 380);
       label4->setGeometry(710, 120, 0, 0);
    }
}

void Widget::choose_4()
{
    static int i = 0;
    i++;
    if (label3->width() > 0)
    {
        label3->setGeometry(710, 120, label3->width() - 10, 380);
        label4->setGeometry(710 + label3->width(), 120, 90 - label3->width() , 380);
    }
    else if (i % 200 == 0)
    {
         n++;
        i = 0;
        QPixmap *pixmap = new QPixmap(QString(infos.at(n%infos.size()).absoluteFilePath()));
        label3->setPixmap(*pixmap);
        *pixmap =  QPixmap(QString(infos.at((n + 1)%infos.size()).absoluteFilePath()));
        label4->setPixmap(*pixmap);
        label3->setGeometry(710, 120, 90, 380);
       label4->setGeometry(710, 120, 0, 0);
    }
}

void Widget::showTime()
{
    QTime time = QTime::currentTime();
    QString strTime = time.toString("hh:mm:ss");
    lcdnumber->display(strTime);

    if(time.second() == 0)
        strTime[2] = ' ';//冒号去掉
    if (time.second() % 2)
        strTime[5] = ' ';
    lcdnumber->display(strTime);
}

void Widget::showtxt()
{
    QString str_txt = QString::fromUtf8("你好！ hello！");
    static int j = 0;

    label_txt->setText(label_txt->text() + str_txt.mid((j % str_txt.length()),1));
    j++;

   /* QPixmap pTimePix(tr("./files/171.jpeg"));
    QPainter painter1;
    painter1.begin(&pTimePix);
    painter1.setPen(Qt::green);
    painter1.drawText(0,50,200,200, Qt::AlignBottom, label_txt->text() + str_txt.mid((j % str_txt.length()),1));    //这里要确保你的坐标等等参数正确，不然你就不知道画到哪里去了
    painter1.end();
    label_txt->setPixmap(pTimePix);*/
    //update();
}

Widget::~Widget()
{

}

void Widget::startMplayer(QString filename)
{
    QStringList args;
    args << "-slave" << "-quiet"
            << "-wid" << QString::number(myform->winId())//告诉要显示的Id
            << "-x" << QString::number(myform->width())
            << "-y" << QString::number(myform->height())
            << "-zoom"
            << filename;
    list_0.append(filename);
    mp->start("mplayer", args);//启动程序
}

void Widget::do_pause()
{
    killTimer(timerid[3]);
    send_cmd("pause\n");//程序里的命令要追加一个'\n'
    if (flag == 0)
    {
        flag = 1;
        btn_pause->setIcon(QIcon("./files/play.png"));
    }
    else if(flag == 1)
    {
        flag = 0;
        timerid[3] = startTimer(1000);
        btn_pause->setIcon(QIcon("./files/pause.png"));
    }
}

void Widget::send_cmd(QByteArray cmd)
{
    mp->write(cmd);//write函数会利用QProcess类的内建管道向子进程写入命令
}

void Widget::do_test()
{
    send_cmd("get_time_length\n");
    //send_cmd("get_time_pos\n");
    hSlider->setValue(hSlider->value() + 1);
    catch_output();
}

void Widget::catch_output()
{
   QString msg = mp->readAllStandardOutput();
    QStringList t1 = msg.split("=");
   static  int time = 0;
    if (t1.at(0) == "ANS_LENGTH")
    {

        int t = t1.at(1).length() - 4;
        if(time != t1.at(1).mid(0,t).toInt())
        {
        time = t1.at(1).mid(0,t).toInt();
        hSlider->setRange(0, t1.at(1).mid(0,t).toInt());
         qDebug() << "time = " << time;
     }
    }
   /* else if(t1.at(0) == "ANS_TIME_POSITION")
    {
        int t = t1.at(1).length() - 3;
        hSlider->setValue(t1.at(1).mid(0,t).toInt()+1);
    }*/
    if (hSlider->value() == time && time != 0)
        do_next();
 }

void Widget::do_next()
{
    hSlider->setValue(1);
    if (m == 0){
    if (pos >= filenames.size() - 1)
        pos = 0;
    else
       pos++;
    mp->close();
        startMplayer("./video/" + filenames[pos]);
    }
    else{
        if (pos >= list_1.size() - 1)
            m = 0;
        else
           pos++;
        mp->close();
        startMplayer(list_1[pos]);
    }
}

void Widget::do_before()
{
    hSlider->setValue(1);
    if (m == 0){
    if (pos == 0)
        pos = filenames.size();
   pos--;
   mp->close();
       startMplayer("./video/" + filenames[pos]);
   }
    else{
        if (pos == 0)
            pos = list_1.size();
       pos--;
       mp->close();
       startMplayer(list_1[pos]);
   }
}

void Widget::do_speed(bool b)
{
  int t;
    if (b)
         t = hSlider->value()+ 10;
    else
         t = hSlider->value() - 10;
    QString str = "seek " + QString::number(t) + " 2\n";
    send_cmd(str.toAscii());
    hSlider->setValue(t + 1);
}

void Widget::do_update(int value)
{
    qDebug() << "value =" << hSlider->value();
    QString str = "seek " + QString::number(value) + " 2\n";
    send_cmd(str.toAscii());
}

void Widget::contextMenuEvent(QContextMenuEvent * event)
{
    int x = event->x();
    int y = event->y();
    if (x < 710 && y < 390 )
    {
        QMenu* popMenu = new QMenu(this);
    if (flag == 0)
        popMenu->addAction(tr("&Pause(P)"), btn_pause, SIGNAL(clicked()));
    else
        popMenu->addAction(tr("&Play(P)"), btn_pause, SIGNAL(clicked()));
        popMenu->addAction(tr("&Open(O)"), this, SLOT(openFile()));
        popMenu->addAction(QString::fromUtf8("下一首(N)"), this, SLOT(do_next()));
        popMenu->addAction(QString::fromUtf8("上一首(V)"), this, SLOT(do_before()));
        popMenu->addAction(QString::fromUtf8("静音(M)"), this, SLOT(voice()));
        QAction *act = new QAction(tr("&Stop"), this);
        QObject::connect(act, SIGNAL(triggered(bool)), this, SLOT(stop()));
        if (hSlider->value() == 0)
        act->setEnabled(false);
        else
            act->setEnabled(true);
        popMenu->addAction(act);
        popMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
        delete popMenu;
    }
    QWidget::contextMenuEvent(event);
}

void Widget::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Video Files"), ".", tr("Video Files (*.mp4 *.flv *.rmvb *.lst *.mp3 *.f4v)"));
    qDebug() <<"fileName = " << fileName << endl;

    if (fileName.split(".").last() == "lst")
    {
        m = 1;
        pos = 0;
        QFile f1(fileName);
        f1.open(QFile::ReadOnly);
        while(!f1.atEnd())
        {
            /*QByteArray tmp=f1.readLine();
            QTextCodec* gbk_codec = QTextCodec::codecForName("utf8");
            QString str = gbk_codec->toUnicode(tmp);
            qDebug() << "tmp =" << str;
            list_1.append(str.left(str.length() - 1));*/
            QString tmp = QString::fromLocal8Bit(f1.readLine());
             qDebug() << "tmp =" << tmp;
            list_1.append(tmp.left(tmp.length() - 1));
        }
        f1.close();
        mp->close();
         hSlider->setValue(0);
        startMplayer(list_1[0]);
    }
    else if(fileName != NULL)
    {
        m = 0;
        mp->close();
         hSlider->setValue(0);
        startMplayer(fileName);
    }
}

void Widget::stop()
{
    hSlider->setValue(0);

    do_pause();
    send_cmd("stop\n");
}

void Widget::do_voice(int b)
{
    if (b == 1)
        send_cmd("volume 1\n");
    else if(b == 0)
        send_cmd("volume 0\n");
    else if(b == 2)
     voice();
}

void Widget::voice()
{
    static int b = 1;
    if (b)
    {
        b = 0;
        send_cmd("mute 1\n");
    }
    else
    {
        b = 1;
        send_cmd("mute 0\n");
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::Ok == QMessageBox::question(this,"Quit",
                            QString::fromUtf8("你确定要退出吗？"),
                            QMessageBox::Ok | QMessageBox::Cancel))
    {
        list_0.removeDuplicates();
        QFile f1("playlist.lst");
        f1.open(QFile::WriteOnly | QFile::Truncate);
        for (int j = 0; j < list_0.size(); j++)
        {
            f1.write(list_0[j].toUtf8() + "\n");
        }
        f1.close();
       event->accept();
    }
}

bool Widget::eventFilter(QObject *, QEvent *event)
{
            // 对QEvent进行强制转换，并获得键值
   int key = (static_cast<QKeyEvent *>(event))->key();
           // 需要过滤的按键
   if (key == Qt::Key_Left || key == Qt::Key_Right || key == Qt::Key_Up ||
                    key == Qt::Key_Down )
   {
       return true;
   }
    return false;
}

void Widget::slotnewConnect()		//有新连接时触发
{
        qDebug() << "new connect";
        socket = server->nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), this, SLOT(slotDataArrive()));
}

void Widget::slotDataArrive()	//有数据可读时触发
{
        QString buf;
        while (socket->bytesAvailable()) {			//读取数据
            buf = socket->readAll();
                qDebug() << "buf=" << buf;
        }

        unsigned char command;
        unsigned int comm_pos = buf.indexOf('&');	//分割得到的字符串, 以'&'为分割符

        command = buf.left(comm_pos).toUInt();		//取出命令
        buf.remove(0, comm_pos + 1);				//修改buf, 余下部分是参数, 某些控制命令没有参数
        qDebug() << "command=" << command;
        qDebug() << "argument=" << buf;
 }
