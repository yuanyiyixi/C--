#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
//#include <session.h>
#include <cgi.h>

int rscallback(void *p, int argc, char **argv, char **argvv) 
{
	int i;

	//for (i = 0; i < argc; i++)
	//	printf("argv[%d]=%s, argvv[%d]=%s<br>", i, argv[i] ? argv[i] : "null", i, argvv[i] ? argvv[i] : "null");

	*(int *) p = 0;		//判断查询结果是否为空
	return 0;
}

int main(void)
{
	int ret = 0;
	int level = -1;	//权限
	char *err = NULL;		
	sqlite3 *db;
	char *sql;
	//char *data;

	//printf("Hello world<br>");
	cgi_init();
	cgi_session_start();
	cgi_process_form();


	if (!cgi_param("text1") || !cgi_param("text2"))
	{
		//cgi_init_headers();
		//puts("<html><body>");
		//puts("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">");
		//printf("<br><br><br><font size=4 color=#ff0000><b><center>请输入用户名与密码!</center></b></font><br>");
		#if 0
		FILE *fp = fopen("../htdocs/index_fail.html", "r");
		
		#endif
		cgi_redirect("/index_empty.html");

		cgi_end();
		return -1;
	}

	ret = sqlite3_open("data.db", &db);
	if (ret != SQLITE_OK)
	{
		cgi_init_headers();
		puts("<html><body>");
		fputs(sqlite3_errmsg(db), stdout);	//打印错误信系到标准出错上
		return -1;
	}

	sql = sqlite3_mprintf("select *from user where name=%Q and passwd=%Q;", cgi_param("text1"), cgi_param("text2"));
	ret = sqlite3_exec(db, sql, rscallback, &level, &err);	//rallback 回调函数 select           
	sqlite3_free(sql);

	if (ret != SQLITE_OK)
	{
		cgi_init_headers();
		puts("<html><body>");
		puts("查找失败<br>");
		printf("err=%s<br>", err);
		sqlite3_close(db);		//不能忘记关闭
		exit(1);
	}
	//printf("level=%d<br>", level);
	//return 0;
	//cgi_init_headers();		//printf("Content_Type:text/html\r\n\r\n");
	//puts("<html><body>");

	if (level >= 0)
	{
		// yeah!
		// in this case, we aren't looking for a correct
		// username or password, so anything is correct
		cgi_session_register_var("logged", "1");	//cgi_session_register_var("logged", id);
		// Sends the user to main page
		//printf("level=%d<br>", level);
		if (level == 0) {
			cgi_redirect("/main.html");
			//cgi_redirect("session.cgi");	//超级用户的主页
			//  puts("<a href=\"http://127.0.0.1:8080/login_ok.html\">登录成功</a>");
		}
		else				//普通用户主页,暂未实现
			cgi_redirect("/main.html");

	}

	else		//未找到,验证失败
	{
		cgi_redirect("/index_fail.html");
	}

	cgi_end();
	return 0;
}
