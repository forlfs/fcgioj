#ifndef __PARAM_LIB_H__
#define __PARAM_LIB_H__

typedef struct param_env{
	char **vec,**cur;
	int size;
}param_env;

param_env *param_new(int);
void param_del(param_env *);
void param_set(param_env *,char *);
char *param_get(param_env *,char *);
char *param_get_x(param_env *,char *,int *);
int param_dec(char *,char *);
void param_query_string_parse(param_env *,char *);
void param_form_data_parse(param_env *,char *,char *);
void param_query_string_set(param_env *);
void param_form_data_set(param_env *);
void param_print(param_env *);

#endif

