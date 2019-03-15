#ifndef __HTTP_PROB_H__
#define __HTTP_PROB_H__

#include"mysql.h"

extern MYSQL mysql;

void prob_list_html();
void prob_info_html();
void prob_submit_html();
void prob_submit_fcgi();
void prob_new_html();
void prob_new_fcgi();
void prob_update_html();
void prob_update_fcgi();
void prob_delete_fcgi();
void prob_discuss_html();
void prob_discuss_fcgi();
void prob_delete_fcgi();

#endif

