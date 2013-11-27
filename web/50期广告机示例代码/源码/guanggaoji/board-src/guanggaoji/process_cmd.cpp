#include "mainframe.h"

void MainWindow::slotDataArrive()	//有数据可读时触发
{
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

	switch (command) {
//视频命令相关处理
	case 1 : {			//播放与暂停, 停止时重新创建进程载入
		if (state) {						//如果正在播放
			prc->write("pause\n");		//播放/暂停
			isplay = !isplay;
		}
		else {							//如果没在播放
			char list[] = LISTNAME;
			open_mplayer(list, 1);				//重新打开mplayer
		}
		socket->close();
		break;
	}
	case 2 : {			//停止
		if (!state) {
			socket->close();
			break;
		}
		prc->write("stop\n");			//停止,结束进程
		socket->close();
		delete prc;						//释放prc
		prc = NULL;
		//state = false;			//状态改为停止

		break;
	}
	case 3 : {			//上一文件
		if (state)
			prc->write("pt_step -1\n");

		socket->close();
		break;
	}
	case 4 : {			//下一文件
		if (state)
			prc->write("pt_step 1\n");

		socket->close();
		break;
	}
	case 5 : {			//播放定位控制
		if (state) {
			QByteArray cmd("seek ");
			cmd += buf;
			cmd += "\n";
			prc->write(cmd);
		}

		socket->close();
		break;
	}

	case 9 : {			//静音开关
		if (!state) {
			socket->close();
			break;
		}
		if (issoundenable) {			//判断当前是否静音状态
			prc->write("mute 1\n");		//静音
			issoundenable = false;		//更改静音标志变量
		}
		else {
			prc->write("mute 0\n");
			issoundenable = true;
		}
		socket->close();
		break;
	}
	case 10 : {			//设置音量大小
		if (!state) {
			socket->close();
			break;
		}

		QByteArray cmd("volume ");
		cmd += buf;
		cmd += " 1\n";
		//qDebug() << "cmd=" << cmd;
		prc->write(cmd);

		socket->close();
		break;
	}
	case 11 : {			//设置循环(0, 无限循环, -1不循环, 正数:循环指定次数)
		if (!state) {
			socket->close();
			break;
		}
		QByteArray cmd("loop ");
		cmd += buf;
		cmd += " 1\n";		//默认没1参数是在原循环上加减, 无意义就没做此功能, 直接设置绝对值
		prc->write(cmd);

		socket->close();
		break;
	}
	case 12 : {			//播放速度, 在原速度上加减, 如加1就相当于2倍速度, 支持小数
		if (!state) {
			socket->close();
			break;
		}
		QByteArray cmd("speed_incr ");
		cmd += buf;
		cmd += '\n';
		prc->write(cmd);

		socket->close();
		break;
	}
	case 13 : {			//播放速度, 倍数方式, 2表示2倍速速度, 相当于加减方式加1
		if (!state) {
			socket->close();
			break;
		}
		QByteArray cmd("speed_mult ");
		cmd += buf;
		cmd += '\n';
		prc->write(cmd);

		socket->close();
		break;
	}
	case 14 : {			//播放速度, 固定值方式, 1表示原始速度
		if (!state) {
			socket->close();
			break;
		}
		QByteArray cmd("speed_set ");
		cmd += buf;
		cmd += '\n';
		prc->write(cmd);

		socket->close();
		break;
	}
	case 15 : {			//停止当前播放, 加载新的播放列表并播放列表中第一文件;
		if (state) {
			FILE *fp;
			QByteArray pathname("./list/");

			pathname += buf;

			if ((fp = fopen(pathname, "r")) == NULL) {	//如果指定列表文件不存在
				socket->close();
				break;
			}
			fclose(fp);

			prc->kill();

			delete prc;
			open_mplayer(pathname.data(), 1);
		}

		socket->close();
		break;
	}
	case 16 : {			//追加本播放列表到当前播放之后, 如果正在播放的是列表中文件,就没效果,单文件或用命令18加载的文件就有效果,FIXME;
		if (state) {
			QByteArray cmd("loadlist ");
			cmd += buf;
			cmd += " 1\n";

			prc->write(cmd);
		}

		socket->close();
		break;
	}
	case 17 : {			//将文件添加到播放列表, 对从列表文件中载入的文件无效, 如果加载的是单个文件或 命令18载入的文件, 才有效果, FIXME
		if (state) {
			QByteArray cmd("loadfile ");
			cmd += buf;
			cmd += " 1\n";
			prc->write(cmd);
		}

		socket->close();
		break;
	}
	case 18 : {			//播放指定文件, 停止原来播放状态,但不会回头播放原列表,播放完后如果没有设置循环或追加新的文件或列表,进程结束
		if (!state) {
			socket->close();
			break;
		}

		FILE *fp;
		QByteArray pathname("./video/");		//自动添加路径，只在此路径下寻找

		pathname += buf;
		if ((fp = fopen(pathname, "r")) == NULL) {	//指定文件不存在
			socket->close();
			break;
		}
		fclose(fp);

		qDebug() << "kill";
		prc->kill();
		//prc->waitForFinished(-1);
		//prc->start("./mplayer /home/zcb/windows_shared/music/hd.mp4");
		delete prc;
		open_mplayer(pathname.data(), 0);

		socket->close();
		break;
	}
	case 19 : {			//获取当前播放列表, 没办法从mplayer中获取, 采用的是直接打开建立的列表文件, 不太好
		QByteArray pathname("./list/");
		pathname += buf;
		FILE *fp = fopen(pathname.data(), "r");

		if (NULL == fp) {
			socket->write("打开播放列表失败");
			socket->close();
			break;
		}
		char *p = new char[1024];
		int n = 1;

		while (fgets(p, 1024, fp) != NULL) {	//读取文件内容并发送
			socket->write(QByteArray::number(n++));		//序号
			socket->write(":");
			socket->write(p);
			socket->write("<br>");					//网页换行符
		}

		delete[] p;
		socket->close();
		break;
	}
//字幕相关命令处理
	case 20 : {			//字幕当前信息
		QString content = text->text->text();	//得到文字内容
		int r, g, b, a;					//存储颜色信息
		text->text_color.getRgb(&r, &g, &b, &a);	//取颜色值
		buf.clear();					//清空buf先前内容
		buf = "当前字幕状态信息:<br>";
		buf += "内容: ";
		buf += content;
		buf += "<br>字体: ";
		buf += text->text->fontInfo().family();		//获取字体信息
		buf += "<br>字号: ";
		buf += QByteArray::number(text->text_size);	//转换数字为文本
		buf += "<br>颜色信息:<br> ";
		buf += "\tR(";
		buf += QByteArray::number(r);
		buf += "), G(";
		buf += QByteArray::number(g);
		buf += "), B(";
		buf += QByteArray::number(b);
		buf += "), 透明度(";
		buf += QByteArray::number(a);
		buf += ")<br>";

		int c, m, y, k;
		text->text_color.getCmyk(&c, &m, &y, &k, &a);
		buf += "\tC(";
		buf += QByteArray::number(c);
		buf += "), M(";
		buf += QByteArray::number(m);
		buf += "), Y(";
		buf += QByteArray::number(y);
		buf += "), K(";
		buf += QByteArray::number(k);
		buf += "), 透明度(";
		buf += QByteArray::number(a);
		buf += ")<br>";

		//qDebug() << "buf=" << buf;
		socket->write(buf);				//写回到客户端
		socket->close();				//关闭连接
		break;
	}
	case 21 : {			//改字幕文字内容
		text->setmsg(buf);
		socket->close();
		break;
	}
	case 22 : {			//改字号
		text->setfontsize(buf.toInt());
		socket->close();
		break;
	}
	case 23 : {			//改字体
		char *p = buf.data();
		text->setfontfamily(p);
		socket->close();
		break;
	}
	case 24 : {			//改字颜色RGB模式
		int r, g, b, a;

		comm_pos = buf.indexOf('&');					//取r值
		if ((r = buf.left(comm_pos).toInt()) > 255)
			r = 255;		//超出范围后自动取颜色最大值

		buf.remove(0, comm_pos + 1);
		comm_pos = buf.indexOf('&');
		if ((g = buf.left(comm_pos).toInt()) > 255)		//取g值
			g = 255;

		buf.remove(0, comm_pos + 1);
		comm_pos = buf.indexOf('&');
		if ((b = buf.left(comm_pos).toInt()) > 255)		//取g值
			b = 255;

		buf.remove(0, comm_pos + 1);
		if ((a = buf.toInt()) > 255)					//取a值
			a = 255;

		//qDebug() << "r=" << r << "g=" << g << "b=" << b << "a=" << a;
		text->setcolor(r, g, b, a);

		socket->close();
		break;
	}
	case 25 : {			//改字颜色CMYK模式
		int c, m, y, k, a;

		comm_pos = buf.indexOf('&');			//取c值
		if ((c = buf.left(comm_pos).toInt()) > 255)
			c = 255;

		buf.remove(0, comm_pos + 1);
		comm_pos = buf.indexOf('&');
		if ((m = buf.left(comm_pos).toInt()) > 255)			//取m值
			m = 255;

		buf.remove(0, comm_pos + 1);
		comm_pos = buf.indexOf('&');
		if ((y = buf.left(comm_pos).toInt()) > 255)			//取y值
			y = 255;

		buf.remove(0, comm_pos + 1);
		comm_pos = buf.indexOf('&');
		if ((k = buf.left(comm_pos).toInt()) > 255)			//取k值
			k = 255;

		buf.remove(0, comm_pos + 1);
		if ((a = buf.toInt()) > 255)						//取a值
			a = 255;

		qDebug() << "c=" << c << "m=" << m << "y=" << y << "k=" << k << "a=" << a;
		text->setcolor(c, m, y, k, a);

		socket->close();
		break;
	}
	case 26 : {			//动态/静态显示开关
		text->change_display_mode();
		socket->close();
		break;
	}
	case 27 : {			//更改滚动速度, 越快, 占用CPU越高
		text->texttimer->start(buf.toInt());
		socket->close();
		break;
	}
	case 28 : {			//平滑率, 越大越平滑, 占用CPU越高
		float a = buf.toFloat();

		if (a <= 0)
			a = 1;
		if (a > 32)
			a = 32;

		text->smoothness = a;
		socket->close();
		break;
	}
	case 29 : {			//显示/隐藏
		text->change_show_hide();
		socket->close();
		break;
	}
//时钟控制区
	case 31 : {			//返回广告机当前时间
		buf = "<font size=4 color=#ff0000><b>广告终端当前时间--->";
		buf.append(QTime::currentTime().toString(lcd->format));	//QString转化为QByteArray,我只想到这个方法
		buf += "</b></font><br>";
		socket->write(buf);
		qDebug() << ("buf=") << buf;

		socket->close();
		break;
	}
	case 32 : {			//显示/隐藏LCD
		lcd->change_hide();

		socket->close();
		break;
	}
	case 33 : {			//关闭/加载LCD
		if (lcd_isload) {
			delete lcd;
			lcd_isload = false;
		}
		else {
			lcd = new Lcdtime(8, this);
			lcd->show();
			lcd_isload = true;
		}

		socket->close();
		break;
	}
	case 34 : {			//更改间隔符号
		char ch = buf[0];
		lcd->change_symbol(ch);

		socket->close();
		break;
	}
	case 35 : {			//更改文字颜色RGB模式
		int r, g, b, a, flag;

		comm_pos = buf.indexOf('&');			//取r值
		if ((r = buf.left(comm_pos).toInt()) > 255)
			r = 255;

		buf.remove(0, comm_pos + 1);
		comm_pos = buf.indexOf('&');
		if ((g = buf.left(comm_pos).toInt()) > 255)			//取g值
			g = 255;

		buf.remove(0, comm_pos + 1);
		comm_pos = buf.indexOf('&');
		if ((b = buf.left(comm_pos).toInt()) > 255)			//取g值
			b = 255;

		buf.remove(0, comm_pos + 1);
		comm_pos = buf.indexOf('&');
		if ((a = buf.left(comm_pos).toInt()) > 255)			//取a值
			a = 255;

		buf.remove(0, comm_pos + 1);
		flag = buf.toInt();						//取模式

		lcd->text_color.setRgb(r, g, b, a);
		lcd->set_foreground_color(lcd->text_color, flag);

		socket->close();
		break;
	}
	case 36 : {			//更改文字颜色CMYK模式
		int c, m, y, k, a, flag;

		comm_pos = buf.indexOf('&');			//取c值
		if ((c = buf.left(comm_pos).toInt()) > 255)
			c = 255;

		buf.remove(0, comm_pos + 1);
		comm_pos = buf.indexOf('&');
		if ((m = buf.left(comm_pos).toInt()) > 255)			//取m值
			m = 255;

		buf.remove(0, comm_pos + 1);
		comm_pos = buf.indexOf('&');
		if ((y = buf.left(comm_pos).toInt()) > 255)			//取y值
			y = 255;

		buf.remove(0, comm_pos + 1);
		comm_pos = buf.indexOf('&');
		if ((k = buf.left(comm_pos).toInt()) > 255)			//取k值
			k = 255;

		buf.remove(0, comm_pos + 1);
		comm_pos = buf.indexOf('&');
		if ((a = buf.left(comm_pos).toInt()) > 255)			//取a值
			a = 255;

		buf.remove(0, comm_pos + 1);
		flag = buf.toInt();									//取模式

		lcd->text_color.setCmyk(c, m, y, k, a);
		lcd->set_foreground_color(lcd->text_color, flag);
		//qDebug() << "c=" << c << "m=" << m << "y=" << y << "k=" << k << "a=" << a << "flag=" << flag;

		socket->close();
		break;
	}
	case 37 : {			//更改边框样式
		lcd->set_frame_mode(buf.toInt());

		socket->close();
		break;
	}
	case 38 : {					//设置背景模式
		if (buf == "1")			//自定义背景
			lcd->set_background_mode(true);
		else					//不允许自定义背景
			lcd->set_background_mode(false);

		socket->close();
		break;
	}

//图片1控制区(右中图片)相关命令处理
	case 40 : {			//显示图片状态信息
		if (!pic1_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		buf = "图片总数: ";
		buf += QByteArray::number(pic1->pic_count);
		buf += "张<br>当前显示: 第";
		buf += QByteArray::number(pic1->pic_cur + 1);
		buf += "张<br>停留时间: ";
		buf += QByteArray::number(pic1->stop_time);
		buf += "毫秒<br>动态显示: ";
		if (pic1->dynamic_state())
			buf += "是<br>";
		else
			buf += "否<br>";
		buf += "图片路径: ";
		buf += PICTURE1_PATH;		//图片路径宏
		buf += "<br>图片名列表:<br>";
		socket->write(buf);

		int i = 0;

		for (i = 0; i < pic1->pic_count; i++) {			//循环遍历文件名
			socket->write(QByteArray::number(i+1));		//打印标号, 从1开始打印
			socket->write(" : ");
			buf.clear();
			buf.append(pic1->stringlist->at(i));		//转化为QByteArray
			socket->write(buf);
			socket->write("<br>");			//网页中的换行符
		}

		socket->close();
		break;
	}
	case 41 : {			//更改图片幻灯片停留时间
		if (!pic1_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		if (pic1_isshow)
			pic1->setstoptime(buf.toInt());
		socket->close();
		break;
	}
	case 42 : {			//是否动态切换图片
		if (!pic1_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		if (pic1_isshow)
			pic1->change_dynamic();
		socket->close();
		break;
	}
	case 43 : {			//指定显示哪张图片
		if (!pic1_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		int n = buf.toInt();

		if (n < pic1->pic_count && n > 0 && pic1_isshow)
			pic1->showpicture(n);

		socket->close();
		break;
	}
	case 44 : {			//显示上一张或下一张, 根据参数是-1或1决定
		if (!pic1_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		int n = buf.toInt();
		if (pic1_isshow)
			pic1->showprev_next_picture(n);

		socket->close();
		break;
	}
	case 45 : {			//显示/隐藏图片1(pic1 右下角)
		if (!pic1_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}
		if (pic1_isshow) {	//是否已关闭
			pic1->hide();
			pic1_isshow = !pic1_isshow;
		}
		else {
			pic1->show();
			pic1_isshow = !pic1_isshow;
		}

		socket->close();
		break;
	}
	case 46 : {			//重新加载图片区1
		if (pic1_isload)
			delete pic1;
		else {
			pic1 = new Picture(this, 1);
			pic1->show();
			pic1_isshow = true;
		}
		pic1_isload = !pic1_isload;

		socket->close();
		break;
	}
//图片2(右下角)相关命令处理
	case 50 : {			//显示图片状态信息
		if (!pic2_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		buf = "图片总数: ";
		buf += QByteArray::number(pic2->pic_count);
		buf += "张<br>当前显示: 第";
		buf += QByteArray::number(pic2->pic_cur + 1);
		buf += "张<br>停留时间: ";
		buf += QByteArray::number(pic2->stop_time);
		buf += "毫秒<br>动态显示: ";
		if (pic2->dynamic_state())
			buf += "是<br>";
		else
			buf += "否<br>";
		buf += "图片路径: ";
		buf += PICTURE2_PATH;		//图片路径宏
		buf += "<br>图片名列表:<br>";
		socket->write(buf);

		int i = 0;

		for (i = 0; i < pic2->pic_count; i++) {			//循环遍历文件名
			socket->write(QByteArray::number(i+1));		//打印标号, 从1开始打印
			socket->write(" : ");
			buf.clear();
			buf.append(pic2->stringlist->at(i));		//转化为QByteArray
			socket->write(buf);
			socket->write("<br>");			//网页中的换行符		
		}

		socket->close();
		break;
	}
	case 51 : {			//更改图片幻灯片停留时间
		if (!pic2_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		if (pic2_isshow)
			pic2->setstoptime(buf.toInt());
		socket->close();
		break;
	}
	case 52 : {			//是否动态切换图片
		if (!pic2_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		if (pic2_isshow)
			pic2->change_dynamic();
		socket->close();
		break;
	}
	case 53 : {			//指定显示哪张图片
		if (!pic2_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		int n = buf.toInt();

		if (n < pic2->pic_count && n > 0 && pic2_isshow)
			pic2->showpicture(n);

		socket->close();
		break;
	}
	case 54 : {			//显示上一张或下一张, 根据参数是-1或1决定
		if (!pic2_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		int n = buf.toInt();
		if (pic2_isshow)
			pic2->showprev_next_picture(n);

		socket->close();
		break;
	}
	case 55 : {			//显示/隐藏图片2(pic2 右下角)
		if (!pic2_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}
		if (pic2_isshow) {	//是否已关闭
			pic2->hide();
			pic2_isshow = !pic2_isshow;
		}
		else {
			pic2->show();
			pic2_isshow = !pic2_isshow;
		}

		socket->close();
		break;
	}
	case 56 : {			//重新加载图片区2
		if (pic2_isload)
			delete pic2;
		else {
			pic2 = new Picture(this, 2);
			pic2->show();
			pic2_isshow = true;
		}
		pic2_isload = !pic2_isload;

		socket->close();
		break;
	}
//图片3(右上角)相关命令处理
	case 60 : {			//显示图片状态信息
		if (!pic3_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		buf = "图片总数: ";
		buf += QByteArray::number(pic3->pic_count);
		buf += "张<br>当前显示: 第";
		buf += QByteArray::number(pic3->pic_cur + 1);
		buf += "张<br>停留时间: ";
		buf += QByteArray::number(pic3->stop_time);
		buf += "毫秒<br>动态显示: ";
		if (pic3->dynamic_state())
			buf += "是<br>";
		else
			buf += "否<br>";
		buf += "图片路径: ";
		buf += PICTURE3_PATH;		//图片路径宏
		buf += "<br>图片名列表:<br>";
		socket->write(buf);

		int i = 0;

		for (i = 0; i < pic3->pic_count; i++) {			//循环遍历文件名
			socket->write(QByteArray::number(i+1));		//打印标号, 从1开始打印
			socket->write(" : ");
			buf.clear();
			buf.append(pic3->stringlist->at(i));		//转化为QByteArray
			socket->write(buf);
			socket->write("<br>");			//网页中的换行符
		}

		socket->close();
		break;
	}
	case 61 : {			//更改图片幻灯片停留时间
		if (!pic3_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		if (pic3_isshow)
			pic3->setstoptime(buf.toInt());
		socket->close();
		break;
	}
	case 62 : {			//是否动态切换图片
		if (!pic3_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		if (pic3_isshow)
			pic3->change_dynamic();
		socket->close();
		break;
	}
	case 63 : {			//指定显示哪张图片
		if (!pic3_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		int n = buf.toInt();

		if (n < pic3->pic_count && n > 0 && pic3_isshow)
			pic3->showpicture(n);

		socket->close();
		break;
	}
	case 64 : {			//显示上一张或下一张, 根据参数是-1或1决定
		if (!pic3_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}

		int n = buf.toInt();
		if (pic3_isshow)
			pic3->showprev_next_picture(n);

		socket->close();
		break;
	}
	case 65 : {			//显示/隐藏图片3(pic3 右上角)
		if (!pic3_isload) {		//如果当前没有加载, 不做任何操作
			socket->close();
			break;
		}
		if (pic3_isshow) {	//是否已关闭
			pic3->hide();
			pic3_isshow = !pic3_isshow;
		}
		else {
			pic3->show();
			pic3_isshow = !pic3_isshow;
		}

		socket->close();
		break;
	}
	case 66 : {			//重新加载图片区3
		if (pic3_isload)
			delete pic3;
		else {
			pic3 = new Picture(this, 3);
			pic3->show();
			pic3_isshow = true;
		}
		pic3_isload = !pic3_isload;

		socket->close();
		break;
	}
	case 100 : {		//获取播放状态信息
		prc->readAllStandardOutput();
		connect(prc, SIGNAL(readyReadStandardOutput()), this, SLOT(read_status_msg()));
		prc->write("get_file_name\n");
		prc->write("get_time_length\n");
		prc->write("get_time_pos\n");
		prc->write("get_percent_pos\n");
		prc->write("get_video_bitrate\n");
		prc->write("get_video_resolution\n");

		break;
	}


	}
	//buf.clear();
}

