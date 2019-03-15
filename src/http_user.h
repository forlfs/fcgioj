#ifndef __HTTP_USER_H__
#define __HTTP_USER_H__

#include"mysql.h"

extern MYSQL mysql;

void user_login_html();
void user_login_fcgi();
void user_logout_fcgi();
void user_reg_html();
void user_reg_fcgi();
void user_update_html();
void user_update_fcgi();
void user_list_html();
void user_info_html();
void user_delete_fcgi();

#endif

