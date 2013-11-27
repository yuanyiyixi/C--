#include "picture.h"
#include <QDir>
#include <QDebug>

Picture::Picture(QWidget *parent, int flag)	//构造
:QLabel(parent)
{
	//qDebug() << "flag=" << flag;
	int width = parent->width();
	int height = parent->height();

	if (flag == 2)				//图片2, 右下
		setGeometry(width * 0.76, height * 0.56, width * 0.23, height * 0.29);
	else if (flag == 1)			//图片1, 右中
		setGeometry(width * 0.76, height * 0.26, width * 0.23, height * 0.29);
	else						//图片区3,右上角
		setGeometry(width * 0.76, height * 0.095, width * 0.23, height * 0.155);

	loadpictures(flag);		//载入图片

	pic_timer = new QTimer(this);
	stop_time = 4000 * flag;	//两块图片区超时时间初值不一样
	pic_timer->start(stop_time);
	isdynamic = true;			//动态显示标志变量
	connect(pic_timer, SIGNAL(timeout()), this, SLOT(showpicture()));
	isview = true;
	//QString path("/home/zcb/dpf-student/dpf-teach/src-test/image/fj05.jpg");
	//pic_map = new QPixmap(path);
}

void Picture::loadpictures(int &flag)
{
	//qDebug() << "loadpicture";
	char picture_path[100];

	if (flag == 2)
		strcpy(picture_path, PICTURE2_PATH);
	else if (flag == 1)
		strcpy(picture_path, PICTURE1_PATH);
	else
		strcpy(picture_path, PICTURE3_PATH);

	QDir dir(picture_path);
	QStringList filters;						//过滤器
	stringlist = new QStringList;				//类似字符串数组
	filters << "*.jpg" << "*.bmp" << "*.jpeg";	//设置过滤器
	*stringlist = dir.entryList(filters);		//遍历目录下符合条件文件, 返回QStringList格式
	//dir.setNameFilters(filters);
	pic_cur = pic_count = (*stringlist).count();//取得符合条件总文件数
	pic_map = new QPixmap *[pic_count];			//二重指针分配空间

	for (int i = 0; i < pic_count; i++) {		//存储每张图片信息
		pic_map[i] = new QPixmap(picture_path + (*stringlist)[i]);
	}
	//qDebug() << "loadover";
}

void Picture::setstoptime(int newtime)	//设置每张图片停留时间
{
	stop_time = newtime;
	pic_timer->start(stop_time);
}

void Picture::change_dynamic()	//动态显示图片开关
{
	if (isdynamic) {
		disconnect(pic_timer, SIGNAL(timeout()), this, SLOT(showpicture()));
	}
	else {
		connect(pic_timer, SIGNAL(timeout()), this, SLOT(showpicture()));
	}
	isdynamic = !isdynamic;
}

void Picture::showpicture()		//显示图片, 槽函数, 每次显示不同的图片
{
	pic_cur += 1;

	if (pic_cur >= pic_count)
		pic_cur = 0;

	setPixmap(pic_map[pic_cur]->scaled(this->width(), this->height()));
}

void Picture::showpicture(int n)		//显示指定哪一张
{
	pic_cur = n - 1;
	setPixmap(pic_map[pic_cur]->scaled(this->width(), this->height()));
}

void Picture::showprev_next_picture(int n) //显示上一张或下一张
{
	pic_cur += n;

	if (pic_cur < 0)
		pic_cur = pic_count - 1;
	if (pic_cur >= pic_count)
		pic_cur = 0;

	setPixmap(pic_map[pic_cur]->scaled(this->width(), this->height()));
}

void Picture::resizeEvent(QResizeEvent *ev)	//大小改变触发
{
	if (pic_cur >= pic_count)
		pic_cur = 0;
	setPixmap(pic_map[pic_cur]->scaled(this->width(), this->height()));
}

Picture::~Picture()			//析构, 释放申请的内存
{
	for (int i = 0; i < pic_count; i++)
		delete pic_map[i];
	delete[] pic_map;
	delete stringlist;	
}
