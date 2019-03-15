#include<string.h>
#include"fcgi_stdio.h"
#include"html_lib.h"
#include"http_info.h"

void sys_info_page()
{
	time_t t;
	struct tm *dt;
	time(&t);
	dt=localtime(&t);
	printf("current time: %04d-%02d-%02d %02d:%02d:%02d<br/>",dt->tm_year+1900,dt->tm_mon+1,dt->tm_mday,dt->tm_hour,dt->tm_min,dt->tm_sec);
	printf("start time: %ds<br/>",t-start_time);
	printf("visit times: %d<br/>",visit_times);
}

void sys_info_html()
{
	html_frame("系统信息",sys_info_page);
}

