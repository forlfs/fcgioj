#ifndef __LOG_LIB_H__
#define __LOG_LIB_H__

#include"mysql.h"

extern MYSQL mysql;

void write_log(char *prefix,char *log);
void write_db_log(char *log);

#endif

