#include<stdlib.h>
#include<string.h>
#include"fcgi_stdio.h"
#include"snowflake.h"
#include"param_lib.h"
#include"html_lib.h"
#include"log_lib.h"
#include"http_user.h"

void user_login_page()
{
	printf("<form method=\"post\" action=\"user_login.fcgi\">");
	printf("<table>\n");
	printf("<tr>\n");
	printf("<td>用户名</td>\n");
	printf("<td><input type=\"text\" name=\"uname\"></td>");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>密码</td>\n");
	printf("<td><input type=\"password\" name=\"upwd\"></td>");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td><input type=\"submit\" value=\"登录\"></td>");
	printf("<td><a href=\"user_reg.html\">注册</a></td>\n");
	printf("</tr>\n");
	printf("</table>");
	printf("</form>");
}

void user_login_html()
{
	html_frame("用户登录",user_login_page);
}

void user_login_fcgi()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	param_env *p;
	long long uid;
	char *uname,*upwd;
	char log[1000];
	p=param_new(4);
	param_query_string_set(p);
	uname=param_get(p,"uname");
	upwd=param_get(p,"upwd");
	sprintf(sql,"select id from t_user where name='%s' and pwd='%s'",uname,upwd);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	if(!(row=mysql_fetch_row(result))){
		param_del(p);
		mysql_free_result(result);
		printf("Location: index.html\r\n");
		printf("\r\n");
		return;
	}
	uid=atoll(row[0]);
	sprintf(log,"%s %lld 登录系统",uname,uid);
	write_log("web",log);
	write_db_log(log);
	param_del(p);
	mysql_free_result(result);
	printf("Set-Cookie: uid=%lld\r\n",uid);
	printf("Location: index.html\r\n");
	printf("\r\n");
}

void user_logout_fcgi()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	char *uid;
	const unsigned char *uname;
	char log[1000];
	uid=getenv("HTTP_COOKIE")+4;
	sprintf(sql,"select name from t_user where id=%s",uid);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	row=mysql_fetch_row(result);
	uname=row[0];
	sprintf(log,"%s %s 退出系统",uname,uid);
	write_log("web",log);
	write_db_log(log);
	mysql_free_result(result);
	printf("Set-Cookie: uid=0; Max-Age=0\r\n");
	printf("Location: index.html\r\n");
	printf("\r\n");
}

void user_reg_page()
{
	printf("<form method=\"post\" action=\"user_reg.fcgi\">\n");
	printf("<table>\n");
	printf("<tr>\n");
	printf("<td>用户名</td>\n");
	printf("<td><input type=\"text\" name=\"uname\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>密码</td>\n");
	printf("<td><input type=\"password\" name=\"upwd\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>邮箱</td>\n");
	printf("<td><input type=\"text\" name=\"uemail\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>说明</td>\n");
	printf("<td><input type=\"text\" name=\"uinfo\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td><input type=\"submit\" value=\"注册\"></td>\n");
	printf("<td><input type=\"reset\" value=\"重填\"></td>\n");
	printf("</tr>\n");
	printf("</table>\n");
	printf("<form>\n");
}

void user_reg_html()
{
	html_frame("用户注册",user_reg_page);
}

void user_reg_fcgi()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	param_env *p;
	long long uid;
	char *uname,*upwd,*uemail,*uinfo;
	char log[1000];
	p=param_new(4);
	param_query_string_set(p);
	uname=param_get(p,"uname");
	upwd=param_get(p,"upwd");
	uemail=param_get(p,"uemail");
	uinfo=param_get(p,"uinfo");
	sprintf(sql,"select 1 from t_user where name='%s'",uname);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	if(row=mysql_fetch_row(result)){
		param_del(p);
		mysql_free_result(result);
		printf("Location: index.html\r\n");
		printf("\r\n");
		return;
	}
	mysql_free_result(result);
	uid=snowflake_id();
	if(uid==-1){
		param_del(p);
		printf("Location: index.html\r\n");
		printf("\r\n");
		return;
	}
	sprintf(sql,"insert into t_user(id,name,pwd,email,info,ctime) values(%lld,'%s','%s','%s','%s',sysdate())",uid,uname,upwd,uemail,uinfo);
	param_del(p);
	mysql_query(&mysql,sql);
	sprintf(log,"%s %lld 登录系统",uname,uid);
	write_log("web",log);
	write_db_log(log);
	printf("Set-Cookie: uid=%lld\r\n",uid);
	printf("Location: index.html\r\n");
	printf("\r\n");
}

void user_update_page()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	char *uid;
	const unsigned char *uname,*upwd,*uemail,*uinfo;
	uid=getenv("HTTP_COOKIE")+4;
	sprintf(sql,"select name,pwd,email,info from t_user where id=%s",uid);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	row=mysql_fetch_row(result);
	uname=row[0];
	upwd=row[1];
	uemail=row[2];
	uinfo=row[3];
	printf("<form method=\"post\" action=\"user_update.fcgi\">\n");
	printf("<table>\n");
	printf("<tr>\n");
	printf("<td>用户名</td>\n");
	printf("<td><input type=\"text\" name=\"uname\" value=\"%s\"></td>\n",uname);
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>密码</td>\n");
	printf("<td><input type=\"password\" name=\"upwd\" value=\"%s\"></td>\n",upwd);
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>邮箱</td>\n");
	printf("<td><input type=\"text\" name=\"uemail\" value=\"%s\"></td>\n",uemail);
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>说明</td>\n");
	printf("<td><input type=\"text\" name=\"uinfo\" value=\"%s\"></td>\n",uinfo);
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td><input type=\"submit\" value=\"提交\"></td>\n");
	printf("<td><input type=\"reset\" value=\"重填\"></td>\n");
	printf("</tr>\n");
	printf("</table>\n");
	printf("</form>\n");
	mysql_free_result(result);
}

void user_update_html()
{
	html_frame("修改用户信息",user_update_page);
}

void user_update_fcgi()
{
	char sql[1000];
	param_env *p;
	char *uid,*upwd,*uemail,*uinfo;
	uid=getenv("HTTP_COOKIE")+4;
	p=param_new(4);
	param_query_string_set(p);
	upwd=param_get(p,"upwd");
	uemail=param_get(p,"uemail");
	uinfo=param_get(p,"uinfo");
	sprintf(sql,"update t_user set pwd='%s',email='%s',info='%s' where id=%s",upwd,uemail,uinfo,uid);
	param_del(p);
	mysql_query(&mysql,sql);
	printf("Location: index.html\r\n");
	printf("\r\n");
}

void user_list_page()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	param_env *p;
	long long uid;
	const unsigned char *uname,*uemail,*uinfo,*uctime;
	char *page;
	int i,n;
	p=param_new(4);
	param_query_string_set(p);
	page=param_get(p,"page");
	if(page==NULL){
		i=0;
	}else{
		i=atoi(page);
	}
	param_del(p);
	sprintf(sql,"select count(*) from t_user where id>1");
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	row=mysql_fetch_row(result);
	n=atoi(row[0]);
	mysql_free_result(result);
	if(n>0){
		n=(n-1)/PAGE_ITEM_NUM;
	}
	if(i>n){
		i=n;
	}
	sprintf(sql,"select id,name,email,info,ctime from t_user where id>1 order by id limit %d offset %d",PAGE_ITEM_NUM,i*PAGE_ITEM_NUM);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	printf("<table width=\"100%%\">\n");
	printf("<tr bgcolor=\"#6589D1\">\n");
	printf("<td width=\"20%%\">用户ID</td>\n");
	printf("<td width=\"10%%\">用户名</td>\n");
	printf("<td width=\"20%%\">邮箱</td>\n");
	printf("<td width=\"20%%\">说明</td>\n");
	if(is_root()==0){
		printf("<td width=\"30%%\">注册时间</td>\n");
	}else{
		printf("<td width=\"20%%\">注册时间</td>\n");
		printf("<td width=\"10%%\">删除</td>\n");
	}
	printf("</tr>\n");
	while(row=mysql_fetch_row(result)){
		uid=atoll(row[0]);
		uname=row[1];
		uemail=row[2];
		uinfo=row[3];
		uctime=row[4];
		printf("<tr bgcolor=#EAEFFD>\n");
		printf("<td>%lld</td>\n",uid);
		printf("<td><a href=\"user_info.html?uid=%lld\">%s</a></td>\n",uid,uname);
		printf("<td>%s</td>\n",uemail);
		printf("<td>%s</td>\n",uinfo);
		printf("<td>%s</td>\n",uctime);
		if(is_root()){
			printf("<td><a href=\"user_delete.fcgi?uid=%lld\">DEL</a></td>\n",uid);
		}
		printf("</tr>\n");
	}
	printf("</table>\n");
	mysql_free_result(result);
	if(n>0){
		html_page_split("user_list.html",n,i);
	}
}

void user_list_html()
{
	html_frame("用户列表",user_list_page);
}

void user_info_page()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	param_env *p;
	char *uid;
	long long pid;
	const unsigned char *uname,*uemail,*uinfo,*uctime;
	int i;
	char *s;
	p=param_new(4);
	param_query_string_set(p);	
	uid=param_get(p,"uid");
	printf("<table width=\"100%%\">\n");
	printf("<tr bgcolor=\"#6589D1\">\n");
	printf("<td width=\"20%%\">用户ID</td>\n");
	printf("<td width=\"10%%\">用户名</td>\n");
	printf("<td width=\"20%%\">邮箱</td>\n");
	printf("<td width=\"20%%\">说明</td>\n");
	printf("<td width=\"30%%\">注册时间</td>\n");
	printf("</tr>\n");
	sprintf(sql,"select name,email,info,ctime from t_user where id=%s",uid);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	row=mysql_fetch_row(result);
	uname=row[0];
	uemail=row[1];
	uinfo=row[2];
	uctime=row[3];
	printf("<tr bgcolor=\"#EAEFFD\">\n");
	printf("<td>%s</td>\n",uid);
	printf("<td>%s</td>\n",uname);
	printf("<td>%s</td>\n",uemail);
	printf("<td>%s</td>\n",uinfo);
	printf("<td>%s</td>\n",uctime);
	printf("</tr>\n");
	mysql_free_result(result);
	sprintf(sql,"select prob_id from t_prob_try where user_id=%s and stat=1 order by prob_id",uid);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	printf("<tr bgcolor=\"#EAEFFD\">\n");
	printf("<td>已解决</td>\n");
	printf("<td colspan=\"5\">\n");
	printf("<table width=\"100%%\">\n");
	printf("<tr>\n");
	i=0;
	while(row=mysql_fetch_row(result)){
		pid=atoll(row[0]);
		printf("<td width=\"9%%\"><a href=\"prob_info.html?pid=%lld\">%lld</a></td>\n",pid,pid);
		i++;
		if(i==10){
			printf("</tr>\n");
			printf("<tr>\n");
			i=0;
		}
	}
	while(i<10){
		printf("<td width=\"9%%\"></td>\n");
		i++;
	}
	mysql_free_result(result);
	printf("</tr>\n");
	printf("</table>\n");
	printf("</td>");
	printf("</tr>");
	sprintf(sql,"select prob_id from t_prob_try where user_id=%s and stat<>1 order by prob_id",uid);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	printf("<tr bgcolor=\"#EAEFFD\">\n");
	printf("<td>未解决</td>\n");
	printf("<td colspan=\"5\">\n");
	printf("<table width=\"100%%\">\n");
	printf("<tr>\n");
	i=0;
	while(row=mysql_fetch_row(result)){
		pid=atoll(row[0]);
		printf("<td width=\"9%%\"><a href=\"prob_info.html?pid=%lld\">%lld</a></td>\n",pid,pid);
		i++;
		if(i==10){
			printf("</tr>\n");
			printf("<tr>\n");
			i=0;
		}
	}
	while(i<10){
		printf("<td width=\"9%%\"></td>\n");
		i++;
	}
	mysql_free_result(result);
	printf("</tr>");
	printf("</table>\n");
	printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");
	s=getenv("HTTP_COOKIE");
	if(s&&strcmp(s+4,uid)==0){
		printf("<a href=\"user_update.html?uid=%s\">修改我的信息</a>\n",uid);
	}
	param_del(p);
}

void user_info_html()
{
	html_frame("用户信息",user_info_page);
}

void user_delete_fcgi()
{
	char sql[1000];
	param_env *p;
	char *uid;
	p=param_new(4);
	param_query_string_set(p);
	uid=param_get(p,"uid");
	sprintf(sql,"delete from t_user where id=%s",uid);
	param_del(p);
	mysql_query(&mysql,sql);
	printf("Location: user_list.html\r\n");
	printf("\r\n");
}

