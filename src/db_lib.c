#include<unistd.h>
#include"mysql.h"
#include"db_lib.h"

void db_open(MYSQL *mysql)
{
	mysql_init(mysql);
	while(!mysql_real_connect(mysql,"127.0.0.1","root","","fcgioj",0,NULL,0)){
		sleep(1);
	}
}

void db_close(MYSQL *mysql)
{
	mysql_close(mysql);
}

