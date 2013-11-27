/* 图片3区(右上角)控制CGI程序 工作流程:
	1. 从数据库读到广告机ID号
	2. 根据广告机ID号再从数据库找到对应IP地址
	3. 解析表单命令与参数
	4. 根据IP地址连接广告机服务器
	5. 根据表单命令发送相应命令到广告机
	6. 根据表单命令判断是否要接收广告机发回数据 
	7. 将结果回送到浏览器
zcb 2012.6.29 */

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
#define GET_PUT_PORT 8001	//文件操作相关要连接端口

#define BUFFER_SIZE 4096

/*--------------------------------------------------------------------------------*/ 
/*----- 全局变量 -----*/
/*--------------------------------------------------------------------------------*/ 
char server_ip[16];			//存储从数据库读出的ip地址
int current_id = 0;			//记录当前操作的广告机id

/*--------------------------------------------------------------------------------*/ 
/*----- 函数声明 -----*/
/*--------------------------------------------------------------------------------*/ 
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
	char *argument;
	int fd;

	/*----- 初始化 -----*/
	cgi_init();
	cgi_process_form();
	memset(buff, 0, sizeof(buff));
	//	printf("data=%s<br>", data);
	
	/*----- 根据按钮命令转换为与广告机通讯相应的命令,应该也做到数据库中, 
		节省代码量也能提高效率, 由于时间有限, 以后处理//FIXME -----*/ 
	data = cgi_param("Button");			//按下的按钮名字
	argument = cgi_param("Name");

	if (!strcmp(data, "状态信息")) {
		command = 60;
		strcpy(buff, "60&");
	}
	
	else if (!strcmp(data, "更改停留时间")) {		
		strcpy(buff, "61&");
		if (!argument) {
			printf("<fo nt size=4 color=#ff0000><b><center>输入时间,单位-->毫秒\
					</center></b></font><br>");
			return 0;
		}

		strcat(buff, argument);
	}
	
	else if (!strcmp(data, "动态显示")) 
		strcpy(buff, "62&");
	
	else if (!strcmp(data, "显示这张")) {
		strcpy(buff, "63&");
		if (!argument) {
			printf("<font size=4 color=#ff0000><b><center>输入图片编号\
					</center></b></font><br>");
			return 0;
		}
		strcat(buff, argument);
	}
	
	else if (!strcmp(data, "上一张")) {
		strcpy(buff, "64&");
		strcat(buff, "-1");
	}
	
	else if (!strcmp(data, "下一张")) {
		strcpy(buff, "64&");
		strcat(buff, "1");
	}
	
	else if (!strcmp(data, "显示/隐藏")) {
		strcpy(buff, "65&");
	}

	else if (!strcmp(data, "关闭/加载")) {
		strcpy(buff, "66&");
	}

	else if (!strcmp(data, "上传图片")) {
		command = 201;
		strcpy(buff, "PUT&PICTURE3&");
		if (!argument) {
			printf("<font size=4 color=#ff0000><b>输入图片文件名,\
				目前只支持从Web服务器到广告机,无法从监控终端上传或下载,\
				路径只会对Web服务器文件起作用,\
				广告机会根据文件类型自动到相应目录下查找文件.\
				</b></ font><br>");
			return 0; 
		}
		if ((fd = open(argument, O_RDONLY)) < 0) {
			printf("<font size=4 color=#ff0000><b>无法打开文件名,\
			请确定路径名是否正确</b></font><br>");
			return 0;	 
		}

		strcat(buff, argument);
	}

	else if (!strcmp(data, "下载图片")) {
		command = 202;
		strcpy(buff, "GET&PICTURE3&");
		if (!argument) {
			printf("<font size=4 color=#ff0000><b>输入图片文件名,\
				目前只支持从Web服 务器到广告机,无法从监控终端上传或下载,\
				路径只会对Web服务器文件起作用,\
				广告机会根据文件类型自动到相应目录下查找文件.\
				</b></font><br>");
			return 0; 
		}
		strcat(buff, argument); 
	}

	else if (!strcmp(data, "删除图片")) {
		command = 203;
		strcpy(buff, "DEL&PICTURE3&");
		if (!argument) {
			printf("<font size=4 color=#ff0000><b>输入要删除的图片文件名!\
				</b></font><br>");
			return 0; 
		}
		strcat(buff, argument); 
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

	if (command > 200)
		ipv4_addr.sin_port = htons(GET_PUT_PORT);
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
	n = send(socket_fd, buff, strlen(buff), 0);

	if (command == 60) {
		n = recv(socket_fd, buff, BUFFER_SIZE - 1, 0);
		buff[n] = '\0';
		printf("%s<br>", buff);
	}

	if (command == 201) {		//上传文件
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
/*----- 回调函数,取得当前操作广告机IP值 -----*/
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
