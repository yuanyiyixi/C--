/* 客户端连接请求格式: 请求模式&文件类型&路径文件名 
   模式:
	PUT: 上传文件到广告机
	GET: 从广告机下载文件
	DEL: 删除文件
	START:打开广告机程序
	RESTART:重新打开广告机
	WINSTART:指定大小打开广告机
	CLOSE:关闭广告机程序
	RESTART:重新打开
	类型: VIDEO视频, PICTURE1图片1, PICTURE2图片2, LOGO, LIST等, 根据类型会进入相应的目录, 只在该目录下建立或打开文件
	文件名: 只取名字, 不取路径  类型已经决定了路径
zcb 2012.6.26
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <string.h>

int is_qtstart = 0;	//qt是否开启标志
pid_t qtpid;		//qt程序pid值

/*--------------------------------------------------------------------------------*/ 
// 宏定义 
/*--------------------------------------------------------------------------------*/ 
#define BUFFER_SIZE 4096	
#define BIND_PORT 8001

/* 目录宏, 服务器只会进入这些目录, 不管上传或下载 */
#define VIDEO_PATH "./guanggaoji/video"
#define PICTURE1_PATH "./guanggaoji/picture1/"
#define PICTURE2_PATH "./guanggaoji/picture2/"
#define PICTURE3_PATH "./guanggaoji/picture3/"
#define LIST_PATH "./guanggaoji/list/"
#define QT_PROGRAM "guanggaoji"
#define QT_PATH "./guanggaoji"

/*--------------------------------------------------------------------------------*/ 
// 函数声明 
/*--------------------------------------------------------------------------------*/ 
int change_directory(char *);		//改变目录, 返回0成功, -1失败
void getfile(int, char *);			//从服务器下载文件
void putfile(int, char *);			//上传文件到服务器
void delfile(int, char *);			//删除文件
void close_qt(void);				//关闭qt程序
void start_qt(char *, char *);		//打开qt程序
void child_process_main(int, char *, char *);		//创建进程处理新连接, 主进程继续监听
void sigchld_handler(int);			//SIGCHLD信号处理, 给子进程收尸

/*--------------------------------------------------------------------------------*/ 
// 主函数入口
/*--------------------------------------------------------------------------------*/ 
int main(int argc, char **argv)
{
	/*----------------------------------------------------------------------------*/ 
	// 信号处理
	/*----------------------------------------------------------------------------*/ 
	struct sigaction act;

	// 初始化
	memset(&act, 0, sizeof(act));
	act.sa_handler = sigchld_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	// 原型：int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
	sigaction(SIGCHLD, &act, NULL);		//捕获SIGCHLD信号

	start_qt(NULL, NULL);		//打开广告机开始播放

	/*----------------------------------------------------------------------------*/ 
	// 父进程, 创建socket
	/*----------------------------------------------------------------------------*/ 
	int listening_socket;
	unsigned short port = BIND_PORT;

	// 原型：int socket(int domain, int type, int protocol);
	if ((listening_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		// 创建失败 
		fprintf(stderr, "[%d]Create new TCP socket failed: %s\n", getpid(), strerror(errno));
		exit(1);
	}
	printf("[%d]Created a new TCP socket, listening_socket = %d.\n", getpid(), listening_socket);

	/*----------------------------------------------------------------------------*/ 
	// 设置socket选项, 保证不会出现IP, 端口被占用情况;
	/*----------------------------------------------------------------------------*/ 
	int optval = 1;

	// 原型:int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
	setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

#ifdef SO_REUSEPORT
	optval = 1;
	setsockopt(listening_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
#endif

	/*----------------------------------------------------------------------------*/ 
	// 绑定本机地址
	/*----------------------------------------------------------------------------*/ 
	struct sockaddr_in local_ipv4_address;	// IPv4 address structure

#if 0
	struct sockaddr_in			//地址结构体原型
	{
		sa_family_t sin_family;
		in_port_t sin_port;
		struct in_addr sin_addr;
		unsigned char sin_zero[8];
	};
#endif

	/* 初始化地址变量 */
	memset(&local_ipv4_address, 0, sizeof(local_ipv4_address));	//清0地址变量
	local_ipv4_address.sin_family = AF_INET;	// IPv4地址
	local_ipv4_address.sin_port = htons(port);	// 网络字节序 

	// 原型:int inet_pton(int af, const char *src, void *dst);
	inet_pton(AF_INET, "0.0.0.0", &local_ipv4_address.sin_addr);
	//local_ipv4_address.sin_addr.s_addr = INADDR_ANY;		//此宏有同样功能

	// 原型:int bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen);
	if (bind(listening_socket, (struct sockaddr *) &local_ipv4_address, 
				sizeof(local_ipv4_address)) < 0) {
		/* 绑定失败 */
		fprintf(stderr, "Bind to 0.0.0.0:%d failed: %s\n", port, strerror(errno));
		close(listening_socket);
		exit(EXIT_FAILURE);
	}
	printf("[%d]Bound 0.0.0.0:%d successfully.\n", getpid(), port);

	/*----------------------------------------------------------------------------*/ 
	// Listen监听端口
	/*----------------------------------------------------------------------------*/ 
	// 原型:int listen(int sockfd, int backlog);
	if (listen(listening_socket, 100) < 0) {
		// 监听失败 
		fprintf(stderr, "[%d]Listen:%d failed: %s\n", getpid(), port, strerror(errno));
		close(listening_socket);
		exit(EXIT_FAILURE);
	}
	printf("[%d]Listen on 0.0.0.0:%d successfully\n", getpid(), port);
	printf("[%d]Waiting for incomming connection ...\n", getpid());

	/*----------------------------------------------------------------------------*/ 
	// 主循环
	/*----------------------------------------------------------------------------*/ 
	int new_connected_socket;
	struct sockaddr_in peer_ipv4_address;
	socklen_t peer_ipv4_address_length;

	while (1) {
		// peer_ipv4_address_length是值结果参数, 每次accept有可能会改变, 所以循环每次赋值
		peer_ipv4_address_length = sizeof(peer_ipv4_address);

		// 原型: int accept(int sockfd, struct sockaddr *addr, socklen_t * addrlen);
		if ((new_connected_socket = accept(listening_socket, 
						(struct sockaddr *) &peer_ipv4_address, &peer_ipv4_address_length)) < 0) {
			// 失败, 正常情况会阻塞住 
			if (errno == EINTR) //被信号打断, 重新回到阻塞状态
				continue;

			fprintf(stderr, "Accept new connection failed: %s\n", strerror(errno));	//其它错误
			break;
		}
		char peer_ipv4_address_string[] = "ddd.ddd.ddd.ddd";	//存储客户端地址

		// 原型: const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
		inet_ntop(AF_INET, &peer_ipv4_address.sin_addr, peer_ipv4_address_string, 
				sizeof(peer_ipv4_address_string));

		/*----- 打印客户端地址 -----*/
		printf("[%d]Accepted a new connection %d from %s:%d.\n", 
				getpid(), new_connected_socket, peer_ipv4_address_string, 
				ntohs(peer_ipv4_address.sin_port)); 

		/*----------------------------------------------------------------------------*/ 
		// 接收头信息(客户端请求, 播放机控制,发送文件还是接收文件)
		/*----------------------------------------------------------------------------*/ 
		char buffer[200];
		ssize_t n;

recv_again:
		if ((n = recv(new_connected_socket, buffer, sizeof(buffer) - 1, 0)) < 0) {
			// failed
			if (errno == EINTR)	//信号打断
				goto recv_again;

			fprintf(stderr, "Receive data from client failed: %s\n", strerror(errno));
			return -1;
		}
		else if (n == 0) {		//客户端关闭了连接
			fprintf(stderr, "[%d]Connection closed by peer.\n", getpid());
			return -1;
		}

		buffer[n] = '\0';

		char *p = strchr(buffer, '&');		//分割符, 提取请求模式
		*p++ = '\0';						//p指向分割符下一个字符, 用于再次切割

		/*----------------------------------------------------------------------------*/ 
		// 根据请求模式，进行处理 
		/*----------------------------------------------------------------------------*/ 
		if (!strcmp(buffer, "CLOSE")) {		//关闭播放机
			if (is_qtstart) {
				close_qt();
				send(new_connected_socket, "ok", 2, 0);	//返回处理成功信息
			}
			else {			//返回处理失败信息
				send(new_connected_socket, "<font size=4 color=#ff0000><b><center>广告机已处于关闭状态</center></b></font><br>", 1024, 0);
			}
		}

		else if (!strcmp(buffer, "START")) {	//开启播放机
			if (!is_qtstart) {
				start_qt(NULL, NULL);
				send(new_connected_socket, "ok", 2, 0);
			}

			else {
				send(new_connected_socket, "<font size=4 color=#ff0000><b><center>\
					广告机已处于开启状态</center></b></font><br>", 1024, 0);
			}
		}

		else if (!strcmp(buffer, "RESTART")) {
			if (is_qtstart) {
				close_qt();
			}
			while (is_qtstart) 
				usleep(100);			//等待收尸完成
			start_qt(NULL, NULL);
			send(new_connected_socket, "ok", 2, 0);
		}

		else if (!strcmp(buffer, "STATUS")) {	//返回播放机是否开启
			if (is_qtstart) {
				send(new_connected_socket, "1", 1, 0);
			}
			else
				send(new_connected_socket, "0", 1, 0);
		}
		
		else if (!strcmp(buffer, "FULLSTART")) {
			if (!is_qtstart) {
				start_qt("FULL", NULL);			//全屏模式
				send(new_connected_socket, "ok", 2, 0);
			}

			else {
				send(new_connected_socket, "<font size=4 color=#ff0000><b><center>\
					广告机已处于开启状态</center></b></font><br>", 1024, 0);
			}
		}

		else if (!strcmp(buffer, "WINSTART")) {
			char *q = strchr(p, '&');		//p, q分别取宽高值
			*q++ = '\0';					//p指向分割符下一个字符, 用于再次切割
			if (!is_qtstart) {
				start_qt(p, q);			//指定大小打开
				send(new_connected_socket, "ok", 2, 0);
			}

			else {
				send(new_connected_socket, "<font size=4 color=#ff0000><b><center>\
					广告机已处于开启状态</center></b></font><br>", 1024, 0);
			}
		}

		/*----------------------------------------------------------------------------*/ 
		// 其它请求: 对文件操作, 为不影响监听和不改变父进程工作目录, 创建子进程处理
		/*----------------------------------------------------------------------------*/ 
		else {	
			pid_t pid;

			// 原型: pid_t fork(void);
			if ((pid = fork()) < 0) 	//失败
				fprintf(stderr, "Create new child process failed: %s\n", strerror(errno));

			else if (pid == 0) {		//子进程
				close(listening_socket);		//关闭继承来的监听连接
				child_process_main(new_connected_socket, buffer, p);	//调用函数去处理
				close(new_connected_socket);	//显式关闭连接
				exit(EXIT_SUCCESS);
			}
			// 父进程
			close(new_connected_socket);		//关闭子进程的socket连接, 必须做
		}
	}

	close(listening_socket);

	return 0;
}

/*--------------------------------------------------------------------------------*/ 
/*----- 子进程负责处理文件请求 -----*/
/*--------------------------------------------------------------------------------*/ 
void child_process_main(int fd, char *buffer, char *p)
{
	ssize_t n;

	printf("mode = %s\n", buffer);		
	printf("p = %s\n", p);

	if (!strcmp(buffer, "GET")) 		//从广告机中下载文件
		getfile(fd, p);	

	else if (!strcmp(buffer, "PUT"))	//上传文件到广告机
		putfile(fd, p);

	else if (!strcmp(buffer, "DEL"))	//删除文件
		delfile(fd, p);


	else {
		fprintf(stderr, "Request not accept\n");
		send(fd, "request not accept", 50, 0);		//请求类型不对, 发送错误信息
	}

	return;
}

/*--------------------------------------------------------------------------------*/ 
/*----- 上传文件到广告机 -----*/
/*--------------------------------------------------------------------------------*/ 
void putfile(int fd, char *p)			
{
	ssize_t n;
	char buffer[BUFFER_SIZE];
	char *filename;

	filename = strchr(p, '&');			//取类型, 根据类型会进入不同目录搜索文件
	*filename++ = '\0';					//filename指向路径文件名
	//printf("type = %s\n", p);
	//printf("filename = %s\n", filename);

	if (change_directory(p) < 0) {		//改变进程工作目录, 返回-1进入目录失败
		fprintf(stderr, "Change directory failed: %s\n", strerror(errno));
		send(fd, "进入目录失败(Change directory failed)", 100, 0);	//发送错误信息到客户端
		return;
	}

	/* 进入目录成功 */
	if ((p = strrchr(filename, '/')) != NULL)	//可能包括了路径, 只取出最后文件名部分
		filename = p + 1;

	int filefd;

	/* 创建的方式打开文件, 如果文件存在截断 */
	if ((filefd = open(filename, O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0) {
		/* 打开文件失败 */
		fprintf(stderr, "Create file failed: %s\n", strerror(errno));
		send(fd, "创建文件失败(Create file failed)", 100, 0);			//发送打开文件失败消息
		return; 
	}
	//printf("Open file successfully!\n");

	// 原型:ssize_t send(int s, const void *buf, size_t len, int flags);
	if ((n = send(fd, "ok", 2, 0)) < 0) {	//发送已准备好消息到客户端, 准备传输
		// failed
		fprintf(stderr, "[%d]Send data to client failed: %s\n", getpid(), strerror(errno));
	}

	while ((n = recv(fd, buffer, BUFFER_SIZE, 0)) > 0) {		//循环接收数据
		write(filefd, buffer, n);			//将数据写入文件
	}

	close(filefd);
	return;
}

/*--------------------------------------------------------------------------------*/ 
/*----- 从广告机下载文件 -----*/
/*--------------------------------------------------------------------------------*/ 
void getfile(int fd, char *p)		
{
	ssize_t n;
	char buffer[BUFFER_SIZE];
	char *filename;

	filename = strchr(p, '&');	//取类型
	*filename++ = '\0';					//filename指向路径与文件名
	//printf("type = %s\n", p);
	printf("filename = %s\n", filename);

	if (change_directory(p) < 0) {		//进入目录失败
		fprintf(stderr, "Change directory failed: %s\n", strerror(errno));
		send(fd, "进入目录失败(Change directory failed)", 100, 0);
		return;
	}

	if ((p = strrchr(filename, '/')) != NULL)		//可能包括了路径, 只取出最后文件名部分
		filename = p + 1;

	int filefd;

	/* 只读方式打开文件 */
	if ((filefd = open(filename, O_RDONLY)) < 0) {
		/* 打开文件失败 */
		fprintf(stderr, "Create file failed: %s\n", strerror(errno));
		send(fd, "打开文件失败(Open file failed)", 100, 0);			//发送打开文件失败消息
		return;
	}
	//printf("Open file successfully!\n");

	// 原型:ssize_t send(int s, const void *buf, size_t len, int flags);
	if ((n = send(fd, "ok", 2, 0)) > 0) {	//发送已准备好消息到客户端, 准备传输
		n = recv(fd, buffer, BUFFER_SIZE - 1, 0);		//接收客户端是否准备好信息
		buffer[n] = '\0';

		if (!strcmp(buffer, "ok")) {			//客户端准备好
			//printf("client ready!\n");
			while ((n = read(filefd, buffer, BUFFER_SIZE))) {	//从文件中读取数据
				send(fd, buffer, n, 0);			//发送到客户端
			}
			//printf("send over\n");
		}
	}

	close(filefd);

	return;
}

/*--------------------------------------------------------------------------------*/ 
/*----- 删除文件实现函数 -----*/
/*--------------------------------------------------------------------------------*/ 
void delfile(int fd, char *p)	
{
	ssize_t n;
	char buffer[BUFFER_SIZE];
	char *filename;

	filename = strchr(p, '&');			//取类型, 根据类型会进入不同目录搜索文件
	*filename++ = '\0';					//filename指向路径文件名
	//printf("type = %s\n", p);
	//printf("filename = %s\n", filename);

	if (change_directory(p) < 0) {		//改变进程工作目录, 返回-1改变失败
		fprintf(stderr, "Change directory failed: %s\n", strerror(errno));
		send(fd, "进入目录失败(Change directory failed)", 100, 0);	//发送错误信息到客户端
		return;
	}

	/* 进入目录成功 */
	if ((p = strrchr(filename, '/')) != NULL)	//可能包括了路径, 只取出最后文件名部分
		filename = p + 1;

	int filefd;

	/* 创建的方式打开文件, 如果文件存在截断 */
	if ((filefd = open(filename, O_RDONLY)) < 0) {
		/* 打开文件失败 */
		fprintf(stderr, "Create file failed: %s\n", strerror(errno));
		send(fd, "找不到文件(Can't find file)", 100, 0);			//发送打开文件失败消息
		return;
	}

	close(filefd);

	if (unlink(filename) < 0) {			//删除文件, 失败 
		fprintf(stderr, "Delete %s failed:%s\n", filename, strerror(errno));
		send(fd, "删除失败(delete failed)", 100, 0);
	}
	else {				//成功
		printf("Delete file \"%s\" successfully!\n", filename);
		send(fd, "删除成功(delete successfully)", 100, 0);
	}

	return;
}

/*--------------------------------------------------------------------------------*/ 
/*----- 根据请求类型进入相应的目录 -----*/
/*--------------------------------------------------------------------------------*/ 
int change_directory(char *p)	
{
	if (!strcmp(p, "VIDEO")) {			//视频类型
		if (chdir(VIDEO_PATH) < 0) {
			return -1;
		}
	}

	else if (!strcmp(p, "PICTURE2")) {	//图片2, 右下
		if (chdir(PICTURE2_PATH) < 0) {	
			return -1;
		}
	}

	else if (!strcmp(p, "PICTURE3")) {		//图片区3, 右上
		if (chdir(PICTURE3_PATH) < 0) {	
			return -1;
		}
	}

	else if (!strcmp(p, "PICTURE1")) {	//图片1, 右中
		if (chdir(PICTURE1_PATH) < 0) {	
			return -1;
		}
	}

	else if (!strcmp(p, "LIST")) {		//播放列表
		if (chdir(LIST_PATH) < 0) {	
			return -1;
		}
	}

	return 0;		//成功返回0
}

/*--------------------------------------------------------------------------------*/ 
/*----- SIGCHLD信号捕获实现函数 -----*/
/*--------------------------------------------------------------------------------*/ 
void sigchld_handler(int signum)	//收尸子进程
{
	//fprintf(stdout, "[%d]Caught signal %d.\n", getpid(), signum);
	pid_t pid;
	int status;

	// 原型: pid_t wait(int *status);
	if ((pid = wait(&status)) < 0) {
		// 失败 
		return;
	}

	if (pid == qtpid) {		//如果是广告机播放程序退出, 则将标志位置0
		is_qtstart = 0;
		printf("QT is down, is_qtstart=%d\n", is_qtstart);
	}
	// 取返回状态
	fprintf(stdout, "[%d]Child process %d terminated, status = 0x%08x\n", getpid(), pid, status);

	return;
}

/*----------------------------------------------------------------------------*/ 
// 加载QT程序 
/*----------------------------------------------------------------------------*/ 
void start_qt(char *width, char *height)
{
	if ((qtpid = fork()) < 0) {		//创建QT进程失败
		fprintf(stderr, "Create QT main process failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	else if (qtpid == 0) {			//子进程, 加载QT主程序
		chdir(QT_PATH);
		if (!width)
			execl(QT_PROGRAM, "guanggaoji", NULL);

		else if (!strcmp(width, "FULL"))			//全屏模式,送一个参数过去
			execl(QT_PROGRAM, "guanggaoji", "FULL", NULL);
		
		else 
			execl(QT_PROGRAM, "guanggaoji", width, height, NULL);	//指定大小	
		
		exit(EXIT_FAILURE);
	}

	is_qtstart = 1;
	//printf("QT process is start!, is_qtstart=%d\n", is_qtstart);

	return;
}	

/*----------------------------------------------------------------------------*/ 
// 关闭QT程序 
/*----------------------------------------------------------------------------*/ 
void close_qt()
{
	if (kill(qtpid, SIGKILL) < 0) {
		fprintf(stderr, "Kill qt process failed: %s\n", strerror(errno));
		return;
	}

	is_qtstart = 0;

	//printf("Qt process is killed!\n");
}
