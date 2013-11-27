/* 视频控制CGI程序 工作流程:
	1. 从数据库读到广告机ID号
	2. 根据广告机ID号再从数据库找到对应IP地址
	3. 解析表单命令与参数
	4. 根据IP地址连接广告机服务器
	5. 根据表单命令发送相应命令到广告机
	6. 根据表单命令判断是否要接收广告机发回数据 
	7. 将结果回送到浏览器
zcb 2012.6.28 */

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <cgi.h>
#include <sqlite3.h>

/*--------------------------------------------------------------------------------*/ 
/*----- 宏定义 -----*/
/*--------------------------------------------------------------------------------*/ 
#define SERVER_PORT 8000	//QT程序绑定的端口, 用于发送控制命令
#define GET_PUT_PORT 8001	//文件上传下载等用到的连接端口

#define BUFFER_SIZE 4096

/*--------------------------------------------------------------------------------*/ 
/*----- 全局变量 -----*/
/*--------------------------------------------------------------------------------*/ 
char server_ip[16];			//存储从数据库读出的ip地址
int current_id = 0;			//记录当前操作的广告机id

/*--------------------------------------------------------------------------------*/ 
/*----- 函数声明 -----*/
/*--------------------------------------------------------------------------------*/ 
void parse_status(char *);					//解析广告机发出的当前视状态信息
int callback(void *, int, char**, char**);	//数据库回调函数

/*--------------------------------------------------------------------------------*/ 
/*----- 主函数 -----*/
/*--------------------------------------------------------------------------------*/ 
int main(int argc, char *argv[])
{
	/*----- HTML表头 -----*/
	cgi_init_headers();
	puts("<html><head>");			
	puts("<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\">");
	puts("</head><body>");

	/*----------------------------------------------------------------------------*/ 
	/*----- 读取数据库获得当前操作广告机ID并根据ID获得广告机IP地址 -----*/
	/*----------------------------------------------------------------------------*/ 
	char *sql;
	sqlite3 *db;
	int code = 1;				//标志变量
	char buff[BUFFER_SIZE];
	
	if (sqlite3_open("./data.db", &db) != SQLITE_OK) {		//读取数据库失败
		printf("<font size=5 color=#ff0000><b><center>读取数据库失败</center></b></font><br>");
		exit(EXIT_FAILURE);
	}

	sql = sqlite3_mprintf("select *from ip where name=%Q", "current_ip");	//合并字符串
	sqlite3_exec(db, sql, callback, &code, NULL);			//执行查找语句,获得当前操作的广告机
	
	if (code != 0) {			//回调函数没有执行, 没找到当前ip值
		printf("<font size=5 color=#ff0000><b><center>要操作的广告机不存在</center></b></font><br>");
		exit(EXIT_FAILURE);
	}

	//printf("server_ip = %s\n<br>", server_ip);
	sqlite3_free(sql);			//释放sql
	current_id = (int)strtol(server_ip, NULL, 10);	//取出当前操作的广告机id, 存在全局变量中

	sprintf(buff, "server_ip%s", server_ip);		//组成server_ip1这样的值, 为了下次查找ip值	
	//printf("buff = %s<br>", buff);
	sql = sqlite3_mprintf("select *from ip where name=%Q", buff);	//合并字符串
	sqlite3_exec(db, sql, callback, &code, NULL);			//执行查找找到当前操作广告机ip值
	sqlite3_free(sql);			//释放sql

	if (code != 0) {			//回调函数没有执行, 没有找到对应的ip地址
		printf("<font size=5 color=#ff0000><b><center>广告机不存在,可能最近执行了删除广告机操作</center></b></font><br>");
		exit(EXIT_FAILURE);
	}
	sqlite3_close(db);				//关闭数据库
	//printf("server_ip = %s<br>", server_ip);

	/*----------------------------------------------------------------------------*/ 
	/*----- 解析表单 -----*/
	/*----------------------------------------------------------------------------*/ 
	unsigned char command = 0;
	char *data = NULL;
	char *argument = NULL;
	int fd;

	cgi_init();
	cgi_process_form();
	data = cgi_param("Button");			//按下的按钮名字

	memset(buff, 0, sizeof(buff));
	//printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/htm\"></head><body>");
	//	printf("data=%s<br>", data);
	
	/*----- 根据按钮命令转换为与广告机通讯相应的命令,应该也做到数据库中, 
		节省代码量也能提高效率, 由于时间有限, 以后处理//FIXME -----*/ 
	if (!strcmp(data, "播放/暂停"))		
		strcpy(buff, "1&");

	else if (!strcmp(data, "停止"))
		strcpy(buff, "2&");

	else if (!strcmp(data, "上一文件"))
		strcpy(buff, "3&");

	else if (!strcmp(data, "下一文件"))
		strcpy(buff, "4&");

	else if (!strcmp(data, "静音")) 
		strcpy(buff, "9&");

	else if (!strcmp(data, "设置声音")) {
		strcpy(buff, "10&");
		argument = cgi_param("Name");
		if (argument)
			strcat(buff, argument);
		else
			return 0;
	}

	else if (!strcmp(data, "时间加减")) {
		strcpy(buff, "5&");
		argument = cgi_param("Name");
		if (argument)
			strcat(buff, argument);
		else
			return 0;
	}

	else if (!strcmp(data, "定位秒")) {
		strcpy(buff, "5&");
		argument = cgi_param("Name");
		if (argument) {
			strcat(buff, argument);
			strcat(buff, " 2");
		}
		else
			return 0;
	}

	else if (!strcmp(data, "百分比")) {
		strcpy(buff, "5&");
		argument = cgi_param("Name");
		if (argument) {
			strcat(buff, argument);
			strcat(buff, " 1");
		}
		else
			return 0;
	}

	else if (!strcmp(data, "设置循环")) {
		strcpy(buff, "11&");
		char *loop = cgi_param("Name");
		if (loop)
			strcat(buff, loop);
		else
			return 0;
	}

	else if (!strcmp(data, "速度加减")) {
		strcpy(buff, "12&");
		char *speed = cgi_param("Name");
		if (speed)
			strcat(buff, speed);
		else
			return 0;
	}

	else if (!strcmp(data, "速度倍数")) {
		strcpy(buff, "13&");
		char *speed = cgi_param("Name");
		if (speed)
			strcat(buff, speed);
		else
			return 0;
	}

	else if (!strcmp(data, "速度值")) {
		strcpy(buff, "14&");
		char *speed = cgi_param("Name");
		if (speed)
			strcat(buff, speed);
		else
			return 0;
	}

	else if (!strcmp(data, "加载播放列表")) {
		strcpy(buff, "15&");
		char *p = cgi_param("Name");
		if (p)
			strcat(buff, p);
		else {
			printf("请输入文件名<br>");
			return 0;
		}
	}

	else if (!strcmp(data, "追加播放列表")) {
		strcpy(buff, "16&");
		char *p = cgi_param("Name");
		if (p)
			strcat(buff, p);
		else {
			printf("请输入文件名<br>");
			return 0;
		}
	}

	else if (!strcmp(data, "添加播放")) {
		strcpy(buff, "17& ");
		char *p = cgi_param("Name");
		if (p)
			strcat(buff, p);
		else {
			printf("请输入文件名<br>");
			return 0;
		}
	}

	else if (!strcmp(data, "播放此文件")) {
		strcpy(buff, "18&");
		char *p = cgi_param("Name");
		if (p)
			strcat(buff, p);
		else {
			printf("请输入文件名<br>");
			return 0;
		}
	}

	else if (!strcmp(data, "获取列表内容")) {
		command = 19;
		strcpy(buff, "19&");
		char *p = cgi_param("Name");
		if (p)
			strcat(buff, p);
		else {
			printf("请输入文件名<br>");
			return 0;
		}
	}

	else if (!strcmp(data, "上传视频")) {			//上传视频到服务器
		command = 201;
		strcpy(buff, "PUT&VIDEO&");
		argument = cgi_param("Name");
		if (argument)
			strcat(buff, argument);
		else {
			printf("<font size=4 color=#ff0000><b>输入文件名,\
				目前只支持从Web服务器与广告机上传与下载,无法从监控终端上传下载,\
				路径只会对Web服务器文件起作用,广告机根据文件类型自动到相应目录下查找文件.\
				</b></font><br>");
			return 0;
		}

		fd = open(argument, O_RDONLY);
		if (fd < 0) {
			printf("<font size=4 color=#ff0000><b>Web服务器找不到该文件,无法上传!\
				</b></font><br>");
			return 0;
		}
	}

	else if (!strcmp(data, "下载视频")) {			//上传视频到服务器
		command = 202;
		strcpy(buff, "GET&VIDEO&");
		argument = cgi_param("Name");

		if (argument)
			strcat(buff, argument);
		else {
			printf("<font size=4 color=#ff0000><b>输入文件名,\
				目前只支持从Web服务器与广告机上传与下载,无法从监控终端上传下载,\
				路径只会对Web服务器文件起作用,广告机根据文件类型自动到相应目录下查找文件.\
				</b></font><br>");
			return 0;
		}
	}

	else if (!strcmp(data, "上传列表")) {			//上传播放列表到服务器
		command = 201;
		strcpy(buff, "PUT&LIST&");
		argument = cgi_param("Name");

		if (argument)
			strcat(buff, argument);
		else {
			printf("<font size=4 color=#ff0000><b>输入文件名,\
				目前只支持从Web服务器与广告机上传与下载,无法从监控终端上传下载,\
				路径只会对Web服务器文件起作用,广告机根据文件类型自动到相应目录下查找文件.\
				</b></font><br>");
			return 0;
		}

		if ((fd = open(argument, O_RDONLY)) < 0) {
			printf("<font size=4 color=#ff0000><b>Web服务器找不到该文件,无法上传!\
				</b></font><br>");
			return 0;
		}
	}

	else if (!strcmp(data, "下载列表")) {			//上传播放列表到服务器
		command = 202;
		strcpy(buff, "GET&LIST&");
		argument = cgi_param("Name");

		if (argument)
			strcat(buff, argument);
		else {
			printf("<font size=4 color=#ff0000><b>输入文件名,\
				目前只支持从Web服务器与广告机上传与下载,无法从监控终端上传下载,\
				路径只会对Web服务器文件起作用,广告机根据文件类型自动到相应目录下查找文件.\
				</b></font><br>");
			return 0;
		}
	}

	else if (!strcmp(data, "删除视频")) {			//上传播放列表到服务器
		command = 203;
		strcpy(buff, "DEL&VIDEO&");
		argument = cgi_param("Name");

		if (argument)
			strcat(buff, argument);
		else {
			printf("<font size=4 color=#ff0000><b><center>输入视频文件名.\
				</center></b></font><br>");
			return 0;
		}
	}

	else if (!strcmp(data, "状态信息")) {			//获取当前播放视频信息
		command = 100;
		strcpy(buff, "100&");
	}

	/*----------------------------------------------------------------------------*/ 
	/*----- 连接广告机服务器 -----*/
	/*----------------------------------------------------------------------------*/ 
	int socket_fd;
	unsigned int port;

	/*----- 创建socket -----*/
	if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("<font size=5 color=#ff0000><b><center>创建socket失败</center></b></font><br>");
		fprintf(stderr, "Create failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in ipv4_addr;
	memset(&ipv4_addr, 0, sizeof(ipv4_addr));

	ipv4_addr.sin_family = AF_INET;

	if (command > 200)		//根据命令决定绑定哪个端口, 100以后命令规定为文件传输命令
		ipv4_addr.sin_port = htons(GET_PUT_PORT);		//连接文件系统服务器
	else
		ipv4_addr.sin_port = htons(SERVER_PORT);		//连接QT程序

	inet_pton(AF_INET, server_ip, &ipv4_addr.sin_addr);

	if (connect(socket_fd, (struct sockaddr *)&ipv4_addr, sizeof(ipv4_addr)) < 0) {
		/*----- 连接失败 -----*/
		printf("<font size=5 color=#ff0000><b><center>\"%d\"号广告终端未开启</center></b></font><br>", current_id);
		close(socket_fd);
		puts("</body></html>");				//html格式尾
		cgi_end();							//cgi结束

		return 0;
	}

	/*----- 连接成功 -----*/
	unsigned int total_size = 0;
	ssize_t n;

	/*----- 发送命令到广告机 -----*/
	send(socket_fd, buff, strlen(buff), 0);

	if (command == 19) {			//根据命令判断是否需要从广告机接收数据
		n = recv(socket_fd, buff, sizeof(buff), 0);
		buff[n] = '\0';
		printf("%s", buff);
	}

	else if (command == 100) {		//获取状态信息
		n = recv(socket_fd, buff, sizeof(buff), 0);
		buff[n] = '\0';
		parse_status(buff);
	}

	else if (command == 201) {		//上传文件
		n = recv(socket_fd, buff, sizeof(buff), 0);
		buff[n] = '\0';
		if (!strcmp(buff, "ok")) {
			while ((n = read(fd, buff, BUFFER_SIZE))) {		//发送文件
				send(socket_fd, buff, n, 0);
				total_size += n;			//记录传输总字节数
			}
			printf("<font size=5 color=#ff0000><b><center>上传成功,\
				共上传了%u字节</center></b></font><br>", total_size);
		}
		else
			printf("%s<br>", buff);		//服务器准备失败, 打印返回的失败信息
	}

	else if (command == 202) {			//下载文件
		n = recv(socket_fd, buff, sizeof(buff), 0);		//接收服务器是否准备好信息
		buff[n] = '\0';

		if (!strcmp(buff, "ok")) {		//服务器准备好发送, 等待客户端是否准备好
			/*----- 建立文件 -----*/
			if ((fd = open(argument, O_WRONLY | O_CREAT | O_TRUNC, 0777)) < 0) {
				printf("<font size=5 color=#ff0000><b><center>Web服务器创建文件失\
					败,检查路径是否正确或权限</center></b></font><br>");
				close(socket_fd);
				puts("</body></html>");				//html格式尾
				cgi_end();							//cgi结束

				return 0;
			}
			send(socket_fd, "ok", 2, 0);				//发送准备好消息

			while ((n = recv(socket_fd, buff, BUFFER_SIZE, 0)))	{	//接收文件
				write(fd, buff, n);
				total_size += n;		//记录传输总字节数
			}
			printf("<font size=5 color=#ff0000><b><center>下载成功, 共下载了%u字节\
								</center></b></font><br>", total_size);
		}
		else			//服务器打开文件失败, 可能文件未找到, 打印返回的错误信息
			printf("%s<br>", buff);
	}

	else if (command == 203) {			//删除文件
		n = recv(socket_fd, buff, sizeof(buff), 0);	//接收服务器处理结果
		buff[n] = '\0';
		
		printf("<font size=4 color=#ff0000><b><center>%s\
								</center></b></font><br>", buff);
	}

	close(socket_fd);
	puts("</body></html>");				//html格式尾
	cgi_end();							//cgi结束

	return 0;
}

/*--------------------------------------------------------------------------------*/ 
/*----- 回调函数 -----*/
/*--------------------------------------------------------------------------------*/ 
int callback(void *p, int argc, char **value, char **name)
{
	*(int *)p = 0;				//更改id值为0, 表示回调函数执行过
	//int i;
	//for (i = 0; i < argc; i++)
	//	printf("name = %s, value = %s\n<br>", name[i], value[i]);
	strcpy(server_ip, value[1]);		//全局变量server_ip存储找到的结果	

	return 0;
}

/*--------------------------------------------------------------------------------*/ 
/*----- 解析状态信息, 并发回到浏览器 -----*/
/*--------------------------------------------------------------------------------*/ 
void parse_status(char *buff)
{
	char *savep, *p, *q;
	char filename[] = "ANS_FILENAME=";
	char length[] = "ANS_LENGTH=";
	char time_pos[] = "ANS_TIME_POSITION=";
	char percent_pos[] = "ANS_PERCENT_POSITION=";
	char video_bit[] = "ANS_VIDEO_BITRATE=";
	char video_res[] = "ANS_VIDEO_RESOLUTION=";

	printf("<font size=5 color=#ff0000><b>&nbsp &nbsp &nbsp &nbsp &nbsp当前视频信息</b></font><br><br>");

	printf("<font size=4 color=#ff0000><b>&nbsp &nbsp &nbsp视频文件名:&nbsp &nbsp</b></font>");
	p = strstr(buff, filename);
	p += strlen(filename);
	q = strstr(p, length);
	*q = '\0';
	printf("<font size=4 color=#0000ff>%s</font><br>", p);

	printf("<font size=4 color=#ff0000><b>&nbsp &nbsp &nbsp文件总长度:&nbsp &nbsp</b></font>");
	q += strlen(length);
	p = strstr(q, time_pos);
	*p = '\0';
	printf("<font size=4 color=#0000ff>%s秒</font><br>", q);

	printf("<font size=4 color=#ff0000><b>&nbsp &nbsp &nbsp现播放位置:&nbsp &nbsp</b></font>");
	p += strlen(time_pos);
	q = strstr(p, percent_pos);
	*q = '\0';
	printf("<font size=4 color=#0000ff>%s秒</font>", p);

	q += strlen(percent_pos);
	p = strstr(q, video_bit);
	*p = '\0';
	printf("<font size=4 color=#0000ff>(%s%%)</font><br>", q);

	printf("<font size=4 color=#ff0000><b>&nbsp &nbsp &nbsp视频波特率:&nbsp &nbsp</b></font>");
	p += strlen(video_bit);
	q = strstr(p, video_res);
	*q = '\0';
	printf("<font size=4 color=#0000ff>%s</font><br>", p);

	printf("<font size=4 color=#ff0000><b>&nbsp &nbsp &nbsp视频分辨率:&nbsp &nbsp</b></font>");
	q += strlen(video_res);
	printf("<font size=4 color=#0000ff>%s</font><br>", q);

}
/*
ANS_FILENAME='nothingintheworld.flv' ANS_LENGTH=317.09 ANS_TIME_POSITION=68.1 ANS_PERCENT_POSITION=1 ANS_VIDEO_BITRATE='205 kbps' ANS_VIDEO_RESOLUTION='512 x 288' PLAY_STATUS=0
*/
