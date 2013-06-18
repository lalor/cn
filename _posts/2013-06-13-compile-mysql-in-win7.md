---
layout: post
title: "windows 7 下源码编译mysql"
description: ""
category: 工具
tags: [mysql, 源码编译]
---

# 1. mysql安装文件介绍

windows下有三种不同的方法安装mysql数据库，分别是傻瓜式图形安装、绿色版mysql以及源码编译mysql，本文主要介绍在windows7下，使用vs2008源码编译mysql。
三种不同的安装方法对应的文件名：

1. mysql-5.5.31-win32.msi，windows 安装包，图形化的傻瓜式安装。 
2. mysql-5.5.31.zip（源码安装需要下载该文件），这个是windows源文件，需要编译，对应的Linux源文件是mysql-5.5.20.tar.gz 
3. mysql-5.5.31-win32.zip，这个文件解包后即可使用，即所谓的绿色版mysql。 

# 2. 准备工作

源码编译安装mysql主要参考个人博客（MysqlOPS数据库与运维自动化技术分享）的文章《[Win7下编译mysql5.5][1]》，略有修改。

* 源码编译mysql的环境 :   

Window 7 32位、vs2008、mysql-5.5.31、CMake 2.8、bison-2.4.1

* 相关软件下载路径：  

1. http://www.mysql.com/downloads/mysql/  中mysql-5.5.31.zip
1. http://www.cmake.org/files/v2.8/cmake-2.8.6-win32-x86.exe
1. http://en.sourceforge.jp/projects/sfnet_gnuwin32/downloads/bison/2.4.1/bison-2.4.1-setup.exe/

* 安装辅助软件：

安装Cmake2.8和bison-2.4.1。安装Cmake和bison时需要注意的是安装路径尽量不使用空格(例如 C:\Program Files)，可以直接安装到C盘根目录（C:\Cmake和C:\GnuWin32），然后将C:\CMake 2.8\bin和C:\GnuWin32\bin添加到环境变量。安装VS2008，这里不做详细描述。

# 3. 源码编译mysql

1. 解压mysql-5.5.31.zip到D:\mysql，解压以后路径为D:\mysql\mysql-5.5.31
2. cd  D:\mysql\mysql-5.5.31
3. 执行cmake . -G “Visual Studio 9 2008″,如下图所示
<a href="http://imgur.com/18SZo2I"><img src="http://i.imgur.com/18SZo2I.png" title="Hosted by imgur.com"/></a>
4. 打开D:\mysql\mysql-5.5.31\sql\mysqld.cc，注释掉test_lc_time_sz()函数的函数体
5. 用记事本打开D:\mysql\mysql-5.5.31\sql\sql_locale.cc，并另存为utf8格式
6. 在D:\mysql\mysql-5.5.31自动生成mysql.sln，使用vs2008打开MySQL.sln
7. 编译整个工程，这个时候没有报错，那么mysql就编译完成了，但是还不能直接运行。
8. 编译完成以后选择Solution --> 右键 --> 属性 --> 在Startup Project选项卡的Single startup project中选择mysqld，如图所示：
<a href="http://imgur.com/iYkDSk9"><img src="http://i.imgur.com/iYkDSk9.png" title="Hosted by imgur.com"/></a>

这个时候运行mysqld工程，我们就启动了服mysql服务器进程，但是，此时必然会启动失败，因为服务器进程需要连接到特定的数据库，而我们现在只有服务器进程，没有数据库，所以，连接失败了。服务器进程可以连接到一个已经存在的数据库（创建数据库的mysql版本需要与我们编译的mysql版本一致），这个数据库可以是通过其他安装方法安装的mysql来初始化的数据库，或者手动初始化的数据库。下面将介绍手动初始化数据库的方法。

# 4. 手动初始化数据库

1.  在D:\mysql目录下创建mydata10（D:\mysql\mydata10）目录，并在mydata10下创建english（D:\mysql\mydata10\english）目录和var（D:\mysql\mydata10\var）目录。
2.  将errmsg.sys（D:\mysql\mysql-5.5.31\sql\share\english\errmsg.sys）拷贝到english（D:\mysql\mydata10\english）目录。
3. 编辑以下三个文件，在文件的首部加入一条语句（use mysql）

	D:\mysql\mysql-5.5.31\scripts\mysql_system_tables.sql
	D:\mysql\mysql-5.5.31\scripts\mysql_system_tables_data.sql
	D:\mysql\mysql-5.5.31\scripts\fill_help_tables.sql

4. 新建fill_data.bat文件，文件内容为：

	   D:\mysql\mysql-5.5.31\sql\Debug\mysqld.exe --bootstrap --console < D:\mysql\mysql-5.5.31\scripts\mysql_system_tables.sql
	   D:\mysql\mysql-5.5.31\sql\Debug\mysqld.exe --bootstrap --console < D:\mysql\mysql-5.5.31\scripts\mysql_system_tables_data.sql
	   D:\mysql\mysql-5.5.31\sql\Debug\mysqld.exe --bootstrap --console < D:\mysql\mysql-5.5.31\scripts\fill_help_tables.sql

5. 在D:\mysql\mysql-5.5.31\sql目录下新建my.ini文件，并写入如下信息：

		[client]
		port            = 3306
		socket          = D:\mysql\mydata10\var\mysqld.sock
		
		[mysqld_safe]
		user            = mysql
		nice            = 0
		
		[mysqld]
		server-id               = 1
		bind-address            = 127.0.0.1
		port                    = 3306
		pid-file                = D:\mysql\mydata10\var\mysqld.pid
		socket                  = D:\mysql\mydata10\var\mysqld.sock
		basedir                 = D:\mysql\mydata10
		datadir                 = D:\mysql\mydata10\var
		log-error               = D:\mysql\mydata10\var\mysqld.log
		slow_query_log			= 1
		slow_query_log_file     = D:\mysql\mydata10\var\mysql-slow.log
		log_bin                 = D:\mysql\mydata10\var\mysql-bin.log
		binlog_format           = MIXED
		
		user                    = mysql
		lc-messages-dir         = D:\mysql\mydata10\english\
		table_cache             = 512
		long_query_time         = 4
		max_connections         = 100
		query_cache_type        = 0
		character-set-server    = gbk
		skip-external-locking
		expire_logs_days        = 7
		max_binlog_size         = 100M
		max_allowed_packet      = 16M
		skip-name-resolve
		skip-grant-tables
		
		[mysqldump]
		quick
		quote-names
		max_allowed_packet      = 16M
		
		[mysql]
		default-character-set	= gbk

6.  双击fill_data.bat运行文件中的三条语句，运行完这三条语句以后，数据库就初始化完成了，回到vs2008，运行mysqld，启动服务器
7.  双击client.exe（D:\mysql\mysql-5.5.31\client\Debug）运行客户端，这个时候就可以对数据库进行操作了，所有操作的结果都会保存在D:\mysql\mydata10\var目录下。

mysql的调试就不再介绍了，和其他程序的调试没有区别，只需要调试启动mysqld，并在关键函数处设置断点，然后启动mysql.exe，敲入一些sql语句，当执行到端口处，程序就会停住。

[1]: http://www.mysqlops.com/2011/12/28/win-compile-mysql55.html
