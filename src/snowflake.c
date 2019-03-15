#include<stdlib.h>
#include<sys/time.h>
#include"snowflake.h"

long long twepoch=0;
long long last_timestamp=0;
int sequence=0;

long long snowflake_time()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (long long)tv.tv_sec*1000+tv.tv_usec/1000;
}

long long snowflake_til_next_millis()
{
	long long timestamp=snowflake_time();
	while(timestamp<=last_timestamp){
		timestamp=snowflake_time();
	}
	return timestamp;
}

long long snowflake_id()
{
	long long timestamp=snowflake_time();
	if(timestamp<last_timestamp){
		return -1;
	}
	if(timestamp==last_timestamp){
		sequence=(sequence+1)&(-1^(-1<<12));
		if(sequence==0){
			timestamp=snowflake_til_next_millis();
		}
	}else{
		sequence=0;
	}
	last_timestamp=timestamp;
	return ((timestamp-twepoch)<<22)|(0<<12)|sequence;
}

