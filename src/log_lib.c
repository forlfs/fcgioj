#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include<fcntl.h>
#include"fcgi_stdio.h"
#include"snowflake.h"
#include"log_lib.h"

void write_log(char *prefix,char *log)
{
	char p[120];
	int fd;
	time_t t;
	struct tm *dt;
	char s[100];
	time(&t);
	dt=localtime(&t);
	sprintf(p,"log/%s-%04d%02d%02d.log",prefix,dt->tm_year+1900,dt->tm_mon+1,dt->tm_mday);
	fd=open(p,O_CREAT|O_WRONLY|O_APPEND,S_IRUSR|S_IWUSR);
	sprintf(s,"%04d-%02d-%02d %02d:%02d:%02d ",dt->tm_year+1900,dt->tm_mon+1,dt->tm_mday,dt->tm_hour,dt->tm_min,dt->tm_sec);
	write(fd,s,strlen(s));
	write(fd,log,strlen(log));
	write(fd,"\n",1);
	close(fd);
}

void write_db_log(char *log)
{
	char sql[10000];
	long long lid;
	int i;
	char *txt;
	i=strlen(log);
	txt=(char *)malloc((i<<1)+1);
	mysql_real_escape_string(&mysql,txt,log,i);
	lid=snowflake_id();
	if(lid==-1){
		free(txt);
		return;
	}
	sprintf(sql,"insert into t_log(id,txt,ctime) values(%lld,'%s',sysdate())",lid,txt);
	free(txt);
	mysql_query(&mysql,sql);
}

