/* 字幕控制CGI程序 工作流程:
	1. 从数据库读到广告机ID号
	2. 根据广告机ID号再从数据库找到对应IP地址
	3. 解析表单命令与参数
	4. 根据IP地址连接广告机服务器
	5. 根据表单命令发送相应命令到广告机
	6. 根据表单命令判断是否要接收广告机发回数据 
	7. 将结果回送到浏览器
zcb 2012.6.27 */

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
#define GET_PUT_PORT 8001	//本CGI不会与文件服务器通讯

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
	//printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/htm\"></head><body>");
	//	printf("data=%s<br>", data);
	
	/*----- 根据按钮命令转换为与广告机通讯相应的命令,应该也做到数据库中, 
		节省代码量也能提高效率, 由于时间有限, 以后处理//FIXME -----*/ 
	data = cgi_param("Button");			//按下的按钮名字
	argument = cgi_param("Name");

	if (!strcmp(data, "更改文字")) {
		strcpy(buff, "21&");
		if (argument)
			strcat(buff, argument);
		else {
			printf("<font size=4 color=#ff0000><b>请输入文字内容</b></font><br>");	
			return 0;
		}
	}

	else if (!strcmp(data, "更改字号")) {
		strcpy(buff, "22&");
		if (argument)
			strcat(buff, argument);
		else {
			printf("<font size=4 color=#ff0000><b>请输入字号,\
			当窗口大小改变时,字号会随之改变</b></font><br>");	
			return 0;
		}
	}

	else if (!strcmp(data, "更改字体")) {
		strcpy(buff, "23&");
		if (argument)
			strcat(buff, argument);
		else {
			printf("<font size=4 color=#ff0000><b>请输入字体,\
			如果广告机中没有此字体,则不会改变,有些中文字体存储的名字不一定是中文的,\
			如楷体字体名:\"AR PL UKai CN\"</b></font><br>");	
			return 0;
		}
	}

	else if (!strcmp(data, "设置RGB")) {
		strcpy(buff, "24&");
		char *color_r = cgi_param("R");
		char *color_g = cgi_param("G");
		char *color_b = cgi_param("B");
		char *color_a = cgi_param("A");
			
		strcat(buff, color_r ? color_r : "0");
		strcat(buff, "&");
		strcat(buff, color_g ? color_g : "0");
		strcat(buff, "&");
		strcat(buff, color_b ? color_b : "0");
		strcat(buff, "&");
		strcat(buff, color_a ? color_a : "255");
		//printf("buff = %s<br>", buff);
		if (color_a && strtol(color_a, NULL, 10) < 10) {
			printf("<font size=4 color=#ff0000><b>注意:透明值太小,您可能看不到任何文字内容</b></font><br>");	
		}
	}

	else if (!strcmp(data, "设置CMYK")) {
		strcpy(buff, "25&");
		char *color_c = cgi_param("C");
		char *color_m = cgi_param("M");
		char *color_y = cgi_param("Y");
		char *color_k = cgi_param("K");
		char *color_a = cgi_param("A");

		strcat(buff, color_c ? color_c : "0");
		strcat(buff, "&");
		strcat(buff, color_m ? color_m : "0");
		strcat(buff, "&");
		strcat(buff, color_y ? color_y : "0");
		strcat(buff, "&");
		strcat(buff, color_k ? color_k : "0");
		strcat(buff, "&");
		strcat(buff, color_a ? color_a : "255");
		//printf("buff = %s<br>", buff);
		if (color_a && strtol(color_a, NULL, 10) < 10) {
			printf("<font size=4 color=#ff0000><b>注意:透明值太小,您可能看不到任何文字内容</b></font><br>");	
		} 
	}
	
	else if (!strcmp(data, "当前字幕信息")) {
		command = 20;
		strcpy(buff, "20&");
	}
	
	else if (!strcmp(data, "动态/静态"))
		strcpy(buff, "26&");
	
	else if (!strcmp(data, "滚动速度")) {
		strcpy(buff, "27&");
		if (argument)
			strcat(buff, argument);
		else {
			printf("<font size=4 color=#ff0000><b>请输入速度值,单位毫秒,\
			数值越小,滚动速度越快,但占用CPU越高</b></font><br>");	
			return 0;
		}
	}
	
	else if (!strcmp(data, "平滑度")) {
		strcpy(buff, "28&");
		if (argument)
			strcat(buff, argument);
		else {
			printf("<font size=4 color=#ff0000><b>请输入平滑度,即每次移动\
				整个字比例,1:每字移动1/1个字位置,8:每次移动1/8个字位置,\
				数字越大,移动越慢,越平滑</b></font><br>");	
			return 0;
		}
	}

	else if (!strcmp(data, "显示/隐藏")) {
		strcpy(buff, "29&");
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

	if (command == 20) {		//当前字幕信息
		n = recv(socket_fd, buff, BUFFER_SIZE - 1, 0);
		buff[n] = '\0';
		printf("%s", buff);
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
