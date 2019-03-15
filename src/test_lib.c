#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include"fcgi_stdio.h"
#include"mysql.h"
#include"db_lib.h"
#include"test_lib.h"

char **envp;
MYSQL mysql;

unsigned long test_stat_time(char *s)
{
	unsigned long a,r;
	int i,j;
	for(i=0,j=0;i<13;i++,j++){
		while(s[j]!=' '){
			j++;
		}
	}
	for(a=0;s[j]!=' ';j++){
		a=a*10+s[j]-'0';
	}
	for(r=0,j++;s[j]!=' ';j++){
		r=r*10+s[j]-'0';
	}
	r+=a;
	return r;
}

unsigned long test_stat_rss(char *s)
{
	unsigned long r;
	int i,j;
	for(i=0,j=0;i<23;i++,j++){
		while(s[j]!=' '){
			j++;
		}
	}
	for(r=0;s[j]!=' ';j++){
		r=r*10+s[j]-'0';
	}
	return r;
}

char *test_stat_new(pid_t pid)
{
	char p[120];
	char *s;
	ssize_t i;
	int fd;
	sprintf(p,"/proc/%d/stat",pid);
	fd=open(p,O_RDONLY);
	s=(char *)malloc(1024);
	i=read(fd,s,1024);
	s[i]='\0';
	close(fd);
	return s;
}

void test_stat_del(char *s)
{
	free(s);
}

void test_file_new(char *p,const unsigned char *s)
{
	int fd;
	fd=open(p,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
	write(fd,s,strlen(s));
	close(fd);
}

void test_file_del(char *p)
{
	unlink(p);
}

off_t test_file_size(char *p)
{
	struct stat fs;
	stat(p,&fs);
	return fs.st_size;
}

int test_file_diff(char *pa,char *pr)
{
	struct stat sa,sr;
	int fa,fr;
	char ba[1024],br[1024];
	ssize_t i;
	stat(pa,&sa);
	stat(pr,&sr);
	if(sa.st_size!=sr.st_size){
		return 1;
	}
	fa=open(pa,O_RDONLY);
	fr=open(pr,O_RDONLY);
	while((i=read(fa,ba,1024))>0){
		read(fr,br,1024);
		if(strncmp(ba,br,i)){
			close(fa);
			close(fr);
			return 1;
		}
	}
	close(fa);
	close(fr);
	return 0;
}

int test_cc(test_stat *ts)
{
	pid_t pid;
	int status;
	char p[120],c[120];
	if((pid=fork())==0){
		sprintf(p,"test/%lld/%lld",ts->tid,ts->tid);
		sprintf(c,"test/%lld/%lld.c",ts->tid,ts->tid);
		execle("/usr/bin/cc","cc","-o",p,c,NULL,envp);
	}
	wait(&status);
	if(WIFEXITED(status)){
		if(WEXITSTATUS(status)){
			return CE;
		}
		return AC;
	}
	return SE;
}

int test_c(test_stat *ts)
{
	char p[120];
	int fd;
	sprintf(p,"prob/%lld/%lld.in",ts->pid,ts->pid);
	fd=open(p,O_RDONLY);
	dup2(fd,STDIN_FILENO);
	close(fd);
	sprintf(p,"test/%lld/%lld.out",ts->tid,ts->tid);
	fd=open(p,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
	dup2(fd,STDOUT_FILENO);
	close(fd);
	sprintf(p,"test/%lld/%lld",ts->tid,ts->tid);
	execle(p,"0",NULL,envp);
}

int test_ans(test_stat *ts)
{
	pid_t pid;
	int status;
	int fd[2];
	char sync;
	char p[120],a[120];
	char *s;
	unsigned long cur;
	off_t size;
	pipe2(fd,O_CLOEXEC);
	if((pid=fork())==0){
		test_c(ts);
	}
	close(fd[1]);
	read(fd[0],&sync,1);
	close(fd[0]);
	sprintf(p,"test/%lld/%lld.out",ts->tid,ts->tid);
	while(waitpid(pid,&status,WNOHANG)==0){
		s=test_stat_new(pid);
		ts->time=test_stat_time(s);
		ts->time=ts->time*1000/sysconf(_SC_CLK_TCK);
		cur=test_stat_rss(s);
		cur=cur*sysconf(_SC_PAGESIZE)/1024;
		if(ts->rss<cur){
			ts->rss=cur;
		}
		size=test_file_size(p);
		size/=1024;
		if(ts->time>TLE_L){
			kill(pid,SIGKILL);
			wait(&status);
			return TLE;
		}
		if(ts->rss>MLE_L){
			kill(pid,SIGKILL);
			wait(&status);
			return MLE;
		}
		if(size>OLE_L){
			kill(pid,SIGKILL);
			wait(&status);
			return OLE;
		}
		test_stat_del(s);
		usleep(1);
	}
	sprintf(a,"prob/%lld/%lld.out",ts->pid,ts->pid);
	if(WIFEXITED(status)){
		if(test_file_diff(a,p)){
			return WA;
		}
		return AC;
	}
	if(WIFSIGNALED(status)){
		return RE;
	}
	return SE;
}

void test_run()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	const unsigned char *ltxt;
	char p[120];
	sprintf(sql,"select id,prob_id,txt from t_test where stat=-1 limit 1");
	mysql_query(&mysql,sql);
	result=mysql_store_result(&mysql);
	if(!(row=mysql_fetch_row(result))){
		mysql_free_result(result);
		return;
	}
	test_stat ts;
	memset(&ts,0,sizeof(ts));
	ts.tid=atoll(row[0]);
	ts.pid=atoll(row[1]);
	ltxt=row[2];
	sprintf(p,"test/%lld",ts.tid);
	mkdir(p,S_IRUSR|S_IWUSR|S_IXUSR);
	sprintf(p,"test/%lld/%lld.c",ts.tid,ts.tid);
	test_file_new(p,ltxt);
	mysql_free_result(result);
	if((ts.ans=test_cc(&ts))==0){
		ts.ans=test_ans(&ts);
	}
	sprintf(sql,"update t_test set run_time=%lu,run_rss=%lu,stat=%d where id=%lld",ts.time,ts.rss,ts.ans,ts.tid);
	mysql_query(&mysql,sql);
	test_file_del(p);
	sprintf(p,"test/%lld/%lld",ts.tid,ts.tid);
	test_file_del(p);
	sprintf(p,"test/%lld/%lld.out",ts.tid,ts.tid);
	test_file_del(p);
	sprintf(p,"test/%lld",ts.tid);
	rmdir(p);
}

void test_daemon_open()
{
	pid_t pid;
	envp=environ;
	if((pid=fork())==0){
		db_open(&mysql);
		while(1){
			test_run();
			sleep(1);
		}
		db_close(&mysql);
		exit(0);
	}
}

void test_daemon_close()
{
}

