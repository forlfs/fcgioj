#include<stdlib.h>
#include<string.h>
#include"fcgi_stdio.h"
#include"param_lib.h"
#include"html_lib.h"
#include"http_test.h"

void test_list_page()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	param_env *p;
	long long tid,uid,pid;
	unsigned long ttime,trss,tsize;
	int tstat;
	const unsigned char *uname,*tctime;
	char *cuid;
	char *page;
	int i,n;
	char status[][7]={"RUN","AC","CE","WA","RE","TLE","MLE","OLE","SE"};
	p=param_new(4);
	param_query_string_set(p);
	page=param_get(p,"page");
	if(page==NULL){
		i=0;
	}else{
		i=atoi(page);
	}
	param_del(p);
	sprintf(sql,"select count(*) from t_test");
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
	sprintf(sql,"select t1.id,t1.user_id,t2.name,t1.prob_id,t1.run_time,t1.run_rss,t1.txt_size,t1.stat,t1.ctime from t_test t1 left join t_user t2 on t1.user_id=t2.id order by t1.id desc limit %d offset %d",PAGE_ITEM_NUM,i*PAGE_ITEM_NUM);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	printf("<table width=\"100%%\">\n");
	printf("<tr bgcolor=\"#6589D1\">\n");
	printf("<td width=\"20%%\">提交ID</td>\n");
	printf("<td width=\"10%%\">用户名</td>\n");
	printf("<td width=\"10%%\">题目ID</td>\n");
	printf("<td width=\"7.5%%\">时间</td>\n");
	printf("<td width=\"7.5%%\">内存</td>\n");
	printf("<td width=\"7.5%%\">代码</td>\n");
	printf("<td width=\"7.5%%\">状态</td>\n");
	if(is_root()==0){
		printf("<td width=\"30%%\">提交时间</td>\n");
	}else{
		printf("<td width=\"20%%\">提交时间</td>\n");
		printf("<td width=\"10%%\">删除</td>\n");
	}
	printf("</tr>\n");
	cuid=getenv("HTTP_COOKIE");
	if(strncmp(cuid,"uid=",4)==0){
		cuid+=4;
	}
	while(row=mysql_fetch_row(result)){
		tid=atoll(row[0]);
		uid=atoll(row[1]);
		uname=row[2];
		pid=atoll(row[3]);
		ttime=atoi(row[4]);
		trss=atoi(row[5]);
		tsize=atoi(row[6]);
		tstat=atoi(row[7]);
		tctime=row[8];
		printf("<tr bgcolor=\"#EAEFFD\">\n");
		printf("<td>%lld</td>\n",tid);
		printf("<td><a href=\"user_info.html?uid=%lld\">%s</a></td>\n",uid,uname);
		printf("<td><a href=\"prob_info.html?pid=%lld\">%lld</a></td>\n",pid,pid);
		printf("<td>%luMS</td>\n",ttime);
		printf("<td>%luK</td>\n",trss);
		if(cuid==NULL||(atoll(cuid)!=1&&atoll(cuid)!=uid)){
			printf("<td>%luB</td>\n",tsize);
		}else{
			printf("<td><a href=\"test_info.html?tid=%lld\">%luB</a></td>\n",tid,tsize);
		}
		printf("<td>%s</td>\n",status[tstat+1]);
		printf("<td>%s</td>\n",tctime);
		if(is_root()){
			printf("<td><a href=\"test_delete.fcgi?tid=%lld\">DEL</a></td>\n",tid);
		}
		printf("</tr>\n");
	}
	printf("</table>\n");
	mysql_free_result(result);
	if(n>0){
		html_page_split("test_list.html",n,i);
	}
}

void test_list_html()
{
	html_frame("提交列表",test_list_page);
}

void test_delete_fcgi()
{
	param_env *p;
	char *tid;
	char sql[1000];
	p=param_new(4);
	param_query_string_set(p);
	tid=param_get(p,"tid");
	sprintf(sql,"delete from t_test where id=%s",tid);
	param_del(p);
	mysql_query(&mysql,sql);
	printf("Location: test_list.html\r\n");
	printf("\r\n");
}

void test_info_page()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	param_env *p;
	char *tid;
	const unsigned char *ttxt;
	p=param_new(4);
	param_query_string_set(p);
	tid=param_get(p,"tid");
	sprintf(sql,"select txt from t_test where id=%s",tid);
	param_del(p);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	row=mysql_fetch_row(result);
	ttxt=row[0];
	html_txt_print(ttxt);
	mysql_free_result(result);
}

void test_info_html()
{
	html_frame("查看提交代码",test_info_page);
}

