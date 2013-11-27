#ifndef PICTURE_H
#define PICTURE_H

#include <QLabel>
#include <QPixmap>
#include <QTimer>

//3个图片区,默认搜索的目录
#define PICTURE1_PATH "./picture1/"
#define PICTURE2_PATH "./picture2/"
#define PICTURE3_PATH "./picture3/"

//动态切换显示图片类
class Picture : public QLabel {
	Q_OBJECT

public:
	Picture(QWidget *parent = 0, int flag = 2);	//构造
	~Picture();						//析构

	QPixmap **pic_map;			//图片存储位置
	int pic_count;				//总图片
	int pic_cur;				//当前位置
	int stop_time;				//停留时间
	bool isview;				//图片正在显示
	QTimer *pic_timer;			//时钟
	QStringList *stringlist;	//字符串数组, 存储图片文件名

	void loadpictures(int &);			//载入目录下所有图片文件
	bool dynamic_state(void) {return isdynamic;}	//返回当前是否动态显示
	void setstoptime(int newtime);			//设置每张图片停留时间
	void change_dynamic(void);				//切换是否动态显示
	void showpicture(int);					//显示指定的图片
	void showprev_next_picture(int);		//显示上一张或下一张
	void closeshow(void);					//关闭图片显示

public slots:
	void showpicture(void);					//动态显示图片

private:
	void resizeEvent(QResizeEvent *);		//父窗口大小发生变化
	bool isdynamic;
};


#endif // PICTURE_H
