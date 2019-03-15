#include<stdlib.h>
#include<string.h>
#include"fcgi_stdio.h"
#include"param_lib.h"

param_env *param_new(int size)
{
	param_env *p;
	p=(param_env *)malloc(sizeof(param_env));
	p->vec=(char **)malloc(size*sizeof(char *));
	p->size=size;
	p->cur=p->vec;
	*p->cur=NULL;
	return p;
}

void param_del(param_env *p)
{
	char **s;
	if(p==NULL){
		return;
	}
	for(s=p->vec;s<p->cur;s++) {
		free(*s);
	}
	free(p->vec);
	free(p);
}

void param_set(param_env *p,char *s)
{
	int size;
	*p->cur++=s;
	size=p->cur-p->vec;
	if(size>p->size){
		p->size<<=1;
	}
	p->vec=(char **)realloc(p->vec,p->size*sizeof(char *));
	p->cur=p->vec+size;
	*p->cur=NULL;
}

char *param_get(param_env *p,char *name)
{
	char **s;
	int i;
	if(p==NULL||name==NULL){
		return NULL;
	}
	for(s=p->vec;s<p->cur;s++){
		i=strlen(name);
		if(strncmp(*s+4,name,i)==0&&(*s+4)[i]=='='){
			return *s+4+i+1;
		}
	}
	return NULL;
}

char *param_get_x(param_env *p,char *name,int *l)
{
	char **s;
	int i;
	if(p==NULL||name==NULL){
		return NULL;
	}
	for(s=p->vec;s<p->cur;s++){
		i=strlen(name);
		if(strncmp(*s+4,name,i)==0&&(*s+4)[i]=='='){
			*l=*(int *)*s;
			return *s+4+i+1;
		}
	}
	return NULL;
}

int param_dec(char *s,char *r)
{
	int i,j;
	for(i=0,j=0;s[i]!='\0';i++,j++){
		if(s[i]=='+'){
			r[j]=' ';
			continue;
		}
		if(s[i]=='%'){
			i++;
			if(s[i]>='A'){
				r[j]=s[i]-'A'+10;
			}else{
				r[j]=s[i]-'0';
			}
			i++;
			if(s[i]>='A'){
				r[j]=(r[j]<<4)|(s[i]-'A'+10);
			}else{
				r[j]=(r[j]<<4)|(s[i]-'0');
			}
			if(r[j]=='\r'){
				j--;
			}
			continue;
		}
		r[j]=s[i];
	}
	r[j]='\0';
	return j;
}

void param_query_string_parse(param_env *p,char *s)
{
	int i,j;
	char *r;
	if(s==NULL){
		return;
	}
	for(i=0;s[i]!='\0';){
		j=i;
		while(s[j]!='&'&&s[j]!='\0'){
			j++;
		}
		r=(char *)malloc((4+j-i+1)*sizeof(char));
		*(int *)r=0;
		strncpy(r+4,&s[i],j-i);
		r[4+j-i]='\0';
		*(int *)r=param_dec(r+4,r+4);
		param_set(p,r);
		if(s[j]=='\0'){
			i=j;
		}else{
			i=j+1;
		}
	}
}

void param_form_data_parse(param_env *p,char *s,char *e)
{
	int i,j,l,n;
	char *h,*r;
	n=strstr(s,"\r\n")-s;
	h=(char *)malloc((n+1)*sizeof(char));
	strncpy(h,s,n);
	h[n]='\0';
	while(s[n]!='-'){
		j=i=strstr(s,"name=\"")-s+6;
		while(s[j]!='\"'){
			j++;
		}
		r=(char *)malloc((4+j-i+2)*sizeof(char));
		*(int *)r=0;
		strncpy(r+4,s+i,j-i);
		r[4+j-i]='=';
		r[4+j-i+1]='\0';
		l=4+j-i+1;
		i=strstr(s+j,"\r\n\r\n")-s+4;
		j=(char *)memmem(s+i,e-(s+i),h,n)-s-2;
		r=(char *)realloc(r,(l+j-i+1)*sizeof(char));
		memcpy(r+l,s+i,j-i);
		r[l+j-i]='\0';
		*(int *)r=j-i;
		param_set(p,r);
		s=s+j+2;
	}
	free(h);
}

void param_print(param_env *p){
	char **s;
	for(s=p->vec;s<p->cur;s++){
		printf("%s\n",*s+4);
	}
}

void param_query_string_set(param_env *p)
{
	char *s;
	long i;
	s=getenv("QUERY_STRING");
	param_query_string_parse(p,s);
	i=atol(getenv("CONTENT_LENGTH"));
	s=(char *)malloc((i+1)*sizeof(char));
	fread(s,sizeof(char),i,stdin);
	s[i]='\0';
	param_query_string_parse(p,s);
	free(s);
}

void param_form_data_set(param_env *p)
{
	char *s;
	long i;
	i=atol(getenv("CONTENT_LENGTH"));
	s=(char *)malloc((i+1)*sizeof(char));
	fread(s,sizeof(char),i,stdin);
	s[i]='\0';
	param_form_data_parse(p,s,s+i);
	free(s);
}

