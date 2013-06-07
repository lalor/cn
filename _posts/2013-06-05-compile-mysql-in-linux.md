---
layout: post
title: "linux下源码编译mysql"
description: ""
category: 工具
tags: [mysql, 源码编译]
---

本文介绍了在Ubuntu 12.04LTS 编译mysql 源码的详细步骤。

##1. 安装make 编译器（一般linux自带已经有了）

下载地址：http://www.gun.org/software/make/

	tar zxvf make-3.82.tar.gz
	cd make-3.82
	./configure
	make
	make install


##2. 安装bison

下载地址：http://www.gun.org/software/bison/

	tar zxvf bison-2.7.tar.gz
	cd bison-3.7
	./configure
	make
	make install


##3. 安装cmake

下载地址：http://www.cmake.org

	tar zxvf cmake-2.8.4.tar.gz
	cd cmake-2.8.4
	./configure
	make
	make install

##4. 安装mysql 源码需要的库

	sudo apt-get install build-essential libncurses5-dev 

##5. 开始安装MySQL
	
	下载地址：http://dev.mysql.com/, 解压并安装MySQL

	tar zxvf mysql-5.5.13.tar.gz  
	cd mysql-5.5.13  
	cmake . \
	-DCMAKE_INSTALL_PREFIX=/usr/local/mysql \
	-DINSTALL_DATADIR=/usr/local/mysql/data  
	make  
	sudo make install  

注意，执行cmake 命令时，反斜杠后面不能有空格。

##6. 完成后，继续下面的操作初始化数据库

	cd /usr/local
	sudo chown -R `whoami` mysql
	sudo chgrp -R `whoami` mysql
	cd /usr/local/mysql
	./scripts/mysql_install_db --user=`whoami` --no-defaults

切记后面有个`--no-defaults`选项，如果没有该选项，则程序会自动载入默认的配置文档，而目前你还没有完成配置文件的编写，因此，很可能载入的是错误的信息。如果该指令能够运行成功，那么恭喜你，你的MySQL 可以成功启动了。

##7. 修改配置文件

	cp support-files/my-default.cnf /etc/my.cnf 
	vim /etc/my.cnf

关于mysql 的配置文件的前后顺序，可以参考[这里](http://weibo.com/2216172320/zxUaAaiuu)。

打开my.cnf 文件，加入下面两条语句

	[client]
	sock = /tmp/mysqld.sock

	[mysqld]
	user=username
	basedir = /usr/local/mysql
	datadir = /usr/local/mysql/data
	sock = /tmp/mysqld.sock

##8. 新建mysqld.sock 文件

	touch /tmp/mysqld.sock
	sudo chown -R `whoami` /tmp/mysqld.sock
	sudo chgrp -R `whoami` /tmp/mysqld.sock

##9. 运行服务进程：

	/usr/local/mysql/bin/mysqld 

如果ctrl+c 不能关闭服务进程，请使用ctrl+\

运行客户端进程：

	/usr/local/mysql/bin/mysql -u root -p   

初始时，root密码为空，直接回车即可。也可以像下面这样：

	/usr/local/mysql/bin/mysql -u root 

我们只要指定myqld和mysql的路径就可以启动mysql了，不过，还有更简单的办法。

##10.启动mysqld

	cd /usr/local/mysql
	sudo cp support-files/mysql.server /etc/init.d/mysqld 

现在我们就可以通过下面的命令启动、查看、关闭mysql 服务了。

	/etc/init.d/mysqld start
	/etc/init.d/mysqld status
	/etc/init.d/mysqld stop

如果你是一个linux 老手，可能你更喜欢像下面这样启动服务。

	service mysqld start
	service mysqld status
	service mysqld stop

为了避免每次启动mysql客户端都输入一串常常的路径，你可以将mysql
程序拷贝的`/usr/local/bin`目录下，也可以向下面这样，将mysql程序的路径添加到环境变量中。

	PATH="$PATH:/usr/local/mysql/bin"
	export PATH

##常见问题1：启动不了?

是否已经启动了一个进程

	ps aux | grep "mysql"

##常见问题2：提示 error: 'Can't connect to local MySQL server through socket '/var/run/mysqld/mysqld.sock' (2)'

一般这种问题都是权限问题。不但要设置对应目录的所有者权限，而且还要设置组的权限。

例如，上面的提示，那么我们需要修改对应目录的权限，以及my.cnf中socket 的值。

	sudo chown -R lalor /var/run/mysqld
	sudo chgrp -R lalor /var/run/mysqld
	sudo vim /etc/my.cnf
	socket = /var/run/mysqld/mysqld.sock 


完。
