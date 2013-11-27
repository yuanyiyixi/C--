/* 状态检查与数据库修改CGI程序 工作流程:
   1. 读取广告机总数
   2. 读取各广告机IP地址
   3. 解析表单命令
   4. 根据表单命令修改数据库记录或连接广告机服务器
   5. 修改结果回送浏览器
   6. 本CGI不与QT程序通讯, 只与主服务器(main_server)发送控制命令 
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
#define GET_PUT_PORT 8001	//本程序绑定的端口, 用于发送接收文件或控制整个QT开关

#define BUFFER_SIZE 4096

/*--------------------------------------------------------------------------------*/ 
/*----- 全局变量 -----*/
/*--------------------------------------------------------------------------------*/ 
char **server_ip;			//存储从数据库读出的ip地址
int max_id = 0;				//记录总广告机数
int current_id = 0;			//记录当前操作的广告机

/*--------------------------------------------------------------------------------*/ 
/*----- 函数声明 -----*/
/*--------------------------------------------------------------------------------*/ 
void *thread_connect_main(void *);					//线程入口, 连接各个广告机
int callback_serverip(void *, int, char**, char**);	//数据库回调函数
int callback(void *, int, char**, char**);	//数据库回调函数
void status_check(void);					//各广告机状态检查
void add_machine(sqlite3 *, char *);		//添加广告机
void del_machine(sqlite3 *, int);			//删除广告机
void modify_machine(sqlite3 *, int, char *);	//修改广告机IP
void set_current_id(sqlite3 *, char *);		//修改当前操作广告机ID
void control_play(sqlite3 *, char *);		//控制播放机(打开,关闭,重载)

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
	/*----- 读取数据库取得广告机总数以及所有广告机IP地址 -----*/
	/*----------------------------------------------------------------------------*/ 
	char *sql;
	sqlite3 *db;
	int i, code = 1;				//标志变量
	char buff[BUFFER_SIZE];

	if (sqlite3_open("./data.db", &db) != SQLITE_OK) {		//读取数据库失败
		printf("<font size=5 color=#ff0000><b><center>读取数据库失败</center></b></font><br>");
		exit(EXIT_FAILURE);
	}

	sql = sqlite3_mprintf("select *from ip where name=%Q", "max_ip");	//合并字符串
	sqlite3_exec(db, sql, callback, &max_id, NULL);			//执行查找语句,获得当前操作的广告机
	sqlite3_free(sql);			//释放sql

	if (max_id == 0) {			//回调函数没有执行, 没找到广告机总数
		printf("<font size=5 color=#ff0000><b><center>无法获得广告机总数</center></b></font><br>");
		exit(EXIT_FAILURE);
	}

	sql = sqlite3_mprintf("select *from ip where name=%Q", "current_ip");	//合并字符串
	sqlite3_exec(db, sql, callback, &current_id, NULL);			//执行查找语句,获得当前操作的广告机
	sqlite3_free(sql);			//释放sql

	if (current_id == 0) {			//回调函数没有执行, 没找到广告机总数
		printf("<font size=5 color=#ff0000><b><center>无法获得当前操作广告机</center></b></font><br>");
		exit(EXIT_FAILURE);
	}
	//printf("max_id = %d\n<br>", max_id);


	for (i = 0; i < max_id; i++) {		//循环获取各广告机ip值
		sprintf(buff, "server_ip%d", i+1);		//组成server_ip1这样的值, 为了下次查找ip值	
		//printf("buff = %s<br>", buff);
		sql = sqlite3_mprintf("select *from ip where name=%Q", buff);	//合并字符串
		sqlite3_exec(db, sql, callback_serverip, &i, NULL);			//执行查找找到当前操作广告机ip值
		sqlite3_free(sql);			//释放sql
	}

#if 0	
	for (i = 0; i < max_id; i++) {
		printf("server_ip[%d] = %s\n<br>", i, server_ip[i]);
	}
#endif

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

	if (!strcmp(data, "广告机总览")) {		//打印广告机基本信息
		printf("<font size=4 color=#ff0000><b>&nbsp &nbsp &nbsp当前共有广告机 \
						&nbsp ---> &nbsp %d</b></font><br><br>", max_id);
		for (i = 0; i < max_id; i++) {
			printf("<font size=3 color=#0000ff>&nbsp &nbsp <b>ID--></b> %d,\
							<b>地址--></b>%s</font><br>", i+1, server_ip[i]);
		}
		printf("<br><br><font size=4 color=#ff00000><b> &nbsp &nbsp\
		当前控制广告机ID: &nbsp ---> &nbsp %d</b></font><br>", current_id);
	}

	if (!strcmp(data, "状态检查")) {		//检查广告机工作状态
		status_check();	
	}

	else if (!strcmp(data, "添加广告机")) {		//添加广告机
		argument = cgi_param("IP");	
		if (!argument) {
			printf("<font size=5 color=#ff0000><b>请输入广告机IP,ID不会读取,\
				系统自动追加到最后ID</b></font><br>");
			return 0;
		}

		/*-----  检查IP地址? FIXME -----*/
		for (i = 0; i < max_id; i++) {
			if (!strcmp(server_ip[i], argument)) {
				printf("<font size=5 color=#ff0000><b><center> IP已存在,请重新输入!\
					</center></b></font><br>");
				return 0;
			}
		}
		add_machine(db, argument);				//执行添加
	}

	else if (!strcmp(data, "删除广告机")) {		//删除某个广告机IP
		argument = cgi_param("ID");
		if (!argument) {
		printf("<font size=5 color=#ff0000><b>请输入广告机ID,系统根据ID删除,\
				不读取IP,删除后,后面广告机ID号依次向前移一位!</b></font><br>");
			return 0;
		}
		int id = (int)strtol(argument, NULL, 10);

		if (id < 1 || id > max_id) {			//超出范围
			printf("<font size=5 color=#ff0000><b><center> ID不在范围,请重新输入\
				</center></b></font><br>");
			return 0;
		}
		del_machine(db, id);				//执行删除
	}

	else if (!strcmp(data, "修改IP")) {			//修改指定ID广告机的IP地址;
		char *modify_id = cgi_param("ID");
		argument = cgi_param("IP");
		
		if (!modify_id || !argument) {
			printf("<font size=5 color=#ff0000><b>请输入ID与IP两个值,\
				系统将修改对应ID的IP值</b></font><br>");
			return 0;
		}
		int id = (int)strtol(modify_id, NULL, 10);

		if (id < 1 || id > max_id) {			//超出范围
			printf("<font size=5 color=#ff0000><b><center> ID不在范围,请重新输入\
				</center></b></font><br>");
			return 0;
		}
	
		/*----- 检查IP是否存在 -----*/
		modify_machine(db, id, argument);		//执行修改
	}

	else if (!strcmp(data, "更改控制广告机ID")) {
		argument = cgi_param("Name");

		if (!argument) {
			printf("<font size=5 color=#ff0000><b><center> 请输入要控制的广告机ID\
					</center></b></font><br>");
			return 0;
		}

		int id = (int)strtol(argument, NULL, 10);
		if (id < 1 || id > max_id) {			//超出范围
			printf("<font size=5 color=#ff0000><b><center> ID不在范围,请重新输入\
				</center></b></font><br>");
			return 0; 
		}

		set_current_id(db, argument);			//执行设置
	}

	else if (!strcmp(data, "打开播放机")) {
		strcpy(buff, "START&");
		control_play(db, buff);
	}

	else if (!strcmp(data, "关闭播放机")) {
		strcpy(buff, "CLOSE&");
		control_play(db, buff);
	}

	else if (!strcmp(data, "重新打开")) {
		strcpy(buff, "RESTART&");
		control_play(db, buff);
	}

	else if (!strcmp(data, "全屏模式打开")) {
		strcpy(buff, "FULLSTART&");
		control_play(db, buff);
	}

	else if (!strcmp(data, "指定大小打开")) {
		char *width = cgi_param("Width");
		char *height = cgi_param("Height");
		if (!width || !height) {
			printf("<font size=5 color=#ff0000><b><center> 请输入宽和高\
				</center></b></font><br>");
			return 0;
		}
		if (strtol(width, NULL, 10) <= 0 || strtol(height, NULL, 10) <= 0) {
			printf("<font size=5 color=#ff0000><b><center> 输入的值不合法\
				</center></b></font><br>");
			return 0;
		}
		strcpy(buff, "WINSTART&");
		strcat(buff, width);
		strcat(buff,"&");
		strcat(buff, height);
		control_play(db, buff);
	}

	else if (!strcmp(data, "发送")) {			//在联系我们标签
		printf("<font size=5 color=#ff0000><b><center> 发送成功! :) 你觉得呢?\
				</center></b></font><br>");
		return 0;
	}


	puts("</body></html>");				//html格式尾
	sqlite3_close(db);				//关闭数据库
	cgi_end();							//cgi结束

	return 0;
}

/*---------------------------------------------------------------------------------*/ 
/*----- 依次尝试连接各广告机 -----*/
/*---------------------------------------------------------------------------------*/ 
void status_check()
{
	int i;
	pthread_t tid[max_id];

	for (i = 0; i < max_id; i++) {			//创建线程
		pthread_create(&tid[i], NULL, thread_connect_main, (void *)i);
	}

	for (i = 0; i < max_id; i++) {			//等待线程处理结束
		pthread_join(tid[i], NULL);
	}
}

/*--------------------------------------------------------------------------------*/ 
/*----- 回调函数, 存储各广告机IP -----*/
/*--------------------------------------------------------------------------------*/ 
int callback_serverip(void *p, int argc, char **value, char **name)
{
	int i = *(int *)p;					//获得当前循环值i

	//printf("argc = %d\n<br>", argc);
	//printf("name = %s, value = %s\n<br>", name[1], value[1]);
	server_ip = realloc(server_ip, sizeof(char *) * (i + 1));	//二重指针分配空间
	server_ip[i] = malloc(16);			//一重指针分配空间, 存储地址
	strcpy(server_ip[i], value[1]);		//存储IP值	

	return 0;
}

/*--------------------------------------------------------------------------------*/ 
/*----- 回调函数, 获得广告机总数或当前操作广告机ID -----*/
/*--------------------------------------------------------------------------------*/ 
int callback(void *p, int argc, char **value, char **name)
{
	*(int *)p = (int)strtol(value[1], NULL, 10);	//记录广告机总数或当前操作广告机ID

	return 0;
}

/*--------------------------------------------------------------------------------*/ 
/*----- 线程入口函数, 连接各个广告机IP -----*/
/*--------------------------------------------------------------------------------*/ 
void *thread_connect_main(void *arg)
{
	int i = (int)arg;
	int socket_fd;
	struct sockaddr_in ipv4_addr;

	/*----- 初始化地址 -----*/
	memset(&ipv4_addr, 0, sizeof(ipv4_addr));
	ipv4_addr.sin_family = AF_INET;
	ipv4_addr.sin_port = htons(GET_PUT_PORT);		//连接文件系统服务器, 本cgi不和qt程序通讯
	inet_pton(AF_INET, server_ip[i], &ipv4_addr.sin_addr);

	if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Create failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (connect(socket_fd, (struct sockaddr *)&ipv4_addr, sizeof(ipv4_addr)) < 0) {
		/*----- 连接失败 -----*/
		printf("<font size=3 color=#ff0000><b>\"%d\"号广告机, IP:\"%s\" ------> 未开启\
			</b></font><br>", i+1, server_ip[i]);
	//	printf("%d 号广告机: IP:\"%s\" ----> 未开启<br>", i + 1, server_ip[i]);
	}

	/*----- 连接成功, 请求播放机状态信息 -----*/
	else {
		ssize_t n;
		char buff[1024];

		send(socket_fd, "STATUS&", 100, 0);
		n = recv(socket_fd, buff, 1024, 0);
		buff[n] = '\0';

		if (!strcmp(buff, "1"))
			printf("<font size=3 color=#ff0000><b>\"%d\"号广告机, IP:\"%s\" ------> 已开启,\
				播放机已启动</b></font><br>", i+1, server_ip[i]);
		else
			printf("<font size=3 color=#ff0000><b>\"%d\"号广告机, IP:\"%s\" ------> 已开启,\
				播放机未启动</b></font><br>", i+1, server_ip[i]);
		close(socket_fd);
	}

	pthread_exit((void *)0);
}

/*--------------------------------------------------------------------------------*/ 
/*----- 添加广告机 -----*/
/*--------------------------------------------------------------------------------*/ 
void add_machine(sqlite3 *db, char *argument)
{
	char buff[1024], *sql;

	sprintf(buff, "%s%d", "server_ip", max_id + 1);
	sql = sqlite3_mprintf("insert into ip values(%Q,%Q)", buff, argument);
	//printf("sql=%s<br>", sql);
	sqlite3_exec(db, sql, NULL, NULL, NULL);			//执行添加操作, 不需要回调函数
	sqlite3_free(sql);			//释放sql
	max_id += 1;				//总广告机数加1

	sprintf(buff, "%d", max_id);	//更新数据库总广告机数
	sql = sqlite3_mprintf("update ip set value=%Q where name=%Q", buff, "max_ip");
	//printf("sql=%s<br>", sql);
	sqlite3_exec(db, sql, NULL, NULL, NULL);			//执行添加操作, 不需要回调函数
	sqlite3_free(sql);			//释放sql

	printf("<font size=4 color=#ff0000><b>添加成功, 共有\"%d\"台广告机</b></font><br>", max_id);
}

/*--------------------------------------------------------------------------------*/ 
/*----- 删除广告机 -----*/
/*--------------------------------------------------------------------------------*/ 
void del_machine(sqlite3 *db, int id)
{
	char buff[1024], *sql;

	sprintf(buff, "%s%d", "server_ip", id);
	sql = sqlite3_mprintf("delete from ip where name=%Q", buff);
	//printf("sql=%s<br>", sql);
	sqlite3_exec(db, sql, NULL, NULL, NULL);			//执行删除操作, 不需要回调函数
	sqlite3_free(sql);			//释放sql

	/*----- 后面的广告机ID依次向前移 -----*/
	char next_id[16];
	
	while (id < max_id) {
		sprintf(next_id, "%s%d", "server_ip", id+1);
		sql = sqlite3_mprintf("update ip set name=%Q where name=%Q", buff, next_id);
		sqlite3_exec(db, sql, NULL, NULL, NULL);			//执行修改操作, 不需要回调函数
		sprintf(buff, "%s%d", "server_ip", id+1);
		sqlite3_free(sql);
		id += 1;
	}

	/*----- 更新数据库总广告机数 -----*/
	max_id -= 1;				
	sprintf(buff, "%d", max_id);	//更新数据库总广告机数
	sql = sqlite3_mprintf("update ip set value=%Q where name=%Q", buff, "max_ip");
	//printf("sql=%s<br>", sql);
	sqlite3_exec(db, sql, NULL, NULL, NULL);			//执行修改操作, 不需要回调函数
	sqlite3_free(sql);			//释放sql

	printf("<font size=4 color=#ff0000><b>删除成功, 共有\"%d\"台广告机</b></font><br>", max_id);
}

/*--------------------------------------------------------------------------------*/ 
/*----- 修改广告机IP -----*/
/*--------------------------------------------------------------------------------*/ 
void modify_machine(sqlite3 *db, int id, char *argument)
{
	char buff[1024], *sql;

	sprintf(buff, "%s%d", "server_ip", id);
	sql = sqlite3_mprintf("update ip set value=%Q where name=%Q", argument, buff);
	//printf("sql=%s<br>", sql);
	sqlite3_exec(db, sql, NULL, NULL, NULL);			//执行删除操作, 不需要回调函数
	sqlite3_free(sql);			//释放sql

	printf("<font size=4 color=#ff0000><b>修改成功, \"%d\" 号广告机IP设置为 ---> \"%s\"\
		</b></font><br>", id, argument);
}

/*--------------------------------------------------------------------------------*/ 
/*----- 修改当前操作广告机ID -----*/
/*--------------------------------------------------------------------------------*/ 
void set_current_id(sqlite3 *db, char *argument)
{
	char *sql;

	sql = sqlite3_mprintf("update ip set value=%Q where name=%Q", argument, "current_ip");
	//printf("sql=%s<br>", sql);
	sqlite3_exec(db, sql, NULL, NULL, NULL);			//执行删除操作, 不需要回调函数
	sqlite3_free(sql);			//释放sql

	
	printf("<font size=4 color=#ff0000><b>修改成功, 当前操作广告机ID为--->\"%s\"\
				</b></font><br>", argument);
}

/*--------------------------------------------------------------------------------*/ 
/*----- 控制播放机 -----*/
/*--------------------------------------------------------------------------------*/ 
void control_play(sqlite3 *db, char *command)
{
	char *sql, buff[1024];
	int current_id = 0;

	/*----- 获取当前控制广告机ID -----*/
	sql = sqlite3_mprintf("select *from ip where name=%Q", "current_ip");	//合并字符串
	sqlite3_exec(db, sql, callback, &current_id, NULL);			//执行查找语句,获得当前操作的广告机
	sqlite3_free(sql);			//释放sql
	
	if (current_id == 0) {			//回调函数没有执行, 没找到当前ip值
		printf("<font size=4 color=#ff0000><b><center>要操作广告机不存在</center></b></font><br>");
		exit(EXIT_FAILURE);
	}

	//printf("server_ip = %s\n<br>", server_ip);
	int socket_fd;

	/*----- 创建socket -----*/
	if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Create failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in ipv4_addr;

	memset(&ipv4_addr, 0, sizeof(ipv4_addr));
	ipv4_addr.sin_family = AF_INET;
	ipv4_addr.sin_port = htons(GET_PUT_PORT);		//连接文件系统服务器
	inet_pton(AF_INET, server_ip[current_id-1], &ipv4_addr.sin_addr);	//current_id - 1

	if (connect(socket_fd, (struct sockaddr *)&ipv4_addr, sizeof(ipv4_addr)) < 0) {
		/*----- 连接失败 -----*/
		printf("<font size=4 color=#ff0000><b><center>要操作广告机未开启</center></b></font><br>");
		close(socket_fd);
		return;
	}

	/*----- 连接成功 -----*/
	ssize_t n;
	send(socket_fd, command, strlen(command), 0);
	n = recv(socket_fd, buff, 1024, 0);
	buff[n] = '\0';

	if (!strcmp(buff, "ok")) {
		printf("<font color=#ff0000, size=4><b><center>操作成功</b></center></font><br>");
	}
	else
		printf("%s", buff);

	return;
}
