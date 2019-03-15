create database fcgioj;
use fcgioj;

/*
select * from t_user;
select * from t_prob;
select * from t_test;
select * from t_prob_try;
select * from t_disc;
select * from t_log;
*/

/*
drop table t_user;
drop table t_prob;
drop table t_test;
drop table t_prob_try;
drop table t_disc;
drop table t_log;
*/

create table t_user(
id bigint primary key,
name varchar(100) not null unique,
pwd varchar(100) not null,
email varchar(100) not null,
info varchar(100) not null,
ctime datetime not null
)engine=innodb default charset=utf8;

insert into t_user values(1,'fcgioj','fcgioj','test','fcgioj@qq.com',sysdate());

create table t_prob(
id bigint primary key,
name varchar(100) not null,
author varchar(100) not null,
source varchar(100) not null,
hard int not null,
ctime datetime not null
)engine=innodb default charset=utf8;
insert into t_prob values(1001,'线段树','www','www',1,sysdate());
insert into t_prob values(1010,'快速幂','www','www',1,sysdate());

create table t_test(
id bigint primary key,
user_id bigint,
prob_id bigint,
run_time int not null,
run_rss int not null,
txt_size int not null,
txt text not null,
stat int not null,
ctime datetime not null
)engine=innodb default charset=utf8;

create table t_prob_try(
user_id bigint,
prob_id bigint,
stat int not null,
primary key (user_id,prob_id)
)engine=innodb default charset=utf8;

create table t_disc(
id bigint primary key,
user_id bigint,
prob_id bigint,
txt text not null,
ctime datetime not null
)engine=innodb default charset=utf8;

delimiter //
create trigger tr_t_test_i after insert on t_test
for each row
begin
	insert into t_prob_try select new.user_id,new.prob_id,if(new.stat=0,1,0) from dual
	where not exists(select 1 from t_prob_try where user_id=new.user_id and prob_id=new.prob_id);
	
	if new.stat=0 then
		update t_prob_try set stat=1 where user_id=new.user_id and prob_id=new.prob_id;
	end if;
end //
delimiter ;

delimiter //
create trigger tr_t_test_u after update on t_test
for each row
begin
	insert into t_prob_try select new.user_id,new.prob_id,if(new.stat=0,1,0) from dual
	where not exists(select 1 from t_prob_try where user_id=new.user_id and prob_id=new.prob_id);
	
	if new.stat=0 then
		update t_prob_try set stat=1 where user_id=new.user_id and prob_id=new.prob_id;
	end if;
end //
delimiter ;

create table t_log(
id bigint primary key,
txt text not null,
ctime datetime not null
)engine=innodb default charset=utf8;

