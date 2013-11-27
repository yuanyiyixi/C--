#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <session.h>
#include <cgi.h>

int main(int argc, char *argv[])
{
	sqlite3 *db;

	int result;
	char *errmsg = NULL;
	char *sql = NULL;

	cgi_init();
	cgi_process_form();

	cgi_init_headers();
	puts("<html><body>");

	if ((result = sqlite3_open("data.db", &db)) != SQLITE_OK) {
		printf("<font size=4 color=#ff0000><center><b>无法打开数据库!</b></center></font><br>");
		return -1;
	}

	if (!strcmp(cgi_param("Button"), "创建账号"))
	{
		char *name = cgi_param("Name");
		char *passwd = cgi_param("Passwd");
		char *level = cgi_param("Level");

		if (!name || !passwd || !level)
		{
			printf("<font size=4 color=#ff0000><center><b>请输入用户名,密码以及权限!</b></center></font><br>");
			cgi_end();
			return -1;
		}

		sql = sqlite3_mprintf("insert into user values(%Q, %Q, %q)", name, passwd, level);
	}

	else if (!strcmp(cgi_param("Button"), "删除账号"))
	{
		char *name = cgi_param("Name");	
		if (!name)
		{
			printf("<font size=4 color=#ff0000><center><b>用户名不能为空!</b></center></font><br>");
			cgi_end();
			return -1;
		}

		sql = sqlite3_mprintf("delete from user where name=%Q", name);
	}

	else if (!strcmp(cgi_param("Button"), "修改密码"))
	{
		char *name = cgi_param("Name");
		char *passwd1 = cgi_param("Passwd1");
		char *passwd2 = cgi_param("Passwd2");

		if (!name || !passwd1 || !passwd2)
		{
			printf("<font size=4 color=#ff0000><center><b>输入用户名,原密码与新密码!</b></center></font><br>");
			cgi_end();
			return -1;
		}
		sql = sqlite3_mprintf("update user set passwd=%Q where name=%Q and passwd=%Q", passwd2, name, passwd1);
	}
	else if (!strcmp(cgi_param("Button"), "修改权限"))
	{
		char *name = cgi_param("Name");
		char *level = cgi_param("Level");

		if (!name || !level)
		{
			printf("<font size=4 color=#ff0000><center><b>用户名和权限不能为空<br></b></center></font><br>");
			cgi_end();
			return -1;
		}
		sql = sqlite3_mprintf("update user set permissions=%q where name=%Q", level, name);
	}
	else if (!strcmp(cgi_param("Button"), "修改用户名"))
	{
		char *name1 = cgi_param("Name1");
		char *name2 = cgi_param("Name2");

		if (!name1 || !name2)
		{
			printf("<font size=4 color=#ff0000><center><b>输入原用户名与新用户名<br></b></center></font><br>");
			cgi_end();
			return -1;
		}
		sql = sqlite3_mprintf("update user set name=%Q where name=%Q", name2, name1);
	}

	result = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	sqlite3_free(sql);

	if (result != SQLITE_OK)
	{
		printf("<font size=4 color=#ff0000><center><b>操作失败! %s<br></b></center></font><br>", errmsg);
		cgi_end();
		sqlite3_close(db);
	}
	else
		printf("<font size=4 color=#ff0000><center><b>操作成功<br></b></center></font><br>");

	puts("</body></html>");

	return 0;
}
