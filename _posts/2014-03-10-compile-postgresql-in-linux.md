---
layout: post
title: "linux下源码编译PostgreSQL"
description: ""
category: 工具
tags: [MySQL, 源码编译]
---

本文介绍了在Ubuntu 12.04LTS编译PostgreSQL源码的详细步骤，并在最后给出了自动化编译的脚本。

###1. 下载PostgreSQL

请到<http://www.postgresql.org/ftp/source/>下载PostgreSQL(注意PostgreSQL的正确写法)的源码，需要说明的是，PostgreSQL v7.4.30 是最后一个缓冲区替换算法使用LRU的版本，新版本的PostgreSQL使用了更复杂的缓冲区替换算法。

###2. 官方的PostgreSQl安装说明

将下载的PostgreSQL解压后会得到一个名为postgresql-7.4.30（以这个版本为例）的目录，在目录里面，有安装说明文件`INSTALL`，这个文件说明了PostgreSQL安装步骤，文件中关键的几行代码如下：

	./configure  	#配置，生成makefile文件
	gmake 			#make
	su 				#切换到root用户
	gmake install   #安装，以上几步是linux 下安装软件的典型方式
	adduser postgres#添加一个用户，添加完用户以后需要通过passwd postgres来修改该用户的密码
	mkdir /usr/local/pgsql/data #新建一个目录，以后数据库的所有数据和操作都在该目录下
	chown postgres /usr/local/pgsql/data #更改目录的所有者为 postgres
	su - postgres   #切换用户为postgres
	/usr/local/pgsql/bin/initdb -D /usr/local/pgsql/data #初始化工作区
	/usr/local/pgsql/bin/postmaster -D /usr/local/pgsql/data >logfile 2>&1 &
	/usr/local/pgsql/bin/createdb test #新建一个名为test 的数据库
	/usr/local/pgsql/bin/psql test  #打开客户端，连接到数据库服务器端


详细说明如下：

首先，执行configure生成makefile文件，在这一步中，通过configure的`--prefix`选项执行PostgreSQL的安装路径，默认情况下，安装到/usr/local/pgsql目录。执行完configure以后，就执行make和make install。

安装完PostgreSQL以后，新建一个用户名为postgres的用户，新建数据文件存放的目录/usr/local/pgsql/data，修改数据文件目录的所有者为postgres。切换到postgres用户，执行/usr/local/pgsql/bin/initdb初始化空库。初始化空库以后就可以启动PostgreSQL服务器端了，在PostgreSQL-7.4.30版中，通过postmaster启动数据库服务器端，在PostgreSQL-9.3.3中，
通过postgres启动数据库服务器端。

启动数据库以后，就可以通过`createdb`创建数据库，可以通过`psql`命令登陆数据库服务器了。

以上就是源码编译PostgreSQL的详细过程，但是，上面的有些操作不是必须的，有些操作在非默认安装的时候，是不能执行的，下面就介绍如何将PostgreSQL安装到其他目录。

###3 PostgreSQL的安装过程

安装PostgreSQL

	./configure --prefix=想要安装的路径 --without-readline --without-zlib
	make
	sudo make install

我们通过第一条语句指定数据库的安装路径，如果不指定，则默认安装到 /usr/loca/pgsql目录下，这是数据库的安装路径，跟具体的数据（表）存放没有任何关系。

执行完上面3条语句以后，数据库就安装完成了，但是还有很多工作要做，首先，修改/usr/local/pgsql/share目录下的conversion_create
文件，将所有的 "$libdir" 替换成 "安装目录/lib"。

替换完成以后，初始化工作区：

	mkdir /usr/local/pgsql/data
	chown userName /usr/local/pgsql/data
	/usr/local/pgsql/bin/initdb -D /usr/local/pgsql/data #初始化工作区

在这里，工作区指定为/usr/local/pgsql/data目录，实际使用当中，可以指定到任何目录，因为数据文件比较重要，所以，最好指定一个安全盘，比如，配置了RAID的磁盘阵列。

PostgreSQL数据库的配置文件是postgresql.conf，为了支持BenchmarkSQL这类性能测试工具，还需要修改配置文件：

>进入存储空间所在文件夹，本例中为`/usr/local/pgsql/data`，打开postgresql.conf文件，在文件末尾增加两行：
>
>   listen_addresses = '*'
>
>   port = 5432

上面步骤完成以后，配置工作就完成了，下面只需要启动数据库服务，然后新建数据库就可以了。

	#启动服务
	/usr/local/pgsql/bin/pg_ctl -D /usr/local/pgsql/data -l logfile start

	#创建一个名为test 的数据库
	/usr/local/pgsql/bin/createdb test

	# 使用pgsql连接到数据库服务器
	/usr/local/pgsql/bin/pgsql test


### 4. PostgreSQL的控制台命令

虽然各数据库都支持标准的SQL语法，但是，各数据库的控制台命令却是完全不一样的。在MySQL数据库下，可以使用show database,use databaseName,show tables, show create table tableName等命令查看或更改数据库与表。在PostgreSQL数据库中，也有相应的命令，如下所示：

* \h：查看SQL命令的解释，比如\h select。
* \?：查看psql命令列表。
* \l：列出所有数据库。
* \c [database_name]：连接其他数据库。
* \d：列出当前数据库的所有表格。
* \d [table_name]：列出某一张表格的结构。
* \du：列出所有用户。
* \e：打开文本编辑器。
* \conninfo：列出当前数据库和连接的信息。

### 5. 自动化脚本

下面是一个自动化配置的脚本，在PostgreSQL源码目录下执行这个脚本，就能够自动编译、安装、初始化空库、创建数据库。

	#!/bin/bash
	#
	#name    : createPostgreSQL
	#utility : 安装PostgreSQL,创建一个名为test的数据库，并进行相应设置，以方便BenchmarkSQL测试数据库性能
	#author  : Mingxing LAI
	#email   : me@mingxinglai.com
	#

	path="/home/lalor/data/pgsql"
	workspace="/home/lalor/data/data"
	DatabaseName="test"

	#首先执行相关配置
	./configure  --prefix=$path --without-readline --without-zlib

	sudo make  && sudo make install

	#将conversion_create 文件中所有的 $libdir 替换为安装目录 /lib
	sudo sed -i "s#\$libdir#$path/lib#g" $path/share/conversion_create.sql

	#创建工作区，以后所有的数据都存放在该目录下
	sudo mkdir $workspace

	#改变该目录的所有者为自己
	sudo chown `whoami` $workspace

	#初始化工作区
	$path/bin/initdb -D $workspace

	#修改工作区的配置文件
	# for PostgreSQL v7
	#echo -e "tcpip_socket = true \nport = 5432" >> $workspace/postgresql.conf
	echo -e "listen_addresses = '*' \nport = 5432" >> $workspace/postgresql.conf


	#启动服务
	$path/bin/pg_ctl -D $workspace -l logfile start

	#需要等待一会再创建数据库，因为启动服务需要时间，太快了会创建失败
	#如果数据库创建失败，可以在之后手动创建数据库
	sleep 30

	#创建数据库
	$path/bin/createdb $DatabaseName

	$path/bin/psql $DatabaseName

正常情况下，执行这个脚本就启动了数据库服务器，并且打开了客户端（pgsql），连接上了服务器。如果没有连上，可以通过如下命令查看服务器是否启动：

	ps aux | grep postgre

如果服务器没有启动，就看一下logfile里的提示内容。

操作完成以后，通过下面的命令关闭数据库服务器：

	/usr/local/pgsql/bin/pg_ctl -D /usr/local/pgsql/data stop

完。
