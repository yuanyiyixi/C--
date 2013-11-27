#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<cgi.h>

#define MAXLINE 80
#define SERV_PORT 8800

int main(int argc, char *argv[])
{
	struct sockaddr_in servaddr, cliaddr;
	char buf[MAXLINE];
	int sockfd, n;
	char *str;

	cgi_init();
	//cgi_init_headers();
	cgi_process_form();

	if(cgi_param("pause"))
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		//inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
		servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		servaddr.sin_port = htons(SERV_PORT);

		connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	
		write(sockfd, "p", strlen("p")+1);
	
		//n = read(sockfd, buf, MAXLINE);
		//printf("Response from server:\n");
		//write(STDOUT_FILENO, buf, n);
		//printf("\n");
		close(sockfd);
		cgi_redirect("/control.html");
	}
	else if(cgi_param("full"))//点击全屏按钮
	{
	}
	else if(cgi_param("quickmove"))//点击快进按钮
	{
	}

	cgi_end();

	return 0;
}
