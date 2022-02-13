create table R2(id int, price float, name string );
create table R3(id int, price float, name string );
create table R1(id int, price float, name string );
create table R4(id int, price float, name string );
show distribution R1 id;
insert into R1(1, 0.2, "Lam");
insert into R1(2, 0.3, "Lamm");
insert into R1(3, 0.4, "Lammm");
insert into R1(1, 1.2, "Lam");
insert into R1(2, 1.3, "Lam");
insert into R1(3, 1.4, "Lamm");
insert into R2(3, 1.4, "Lamm");
insert into R3(3, 1.4, "Lamm");
insert into R4(3, 1.4, "Lamm");
insert into R2(5,357.15,"test");
insert into R2(2,357.15,"test");
insert into R3(3,324.2,"test");
insert into R3(6,324.2,"test");
insert into R3(36,312.7,"Lam");
show distribution R1 id;
select * from R1, R2, R3 where R1.id < R3.id;

select R1.id, R2.price, R3.name from R1, R2, R3, R4 where R1.id < 4 and R1.id < R2.id and R3.price > 100 and R3.id > R4.id;

select * from R1 where id<3;
select * from R2;
select * from R1, R2;
select R1.id,R2.id from R1,R2 where R1.id<R2.id;

select * from R1;
update R1 set R1.name="somebody" where R1.id=3;
select * from R1;

select * from R2;
delete from R2 where R2.id = 2;
select * from R2;