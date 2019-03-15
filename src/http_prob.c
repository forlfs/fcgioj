#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include"fcgi_stdio.h"
#include"snowflake.h"
#include"param_lib.h"
#include"html_lib.h"
#include"test_lib.h"
#include"http_prob.h"

void prob_list_page()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	param_env *p;
	long long pid;
	int phard;
	const unsigned char *pname,*pauthor,*psource,*pctime;
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
	sprintf(sql,"select count(*) from t_prob");
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
	sprintf(sql,"select id,name,author,source,hard,ctime from t_prob order by id limit %d offset %d",PAGE_ITEM_NUM,i*PAGE_ITEM_NUM);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	printf("<table width=\"100%%\">\n");
	printf("<tr bgcolor=\"#6589D1\">\n");
	printf("<td width=\"10%%\">题目ID</td>\n");
	printf("<td width=\"30%%\">题名</td>\n");
	printf("<td width=\"10%%\">作者</td>\n");
	printf("<td width=\"10%%\">来源</td>\n");
	printf("<td width=\"10%%\">难度</td>\n");
	if(is_root()==0){
		printf("<td width=\"30%%\">加入时间</td>\n");
	}else{
		printf("<td width=\"20%%\">加入时间</td>\n");
		printf("<td width=\"5%%\">修改</td>\n");
		printf("<td width=\"5%%\">删除</td>\n");
	}
	printf("</tr>\n");
	while(row=mysql_fetch_row(result)){
		pid=atoll(row[0]);
		pname=row[1];
		pauthor=row[2];
		psource=row[3];
		phard=atoi(row[4]);
		pctime=row[5];
		printf("<tr bgcolor=#EAEFFD>\n");
		printf("<td>%lld</td>\n",pid);
		printf("<td><a href=\"prob_info.html?pid=%lld\">%s</a></td>\n",pid,pname);
		printf("<td>%s</td>\n",pauthor);
		printf("<td>%s</td>\n",psource);
		printf("<td>%d</td>\n",phard);
		printf("<td>%s</td>\n",pctime);
		if(is_root()){
			printf("<td><a href=\"prob_update.html?pid=%lld\">UPD</a></td>\n",pid);
			printf("<td><a href=\"prob_delete.fcgi?pid=%lld\">DEL</a></td>\n",pid);
		}
		printf("</tr>");
	}
	printf("</table>");
	mysql_free_result(result);
	if(is_root()){
		printf("<a href=\"prob_new.html\">添加题目</a>");
	}
	if(n>0){
		html_page_split("prob_list.html",n,i);
	}
}

void prob_list_html()
{
	html_frame("题目列表",prob_list_page);
}

void prob_txt_print(char *pid)
{
	char p[120];
	int fd;
	struct stat fs;
	off_t n;
	char *s;
	sprintf(p,"prob/%s/%s.txt",pid,pid);
	stat(p,&fs);
	n=fs.st_size;
	s=(char *)malloc((n+1)*sizeof(char));
	fd=open(p,O_RDONLY);
	read(fd,s,n);
	s[n]='\0';
	close(fd);
	html_txt_print(s);
	free(s);
}

void prob_info_page()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	param_env *p;
	char *pid;
	const unsigned char *pname;
	char *ptxt;
	p=param_new(4);
	param_query_string_set(p);
	pid=param_get(p,"pid");
	sprintf(sql,"select name from t_prob where id=%s",pid);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	row=mysql_fetch_row(result);
	pname=row[0];
	printf("<center>%s</center>\n",pname);
	mysql_free_result(result);
	prob_txt_print(pid);
	printf("<hr />\n");
	printf("<table width=\"8%%\">\n");
	printf("<tr>\n");
	printf("<td><a href=\"prob_submit.html?pid=%s\">提交</a></td>\n",pid);
	printf("<td><a href=\"prob_discuss.html?pid=%s\">讨论</a></td>\n",pid);
	printf("</tr>\n");
	printf("</table>\n");
	param_del(p);
}

void prob_info_html()
{
	html_frame("看题",prob_info_page);
}

void prob_submit_page()
{
	param_env *p;
	char *pid;
	p=param_new(4);
	param_query_string_set(p);
	pid=param_get(p,"pid");
	printf("<form method=\"post\" action=\"prob_submit.fcgi\" enctype=\"multipart/form-data\">\n");
	printf("<table>\n");
	printf("<tr>\n");
	printf("<td>题目ID</td>\n");
	printf("<td><input type=\"text\" name=\"pid\" value=\"%s\"></td>\n",pid);
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>代码</td>\n");
	printf("<td><input type=\"file\" name=\"itxt\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td><input type=\"submit\" value=\"提交\"></td>\n");
	printf("<td><input type=\"reset\" value=\"重填\"></td>\n");
	printf("</tr>\n");
	printf("</table>\n");
	printf("</form>\n");
	param_del(p);
}

void prob_submit_html()
{
	if(is_login()==0){
		printf("Location: user_login.html\r\n");
		printf("\r\n");
		return;
	}
	html_frame("提交",prob_submit_page);
}

void prob_submit_fcgi()
{
	char sql[10000];
	param_env *p;
	long long tid;
	char *uid,*pid,*itxt;
	int i;
	char *txt;
	uid=getenv("HTTP_COOKIE")+4;
	p=param_new(4);
	param_form_data_set(p);
	pid=param_get(p,"pid");
	itxt=param_get(p,"itxt");
	i=strlen(itxt);
	txt=(char *)malloc((i<<1)+1);
	mysql_real_escape_string(&mysql,txt,itxt,i);
	tid=snowflake_id();
	if(tid==-1){
		free(txt);
		param_del(p);
		printf("Location: index.html\r\n");
		printf("\r\n");
		return;
	}
	sprintf(sql,"insert into t_test(id,user_id,prob_id,run_time,run_rss,txt_size,txt,stat,ctime) values(%lld,%s,%s,0,0,%d,'%s',-1,sysdate())",tid,uid,pid,i,txt);
	free(txt);
	param_del(p);
	mysql_query(&mysql,sql);
	printf("Location: test_list.html\r\n");
	printf("\r\n");
}

void prob_new_page()
{
	printf("<form method=\"post\" action=\"prob_new.fcgi\" enctype=\"multipart/form-data\">\n");
	printf("<table>\n");
	printf("<tr>\n");
	printf("<td>题目ID</td>\n");
	printf("<td><input type=\"text\" name=\"pid\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>题名</td>\n");
	printf("<td><input type=\"text\" name=\"pname\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>作者</td>\n");
	printf("<td><input type=\"text\" name=\"pauthor\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>来源</td>\n");
	printf("<td><input type=\"text\" name=\"psource\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>难度</td>\n");
	printf("<td><input type=\"text\" name=\"phard\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>内容</td>\n");
	printf("<td><input type=\"file\" name=\"ptxt\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>输入</td>\n");
	printf("<td><input type=\"file\" name=\"pin\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>答案</td>\n");
	printf("<td><input type=\"file\" name=\"pout\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td><input type=\"submit\" value=\"提交\"></td>\n");
	printf("<td><input type=\"reset\" value=\"重填\"></td>\n");
	printf("</tr>\n");
	printf("</table>\n");
	printf("</form>\n");
}

void prob_new_html()
{
	html_frame("添加题目",prob_new_page);
}

void prob_update_page()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	param_env *p;
	char *pid;
	const char *pname,*pauthor,*psource;
	int phard;
	p=param_new(4);
	param_query_string_set(p);
	pid=param_get(p,"pid");
	sprintf(sql,"select name,author,source,hard from t_prob where id=%s",pid);
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	row=mysql_fetch_row(result);
	pname=row[0];
	pauthor=row[1];
	psource=row[2];
	phard=atoi(row[3]);
	mysql_free_result(result);
	printf("<form method=\"post\" action=\"prob_update.fcgi\" enctype=\"multipart/form-data\">\n");
	printf("<table>\n");
	printf("<tr>\n");
	printf("<td>旧题目ID</td>\n");
	printf("<td><input type=\"text\" name=\"cpid\" value=\"%s\"></td>\n",pid);
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>新题目ID</td>\n");
	printf("<td><input type=\"text\" name=\"npid\" value=\"%s\"></td>\n",pid);
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>题名</td>\n");
	printf("<td><input type=\"text\" name=\"pname\" value=\"%s\"></td>\n",pname);
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>作者</td>\n");
	printf("<td><input type=\"text\" name=\"pauthor\" value=\"%s\"></td>\n",pauthor);
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>来源</td>\n");
	printf("<td><input type=\"text\" name=\"psource\" value=\"%s\"></td>\n",psource);
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>难度</td>\n");
	printf("<td><input type=\"text\" name=\"phard\" value=\"%d\"></td>\n",phard);
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>内容</td>\n");
	printf("<td><input type=\"file\" name=\"ptxt\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>输入</td>\n");
	printf("<td><input type=\"file\" name=\"pin\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>答案</td>\n");
	printf("<td><input type=\"file\" name=\"pout\"></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td><input type=\"submit\" value=\"提交\"></td>\n");
	printf("<td><input type=\"reset\" value=\"重填\"></td>\n");
	printf("</tr>\n");
	printf("</table>\n");
	printf("</form>\n");
	param_del(p);
}

void prob_update_html()
{
	html_frame("修改题目",prob_update_page);
}

void prob_file_new(char *p,char *s)
{
	int fd;
	fd=open(p,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
	write(fd,s,strlen(s));
	close(fd);
}

void prob_file_del(char *p)
{
	unlink(p);
}

void prob_dir_new(long long pid,char *ptxt,char *pin,char *pout)
{
	char p[120];
	sprintf(p,"prob/%lld",pid);
	mkdir(p,S_IRUSR|S_IWUSR|S_IXUSR);
	sprintf(p,"prob/%lld/%lld.txt",pid,pid);
	prob_file_new(p,ptxt);
	sprintf(p,"prob/%lld/%lld.in",pid,pid);
	prob_file_new(p,pin);
	sprintf(p,"prob/%lld/%lld.out",pid,pid);
	prob_file_new(p,pout);
}

void prob_dir_del(long long pid)
{
	char p[120];
	sprintf(p,"prob/%lld/%lld.txt",pid,pid);
	prob_file_del(p);
	sprintf(p,"prob/%lld/%lld.in",pid,pid);
	prob_file_del(p);
	sprintf(p,"prob/%lld/%lld.out",pid,pid);
	prob_file_del(p);
	sprintf(p,"prob/%lld",pid);
	rmdir(p);
}

void prob_new_fcgi()
{
	char sql[1000];
	param_env *p;
	long long pid;
	char *pname,*pauthor,*psource,*phard,*ptxt,*pin,*pout;
	p=param_new(4);
	param_form_data_set(p);
	pid=atoll(param_get(p,"pid"));
	pname=param_get(p,"pname");
	pauthor=param_get(p,"pauthor");
	psource=param_get(p,"psource");
	phard=param_get(p,"phard");
	ptxt=param_get(p,"ptxt");
	pin=param_get(p,"pin");
	pout=param_get(p,"pout");
	sprintf(sql,"insert into t_prob(id,name,author,source,hard,ctime) values(%lld,'%s','%s','%s',%s,sysdate())",pid,pname,pauthor,psource,phard);
	mysql_query(&mysql,sql);
	prob_dir_new(pid,ptxt,pin,pout);
	param_del(p);
	printf("Location: prob_list.html\r\n");
	printf("\r\n");
}

void prob_update_fcgi()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	param_env *p;
	long long npid,cpid;
	char *pname,*pauthor,*psource,*phard,*ptxt,*pin,*pout;
	char s[120];
	p=param_new(4);
	param_form_data_set(p);
	cpid=atoll(param_get(p,"cpid"));
	npid=atoll(param_get(p,"npid"));
	pname=param_get(p,"pname");
	pauthor=param_get(p,"pauthor");
	psource=param_get(p,"psource");
	phard=param_get(p,"phard");
	ptxt=param_get(p,"ptxt");
	pin=param_get(p,"pin");
	pout=param_get(p,"pout");
	if(npid!=cpid){
		sprintf(sql,"select id from t_prob where id=%lld",npid);
		mysql_query(&mysql,sql);
		result=mysql_store_result(&mysql);
		if(row=mysql_fetch_row(result)){
			printf("Location: prob_list.html\r\n");
			printf("\r\n");
			param_del(p);
			mysql_free_result(result);
			return;
		}
		mysql_free_result(result);
	}
	prob_dir_del(cpid);
	prob_dir_new(npid,ptxt,pin,pout);
	sprintf(sql,"update t_prob set id=%lld,name='%s',author='%s',source='%s',hard='%s' where id=%lld",npid,pname,pauthor,psource,phard,cpid);
	param_del(p);
	mysql_query(&mysql,sql);
	printf("Location: prob_list.html\r\n");
	printf("\r\n");
}

void prob_delete_fcgi()
{
	char sql[1000];
	param_env *p;
	long long pid;
	char s[120];
	p=param_new(4);
	param_query_string_set(p);
	pid=atoll(param_get(p,"pid"));
	prob_dir_del(pid);
	sprintf(sql,"delete from t_prob where id=%lld",pid);
	param_del(p);
	mysql_query(&mysql,sql);
	printf("Location: prob_list.html\r\n");
	printf("\r\n");
}

void prob_discuss_page()
{
	param_env *p;
	char *pid;
	p=param_new(4);
	param_query_string_set(p);
	pid=param_get(p,"pid");
	printf("<form method=\"post\" action=\"prob_discuss.fcgi\">\n");
	printf("<table>\n");
	printf("<tr>\n");
	printf("<td>题目ID</td>\n");
	printf("<td><input type=\"text\" name=\"pid\" value=\"%s\"></td>\n",pid);
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td>内容</td>\n");
	printf("<td><textarea cols=\"100\" rows=\"4\" name=\"dtxt\"></textarea></td>\n");
	printf("</tr>\n");
	printf("<tr>\n");
	printf("<td><input type=\"submit\" value=\"提交\"></td>\n");
	printf("<td><input type=\"reset\" value=\"重填\"></td>\n");
	printf("</tr>\n");
	printf("</table>\n");
	param_del(p);
}

void prob_discuss_html()
{
	if(is_login()==0){
		printf("Location: user_login.html\r\n");
		printf("\r\n");
		return;
	}
	html_frame("发表讨论",prob_discuss_page);
}

void prob_discuss_fcgi()
{
	char sql[10000];
	param_env *p;
	char *uid,*pid,*dtxt;
	long long did;
	int i;
	char *txt;
	uid=getenv("HTTP_COOKIE")+4;
	p=param_new(4);
	param_query_string_set(p);
	pid=param_get(p,"pid");
	dtxt=param_get(p,"dtxt");
	i=strlen(dtxt);
	txt=(char *)malloc((i<<1)+1);
	mysql_real_escape_string(&mysql,txt,dtxt,i);
	did=snowflake_id();
	if(did==-1){
		free(txt);
		param_del(p);
		printf("Location: index.html\r\n");
		printf("\r\n");
		return;
	}
	sprintf(sql,"insert into t_disc(id,user_id,prob_id,txt,ctime) values(%lld,%s,%s,'%s',sysdate())",did,uid,pid,txt);
	free(txt);
	param_del(p);
	mysql_query(&mysql,sql);
	printf("Location: disc_list.html\r\n");
	printf("\r\n");
}

