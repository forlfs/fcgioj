#ifndef __HTML_LIB_H__
#define __HTML_LIB_H__

#include"mysql.h"

#define PAGE_ITEM_NUM 10

extern MYSQL mysql;

int is_root();
int is_login();
void html_txt_print(const char *);
void html_page_split(char *,int,int);
void html_frame(char *,void (*)());

#endif

