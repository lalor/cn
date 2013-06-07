---
layout: post
title: "Python数据库访问"
description: ""
category: 程序语言
tags: [python, database, mysql, sqlite]
---


本文介绍python操作数据库的方法，并以mysql与sqlite数据库为例，从一个csv文件中读入数据，插入到数据库中，再将数据库中的数据读出，保存到另一个csv文件。

#介绍

python 社区制定了操作数据库的标准，所以，我们可以通过统一的接口访问不同的数据库，减少了我们的学习负担。

标准主要定义了两个对象，一个用于管理Connection 对象，另一个是用于执行查询的Cursor 对象。

#Python 操作数据库的步骤

python 操作数据库的思路如下：

0. 导入相应的数据库模块(import sqlite3, MySQLdb)
1. 连接数据库（connect)，返回一个Connection 对象
2. 通过该对象的cursor()成员函数返回一个Cursor 对象
3. 通过Cursor对象的execute()方法执行SQL语句
4. 如果执行的是查询语句，则通过Cursor 对象的fetchone()等语句获取返回结果
5. 关闭Cursor对象(close())
6. 关闭Connection对象(close())


#插入数据

对于sqlite 数据库，python 自带了sqlite3 模块，直接导入即可，对于mysql
数据库，则需要安装第三方模块Mysql-python 。安装完以后，在程序中导入模块即可。

	#!/usr/bin/python
	#-*- coding: UTF-8 -*-
	
	import csv
	
	def main():
		DATABASE="sqlite3"
		
		#open databases
		if	DATABASE == "sqlite3":
			import sqlite3 as db
			conn = db.connect("mydb1")
			strInsert = "insert into stocks values(?, ?, ?)"
		else:
			import MySQLdb as db
			#conn = db.connect(host="localhost", user="root", passwd="your password", db="students")
			conn = db.connect(host="localhost", db="students", read_default_file="~/.my.cnf")
			strInsert = "insert into stocks values(%s, %s, %s)"
		
		#get cursor object
		cur = conn.cursor()
	
		#read CSV file
		f = open("stock_data")
		stocks = []
		for r in csv.reader(f):
			stocks.append(r)
		
		create databses
		cur.execute("create table stocks( symbol text, shares integer, price real)")
		conn.commit()
		
		#execute statements of insert
		cur.executemany(strInsert, stocks)
		conn.commit()
		
		#close connection
		cur.close()
		conn.close()
	
	
	if __name__ == '__main__':
		main()

mysql数据库有两种连接方式（可能有很多种），一种是通过指定用户名和密码的方式，还有一种是指定read_default_file
参数，关于`.my.cnf`文件，可以参考[这里][1]。


#读取数据

下面执行查询语句，并将结果输出到一个CSV文件。

	#!/usr/bin/python
	#-*- coding: UTF-8 -*-
	import csv
	
	def main():
		DATABASE="mysql"
		
		#open databases
		if	DATABASE == "sqlite3":
			import sqlite3 as db
			conn = db.connect("mydb")
		else:
			import MySQLdb as db
			#conn = db.connect(host="localhost", user="root", passwd="your password", db="students")
			conn = db.connect(host="localhost", db="students", read_default_file="~/.my.cnf")
		
		#crate cursor object
		cur = conn.cursor()
		
		f = open("output", 'w')
		w = csv.writer(f)
		
		#read data
		cur.execute("select * from stocks")
	
		#write data to csv file
		while True:
			row = cur.fetchone()
			if	not row: break
			w.writerow(row)
		
		f.close()
		cur.close()
		conn.close()
	
	
	if __name__ == '__main__':
		main()

#ubuntu安装MySQL-Python出现mysql_config not found错误

在ubuntu 下安装Mysql-Python时，可能出现"mysql_config not found"错误。提示：

	EnvironmentError: mysql_config not found

Google后得知mysql_config是属于MySQL开发用的文件，而使用apt-get安装的MySQL是没有这个文件的，于是在包安装器里面寻找

	sudo apt-get install libmysqld-dev
	sudo apt-get install libmysqlclient-dev

这两个包安装后问题即可解决。

[1]: http://mingxinglai.com/cn/2013/01/use-mysql-in-shell/
