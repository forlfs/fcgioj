#ifndef __TEST_LIB_H__
#define __TEST_LIB_H__

#define AC 0
#define CE 1
#define WA 2
#define RE 3
#define TLE 4
#define MLE 5
#define OLE 6
#define SE 7

#define TLE_L 1000
#define MLE_L 10240
#define OLE_L 1024

extern char **environ;

typedef struct test_stat{
	int ans;
	long long pid,tid;
	unsigned long time,rss;
}test_stat;

void test_daemon_open();
void test_daemon_close();

#endif

