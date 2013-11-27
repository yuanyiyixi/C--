#include "lcdtime.h"
#include <QDebug>
#include <QPalette>

Lcdtime::Lcdtime(int num, QWidget *parent)
:QLCDNumber(8, parent)
{
	int width = parent->width();
	int height = parent->height();

	strcpy(format, "hh:mm:ss");		//默认样式
	symbol = ':';					//默认间隔符号
	setGeometry(width * 0.76, height * 0.01, width * 0.23, height * 0.075);

	QString timestr = QTime::currentTime().toString(format);
	//display(timestr);

	timer = new QTimer(this);
	timer->start(1000);

	text_color.setRgb(0, 0, 0xff, 255);
	int mode = 0;
	this->set_foreground_color(text_color, mode);	//默认,前景文字全填充

	//ground_color.setRgb(0, 0xff, 0, 255);
	//this->set_background_color(ground_color);	//背景色填充色默认开启

	setFrameStyle(QFrame::NoFrame);				//默认无边框

	ishide = true;
	connect(timer, SIGNAL(timeout()), this, SLOT(timeadd()));
}

void Lcdtime::timeadd()				//时钟Timer,读取系统时间
{
	QString timestr = QTime::currentTime().toString(format);
	display(timestr);
	//qDebug() << lcd->intValue();
}

void Lcdtime::change_symbol(char ch)	//更改间隔符号
{
	char *p;

	p = strchr(format, symbol);
	*p = ch;
	p = strchr(p, symbol);
	*p = ch;

	symbol = ch;
}

void Lcdtime::change_hide()			//切换显示与隐藏
{
	if (ishide)
		this->hide();
	else
		this->show();
	ishide = !ishide;
}

void Lcdtime::set_foreground_color(QColor &color, int &flag)	//设置前景色, 文字颜色
{
	if (flag == 1)
		setSegmentStyle(QLCDNumber::Filled);		//Filled无边框不填充，
	else if (flag == 0)
		setSegmentStyle(QLCDNumber::Flat);			//Flat,全填充,含边框
	else
		setSegmentStyle(QLCDNumber::Outline);		//Outline(背景色)

	QPalette plt;
	plt.setColor(QPalette::WindowText, color);
	setPalette(plt);
}

void Lcdtime::set_frame_mode(int num)				//设置边框样式
{
	setFrameStyle(num);
}

void Lcdtime::set_background_mode(bool mode)		//设置背景模式
{
	setAutoFillBackground(mode);
}
