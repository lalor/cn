---
layout: post
title: "shell中读写mysql数据库"
description: ""
category: 程序语言
tags: [shell, mysql]
---

本文介绍了如何在shell中读写mysql数据库。主要介绍了如何在shell
中连接mysql数据库，如何在shell中创建数据库，创建表，插入csv文件，读取mysql数据库，导出mysql数据库为xml或html文件，
并分析了核心语句。本文介绍的方法适用于PostgreSQL ，相对mysql而言，shell
中读写PostgreSQL会更简单些。


#1. 连接mysql 数据库

shell中连接数据库的方法很简单，只需要指定用户名，密码，连接的数据库名称，然后通过重定向，输入mysql的语句，如下所示：

	mysql -u USERNAME -p PASSWORD DATABASENAME <<EOF 2>/dev/null
		show databases;
	EOF

但这并不是一个好办法，任何使用该脚本的用户都能看到该数据库用户的账号和密码，要解决这个问题，可以用mysql
数据库的一个特殊配置文件。mysql 数据库使用`$HOME/.my.cnf`
文件来读取特殊的启动命令和设置。其中一项设置是由该用户账户发起的mysql
会话的默认密码。要在这个文件中设置默认密码，可以加入下面的内容：

	[client]
	password = 123456

然后，别忘了修改权限：
	
	chmod 400  .my.cnf

这样就可以通过脚本访问mysql数据库了，如下所示：

	#!/bin/bash
	MYSQL=`which mysql`
	$MYSQL test -u root << EOF
	show databases;
	show tables;
	select * from employees where salary > 4000;
	EOF


#2. 创建数据库

通过上面的方法连接数据库，再通过重定向输入mysql语句，shell中读写mysql基本就介绍完了。只要把sql语句写对了，通过重定向执行即可，下面来看一个实例：

	#!/bin/bash
	##############################
	# @file create_db_mysql.sh
	# @brief create database and tables in mysql
	# @author Mingxing LAI
	# @version 0.1
	# @date 2013-01-20
	##############################
	USER="root"
	DATABASE="students"
	TABLE="students"
	
	######################
	#crate database
	mysql -u $USER << EOF 2>/dev/null
	CREATE DATABASE $DATABASE
	EOF
	[ $? -eq 0 ] && echo "created DB" || echo DB already exists
	
	######################
	#create table
	mysql -u $USER $DATABASE << EOF 2>/dev/null
	CREATE TABLE $TABLE(
	id int,
	name varchar(100),
	mark int,
	dept varchar(4)
	);
	EOF
	[ $? -eq 0 ] && echo "Created table students" || echo "Table students already exist" 
	
	
	######################
	#delete data
	mysql -u $USER $DATABASE << EOF 2>/dev/null
	DELETE FROM $TABLE;
	EOF

这个脚本比较简单，就是几条SQL语句，没什么好解释的，下面来看一下，如何读入csv
文件，然后插入到mysql数据库中。


#3. 插入csv 文件

上面创建了一个学生表，表中有学生的学号，姓名，成绩，系别，假设有一个csv文件，内容如下：

	$cat data 
	1,Navin M,98,CS
	2,Kavya N,70,CS
	3,Nawaz O,80,CS
	4,Hari S,80,EC
	5,Alex M,50,EC
	6,Neenu J,70,EC
	7,Bob A,30,EC
	8,Anu M,90,AE
	9,Sruthi,89,AE
	10,Andrew,89,AE

为了将csv
文件插入到数据库，我们需要逐行读入，然后给字符串加上双引号，最后生成语句如下：

	insert into students VALUES(1, "Navin M", 98, "CS");

要解析csv
文件，最好的工具莫过于awk了，将域的分隔符指定为逗号`-F,`，awk就自动将各个域拆分出来了，然后在需要双引号的地方打印输出一个双引号，就能够轻松得到下面这样的数据：

	1, "Navin M", 98, "CS"

awk 代码如下：

	query=`echo $line | awk -F, '{ printf("%s,\"%s\",%s,\"%s\"", $1, $2, $3, $4)}'`
	statement=`echo "INSERT INTO $TABLE VALUES($query);"`
	echo $statement

当然了，你也可以用其他办法，不过，几乎没有比awk更简单的了，第2种方法如下：

	oldIFS=$IFS
	IFS=,
	values=($line)
	
	values[1]="\"`echo ${values[1]} | tr ' ' '#' `\""
	values[3]="\"`echo ${values[3]}`\""
	
	query=`echo ${values[@]} | tr ' #' ', '`
	IFS=$oldIFS
	
	statement=`echo "INSERT INTO $TABLE VALUES($query);"`
	echo "$statement"

首先通过指定域分隔符，将csv文件解析成一个数组，然后将空格替换成一个特殊的符号"#"（因为后面的替换中，会一次性输出数组，而数组是用空格分隔各字段,我们要将分隔数组的空格替换成逗号，所以这里将数据中的空格替换成"#"） ，给字符串加上双引号，最后再把空格替换成逗号，把"#"替换为空格。这种方法真是让人抓狂，我第一次就没有看明白，尤其是为什么要将空格替换成"#"。


完整的插入数据的程序如下：

	#!/bin/bash
	#
	# @file write_to_db_mysql.sh
	# @brief wirte data to database in mysql
	# @author Mingxing LAI
	# @version 0.1
	# @date 2013-01-20
	#
	USER="root"
	DATABASE="students"
	TABLE="students"
	
	if [ $# -ne 1 ]; then
		echo $0 DATAFILE
		echo
		exit 2
	fi
	
	
	data=$1
	while  read line;
	do
	#	query=`echo $line | awk -F, '{ printf("%s,\"%s\",%s,\"%s\"", $1, $2, $3, $4)}'`
		oldIFS=$IFS
		IFS=,
		values=($line)
		
		values[1]="\"`echo ${values[1]} | tr ' ' '#' `\""
		values[3]="\"`echo ${values[3]}`\""
		
		query=`echo ${values[@]} | tr ' #' ', '`
		IFS=$oldIFS
	
		statement=`echo "INSERT INTO $TABLE VALUES($query);"`
	#	echo $statement
	
	mysql -u $USER $DATABASE << EOF
		INSERT INTO $TABLE VALUES($query);
	EOF
		
	done < $data
	
	if [[ $? -eq 0 ]]; then
		echo "Wrote data into DB"
	fi

#4. 读取数据

知道怎么在shell 中连接mysql ，也知道了怎么在shell中批量执行sql
语句，读取数据，就没有任何难度了。

	#!/bin/bash
	#
	# @file read_db_mysql.sh
	# @brief read data from mysql
	# @author Mingxing LAI
	# @version 0.1
	# @date 2013-01-20
	#
	
	USER="root"
	DATABASE="students"
	TABLE="students"
	
	#用tail 去掉表头
	depts=`mysql -u $USER $DATABASE <<EOF | tail -n +2
	SELECT DISTINCT dept FROM $TABLE;
	EOF`
	
	for d in $depts; do
		echo Department: $d
		result="`mysql -u $USER $DATABASE << EOF
		set @i:=0;
		SELECT @i:=@i+1 as rank, name, mark FROM students WHERE dept="$d" ORDER BY mark DESC;
	EOF`"
	echo "$result"
	echo 
	done

我们还可以在mysql语句中，使用选项来控制数据的输出格式

* -H 输出为html
* -X 输出为xml

如下所示：

	#!/bin/bash
	
	USER="root"
	DATABASE="students"
	TABLE="students"
	
	mysql -u $USER $DATABASE -H << EOF
	select * from $TABLE
	EOF

####html 格式的可读性比较差，输出效果如下：

	<TABLE BORDER=1><TR><TH>id</TH><TH>name</TH><TH>mark</TH><TH>dept</TH></TR><TR><TD>1</TD><TD>Navin M</TD><TD>98</TD><TD>CS</TD></TR><TR><TD>2</TD><TD> Kavya N</TD><TD>70</TD><TD>CS</TD></TR><TR><TD>3</TD><TD> Nawaz O</TD><TD>80</TD><TD>CS</TD></TR><TR><TD>4</TD><TD>Hari S</TD><TD>80</TD><TD>EC</TD></TR><TR><TD>5</TD><TD>Alex M</TD><TD>50</TD><TD>EC</TD></TR><TR><TD>6</TD><TD>Neenu J</TD><TD>70</TD><TD>EC</TD></TR><TR><TD>7</TD><TD>Bob A</TD><TD>30</TD><TD>EC</TD></TR><TR><TD>8</TD><TD>Anu M</TD><TD>90</TD><TD>AE</TD></TR><TR><TD>9</TD><TD>Sruthi</TD><TD>89</TD><TD>AE</TD></TR><TR><TD>10</TD><TD>Andrew</TD><TD>89</TD><TD>AE</TD></TR></TABLE>

可读性差也可以理解，因为人家觉得，你没必要修改么，直接以html形式展示数据就可以了。
<TABLE BORDER=1><TR><TH>id</TH><TH>name</TH><TH>mark</TH><TH>dept</TH></TR><TR><TD>1</TD><TD>Navin M</TD><TD>98</TD><TD>CS</TD></TR><TR><TD>2</TD><TD> Kavya N</TD><TD>70</TD><TD>CS</TD></TR><TR><TD>3</TD><TD> Nawaz O</TD><TD>80</TD><TD>CS</TD></TR><TR><TD>4</TD><TD>Hari S</TD><TD>80</TD><TD>EC</TD></TR><TR><TD>5</TD><TD>Alex M</TD><TD>50</TD><TD>EC</TD></TR><TR><TD>6</TD><TD>Neenu J</TD><TD>70</TD><TD>EC</TD></TR><TR><TD>7</TD><TD>Bob A</TD><TD>30</TD><TD>EC</TD></TR><TR><TD>8</TD><TD>Anu M</TD><TD>90</TD><TD>AE</TD></TR><TR><TD>9</TD><TD>Sruthi</TD><TD>89</TD><TD>AE</TD></TR><TR><TD>10</TD><TD>Andrew</TD><TD>89</TD><TD>AE</TD></TR></TABLE>

xml形式的数据显示就比较正常了，直接将上面的-H 换成-X,输出如下：


	<?xml version="1.0"?>
	
	<resultset statement="select * from students
	" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
	  <row>
		<field name="id">1</field>
		<field name="name">Navin M</field>
		<field name="mark">98</field>
		<field name="dept">CS</field>
	  </row>
	
	  <row>
		<field name="id">2</field>
		<field name="name"> Kavya N</field>
		<field name="mark">70</field>
		<field name="dept">CS</field>
	  </row>
	</resultset>

完。
