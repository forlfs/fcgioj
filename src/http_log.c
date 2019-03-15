#include<stdlib.h>
#include<string.h>
#include"fcgi_stdio.h"
#include"param_lib.h"
#include"html_lib.h"
#include"http_log.h"

void log_list_page()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	param_env *p;
	long long lid;
	const unsigned char *ltxt,*lctime;
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
	sprintf(sql,"select count(*) from t_log");
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
	sprintf(sql,"select id,txt,ctime from t_log order by id desc limit %d offset %d",PAGE_ITEM_NUM,i*PAGE_ITEM_NUM);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	printf("<table width=\"100%%\">\n");
	printf("<tr bgcolor=\"#6589D1\">\n");
	printf("<td width=\"20%%\">日志ID</td>\n");
	printf("<td width=\"50%%\">内容</td>\n");
	printf("<td width=\"30%%\">记录时间</td>\n");
	printf("</tr>\n");
	while(row=mysql_fetch_row(result)){
		lid=atoll(row[0]);
		ltxt=row[1];
		lctime=row[2];
		printf("<tr bgcolor=\"#EAEFFD\">\n");
		printf("<td>%lld</td>\n",lid);
		printf("<td>%s</td>\n",ltxt);
		printf("<td>%s</td>\n",lctime);
		printf("</tr>\n");
	}
	printf("</table>\n");
	mysql_free_result(result);
	if(n>0){
		html_page_split("log_list.html",n,i);
	}
}

void log_list_html()
{
	html_frame("日志列表",log_list_page);
}

