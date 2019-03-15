#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include"fcgi_stdio.h"
#include"html_lib.h"
#include"db_lib.h"
#include"test_lib.h"
#include"log_lib.h"
#include"http_user.h"
#include"http_prob.h"
#include"http_test.h"
#include"http_disc.h"
#include"http_log.h"
#include"http_info.h"
#include"index.h"

#define MODULE_NUM 30

typedef struct module{
	char *name;
	void (*func)();
}module;

module modules[MODULE_NUM]={
	{"/index.html",index_html},
	{"/user_login.html",user_login_html},
	{"/user_login.fcgi",user_login_fcgi},
	{"/user_logout.fcgi",user_logout_fcgi},
	{"/user_reg.html",user_reg_html},
	{"/user_reg.fcgi",user_reg_fcgi},
	{"/user_update.html",user_update_html},
	{"/user_update.fcgi",user_update_fcgi},
	{"/user_list.html",user_list_html},
	{"/user_info.html",user_info_html},
	{"/user_delete.fcgi",user_delete_fcgi},
	{"/prob_list.html",prob_list_html},
	{"/prob_info.html",prob_info_html},
	{"/prob_submit.html",prob_submit_html},
	{"/prob_submit.fcgi",prob_submit_fcgi},
	{"/prob_new.html",prob_new_html},
	{"/prob_new.fcgi",prob_new_fcgi},
	{"/prob_update.html",prob_update_html},
	{"/prob_update.fcgi",prob_update_fcgi},
	{"/prob_delete.fcgi",prob_delete_fcgi},
	{"/prob_discuss.html",prob_discuss_html},
	{"/prob_discuss.fcgi",prob_discuss_fcgi},
	{"/test_list.html",test_list_html},
	{"/test_delete.fcgi",test_delete_fcgi},
	{"/test_info.html",test_info_html},
	{"/disc_list.html",disc_list_html},
	{"/disc_delete.fcgi",disc_delete_fcgi},
	{"/log_list.html",log_list_html},
	{"/sys_info.html",sys_info_html},
	{"/err_404.html",err_404_html}
};

MYSQL mysql;

time_t start_time=0;
int visit_times=0;

void index_page()
{
	printf("<p>这是一个简单的OJ系统，不过麻雀虽小，五脏俱全。</p>\n");
}

void index_html()
{
	html_frame("fcgioj",index_page);
}

void err_404_page()
{
	printf("找不到页面\n");
}

void err_404_html()
{
	html_frame("找不到页面",err_404_page);
}

int main()
{
	char *s;
	int i;
	chdir("/home/www/fcgioj");
	write_log("web","start");
	db_open(&mysql);
	test_daemon_open();
	time(&start_time);
	visit_times=0;
	while(FCGI_Accept()>=0){
		visit_times++;
		s=getenv("SCRIPT_FILENAME");
		for(i=0;i<MODULE_NUM-1;i++){
			if(strcmp(s,modules[i].name)==0){
				modules[i].func();
				break;
			}
		}
		if(i==MODULE_NUM-1){
			modules[i].func();
		}
	}
	test_daemon_close();
	db_close(&mysql);
	return 0;
}

