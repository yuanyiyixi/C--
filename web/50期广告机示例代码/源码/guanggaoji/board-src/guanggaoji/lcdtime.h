#ifndef LCDTIME_H
#define LCDTIME_H

#include <QLCDNumber>
#include <QTimer>
#include <QTime>
#include <QColor>

class Lcdtime : public QLCDNumber
{
	Q_OBJECT

public:
	QTimer *timer;			//时钟,1秒一跳
	QColor text_color;		//字体颜色
	QColor ground_color;	//背景颜色
	char format[9];			//hh:mm:ss,显示格式

	Lcdtime(int num, QWidget *parent = 0);
	void change_symbol(char);		//设置间隔符
	void change_hide(void);	//切换显示与隐藏
	void set_foreground_color(QColor &, int &);	//设置前景色,三种模式
	void set_frame_mode(int);		//设置边框样式
	void set_background_mode(bool);	//设置背景样式

public slots:
	void timeadd(void);		//槽,读取系统时间

private:
	bool ishide;		//是否隐藏
	char symbol;		//间隔符1
};

#endif // LCDTIME_H
