#include<stdlib.h>
#include<string.h>
#include"fcgi_stdio.h"
#include"param_lib.h"
#include"html_lib.h"
#include"http_disc.h"

void disc_list_page()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	param_env *p;
	long long did,uid,pid;
	const unsigned char *uname,*dtxt,*dctime;
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
	sprintf(sql,"select count(*) from t_disc");
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
	sprintf(sql,"select t1.id,t1.user_id,t2.name,t1.prob_id,t1.txt,t1.ctime from t_disc t1 left join t_user t2 on t1.user_id=t2.id order by t1.id desc limit %d offset %d",PAGE_ITEM_NUM,i*PAGE_ITEM_NUM);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	printf("<table width=\"100%%\">\n");
	printf("<tr bgcolor=\"#6589D1\">\n");
	printf("<td width=\"20%%\">讨论ID</td>\n");
	printf("<td width=\"10%%\">用户名</td>\n");
	printf("<td width=\"10%%\">题目ID</td>\n");
	printf("<td width=\"30%%\">内容</td>\n");
	if(is_root()==0){
		printf("<td width=\"30%%\">发表时间</td>\n");
	}else{
		printf("<td width=\"20%%\">发表时间</td>\n");
		printf("<td width=\"10%%\">删除</td>\n");
	}
	printf("</tr>\n");
	while(row=mysql_fetch_row(result)){
		did=atoll(row[0]);
		uid=atoll(row[1]);
		uname=row[2];
		pid=atoll(row[3]);
		dtxt=row[4];
		dctime=row[5];
		printf("<tr bgcolor=\"#EAEFFD\">\n");
		printf("<td>%lld</td>\n",did);
		printf("<td><a href=\"user_info.html?uid=%lld\">%s</a></td>\n",uid,uname);
		printf("<td><a href=\"prob_info.html?pid=%lld\">%lld</a></td>\n",pid,pid);
		printf("<td>%s</td>\n",dtxt);
		printf("<td>%s</td>\n",dctime);
		if(is_root()){
			printf("<td><a href=\"disc_delete.fcgi?did=%lld\">DEL</a></td>\n",did);
		}
		printf("</tr>\n");
	}
	printf("</table>\n");
	mysql_free_result(result);
	if(n>0){
		html_page_split("disc_list.html",n,i);
	}
}

void disc_list_html()
{
	html_frame("讨论列表",disc_list_page);
}

void disc_delete_fcgi()
{
	param_env *p;
	char *did;
	char sql[1000];
	p=param_new(4);
	param_query_string_set(p);
	did=param_get(p,"did");
	sprintf(sql,"delete from t_disc where id=%s",did);
	param_del(p);
	mysql_query(&mysql,sql);
	printf("Location: disc_list.html\r\n");
	printf("\r\n");
}

