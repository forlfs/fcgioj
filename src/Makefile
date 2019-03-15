#-loj必须放在命令末尾，不然报错
index.fcgi: index.c liboj.a
	cc -L/usr/local/lib -lfcgi -I/usr/local/mysql/include -L/usr/local/mysql/lib -lmysqlclient -o index.fcgi http_user.c http_prob.c http_test.c http_disc.c http_log.c http_info.c index.c -L. -loj
liboj.a: snowflake.c param_lib.c html_lib.c db_lib.c test_lib.c log_lib.c
	cc -c -L/usr/local/lib -lfcgi -I/usr/local/mysql/include -L/usr/local/mysql/lib -lmysqlclient snowflake.c param_lib.c html_lib.c db_lib.c test_lib.c log_lib.c -D_GNU_SOURCE
	ar rs liboj.a snowflake.o param_lib.o html_lib.o db_lib.o test_lib.o log_lib.o
install: index.fcgi
	cp index.fcgi ../
uninstall:
	rm ../index.fcgi
clean:
	rm liboj.a snowflake.o param_lib.o html_lib.o db_lib.o test_lib.o log_lib.o
	rm index.fcgi

