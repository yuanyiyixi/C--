#ifndef TEXTAREA_H
#define TEXTAREA_H

#include <QLabel>
#include <QTimer>

class Textarea : public QLabel
{
	Q_OBJECT
public:
	Textarea(QWidget *parent = 0);	//构造
	QLabel *text;				//文字label, 本类不直接显示文字, 而是新建了text的label,显示文字
	QColor text_color;			//字体颜色
	QByteArray text_family;		//字体名
	int text_size;				//字号
	QTimer *texttimer;			//动态滚动定时器
	float smoothness;			//平滑度


	void setmsg(QString);		//设置文字内容函数
	void setfontsize(int);		//设置字号
	void setfontfamily(char *);	//设置字体
	void setcolor(int r, int g, int b, int a);			//设置文字颜色(RGB方式)
	void setcolor(int c, int m, int y, int k, int a);	//设置文字颜色(CMYK方式)
	void change_display_mode(void);		//改变显示模式, 静态/动态
	void change_show_hide(void);		//显示与隐藏

public slots:
	void dynamic_displaytext(void);		//动态显示处理函数, 槽函数, 时钟超时触发

private:
	int i;				//动态显示, 循环的变量, 记录要显示的x坐标
	int textwidth;		//字符占用总宽度
	bool isdynamic;		//动态显示标志			true动态, false静态
	bool isshow;		//是否显示/隐藏标志	true显示, false隐藏


	void resizeEvent(QResizeEvent *);	//大小改变时,载入时 会触发
};

#endif // TEXTAREA_H
