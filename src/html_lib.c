#include<stdlib.h>
#include<string.h>
#include"fcgi_stdio.h"
#include"param_lib.h"
#include"html_lib.h"

int is_root()
{
	char *s;
	s=getenv("HTTP_COOKIE");
	return strcmp(s,"uid=1")==0;
}

int is_login()
{
	char *s;
	s=getenv("HTTP_COOKIE");
	return strcmp(s,"uid=0")!=0&&strncmp(s,"uid=",4)==0;
}

void html_txt_print(const char *s){
	int i,n;
	printf("<pre>\n");
	for(i=0;s[i]!='\0';i++){
		if(s[i]=='<'){
			printf("&lt;");
			continue;
		}
		if(s[i]=='>'){
			printf("&gt;");
			continue;
		}
		putchar(s[i]);
	}
	printf("</pre>\n");
}

void html_page_index()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	char *uid;
	uid=getenv("HTTP_COOKIE");
	if(strcmp(uid,"uid=1")==0){
		printf("<table width=\"100%%\">\n");
		printf("<tr bgcolor=\"#EAEFFD\">\n");
		printf("<td colspan=\"10\">\n");
		printf("<img src=\"pic/logo.png\">\n");
		printf("</td>\n");
		printf("</tr>\n");
		printf("<tr bgcolor=\"#EAEFFD\">\n");
		printf("<td width=\"4%%\"><a href=\"index.html\">首页</a></td>\n");
		printf("<td width=\"7.5%%\"><a href=\"user_list.html\">用户管理</a></td>\n");
		printf("<td width=\"7.5%%\"><a href=\"prob_list.html\">题目管理</a></td>\n");
		printf("<td width=\"7.5%%\"><a href=\"test_list.html\">状态管理</a></td>\n");
		printf("<td width=\"7.5%%\"><a href=\"disc_list.html\">讨论管理</a></td>\n");
		printf("<td width=\"7.5%%\"><a href=\"log_list.html\">系统日志</a></td>\n");
		printf("<td width=\"7.5%%\"><a href=\"sys_info.html\">系统信息</a></td>\n");
		sprintf(sql,"select name from t_user where id=%s",uid+4);
		mysql_query(&mysql,sql);
		result=mysql_store_result(&mysql);
		row=mysql_fetch_row(result);
		printf("<td width=\"43%%\"></td>\n");
		printf("<td width=\"4%%\">%s</td>\n",row[0]);
		printf("<td width=\"4%%\"><a href=\"user_logout.fcgi\">退出</a></td>\n");
		mysql_free_result(result);
		printf("</tr>\n");
		printf("</table>\n");
		return;
	}
	printf("<table width=\"100%%\">\n");
	printf("<tr bgcolor=\"#EAEFFD\">\n");
	printf("<td colspan=\"8\">\n");
	printf("<img src=\"pic/logo.png\">\n");
	printf("</td>\n");
	printf("</tr>\n");
	printf("<tr bgcolor=\"#EAEFFD\">\n");
	printf("<td width=\"4%%\"><a href=\"index.html\">首页</a></td>\n");
	printf("<td width=\"4%%\"><a href=\"user_list.html\">用户</a></td>\n");
	printf("<td width=\"4%%\"><a href=\"prob_list.html\">题目</a></td>\n");
	printf("<td width=\"4%%\"><a href=\"test_list.html\">状态</a></td>\n");
	printf("<td width=\"4%%\"><a href=\"disc_list.html\">讨论</a></td>\n");
	if(strcmp(uid,"uid=0")!=0&&strncmp(uid,"uid=",4)==0){
		sprintf(sql,"select name from t_user where id=%s",uid+4);
		mysql_query(&mysql,sql);
		result=mysql_store_result(&mysql);
		row=mysql_fetch_row(result);
		printf("<td width=\"62%%\"></td>\n");
		printf("<td width=\"4%%\"><a href=\"user_info.html?uid=%s\">%s</a></td>\n",uid+4,row[0]);
		printf("<td width=\"4%%\"><a href=\"user_logout.fcgi\">退出</a></td>\n");
		mysql_free_result(result);
		printf("</tr>\n");
		printf("</table>\n");
		return;
	}
	printf("<td width=\"62%%\"></td>\n");
	printf("<td width=\"4%%\"><a href=\"user_login.html\">登录</a></td>\n");
	printf("<td width=\"4%%\"><a href=\"user_reg.html\">注册</a></td>\n");
	printf("</tr>\n");
	printf("</table>\n");
}

void html_page_split(char *href,int n,int i)
{
	printf("<center>\n");
	printf("<table width=\"40%%\">\n");
	printf("<tr>\n");
	if(i==0){
		printf("<td>第一页</td>\n");
		printf("<td>上一页</td>\n");
	}else{
		printf("<td><a href=\"%s\">第一页</a></td>\n",href);
		printf("<td><a href=\"%s?page=%d\">上一页</a></td>\n",href,i-1);
	}
	if(i==n){
		printf("<td>下一页</td>\n");
		printf("<td>最后一页</td>\n");
	}else{
		printf("<td><a href=\"%s?page=%d\">下一页</a></td>\n",href,i+1);
		printf("<td><a href=\"%s?page=%d\">最后一页</a></td>\n",href,n);
	}
	printf("</tr>\n");
	printf("</table>\n");
	printf("</center>\n");
}

void html_frame(char *title,void (*page)())
{
	printf("Content-Type: text/html; charset=utf-8\r\n");
	printf("\r\n");
	printf("<html>\n");
	printf("<head>\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("<link rel=\"icon\" href=\"pic/logo.ico\" type=\"image/x-ico\"/>\n"); //2016-10-15 forlfs 在浏览器标签的标题中显示小图标，图标大小为16*16
	printf("<title>%s</title>\n",title);
	printf("</head>\n");
	printf("<body>\n");
	html_page_index();
	printf("<hr/>\n");
	page();
	printf("</body>\n");
	printf("</html>\n");
}

