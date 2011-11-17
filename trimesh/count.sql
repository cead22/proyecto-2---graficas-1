select count(*) from GRUPO15_NATION;
select count(*) from NATION;
select count(*) from GRUPO15_REGION;
select count(*) from REGION;
select count(*) from GRUPO15_CUSTOMER;
select count(*) from CUSTOMER;
select count(*) from GRUPO15_PART;
select count(*) from PART;
select count(*) from GRUPO15_PARTSUPPLIER;
select count(*) from PARTSUPPLIER;
select count(*) from GRUPO15_SUPPLIER;
select count(*) from SUPPLIER;
select count(*) from GRUPO15_LINEITEM;
select count(*) from LINEITEM;
select count(*) from GRUPO15_ORDERS;
select count(*) from ORDERS;


drop table GRUPO15_NATION;
drop table NATION;
drop table GRUPO15_REGION;
drop table REGION;
drop table GRUPO15_CUSTOMER;
drop table CUSTOMER;
drop table GRUPO15_PART;
drop table PART;
drop table GRUPO15_PARTSUPPLIER;
drop table PARTSUPPLIER;
drop table GRUPO15_SUPPLIER;
drop table SUPPLIER;
drop table GRUPO15_LINEITEM;
drop table LINEITEM;
drop table GRUPO15_ORDERS;
drop table ORDERS;

select tablespace_name from user_Tables where tablespace_name like "%15%";