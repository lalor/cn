---
layout: post
title: "MySQL 5.7新特性介绍"
description: ""
category: 数据库
tags: [MySQL]
---


# 1. 介绍

身处MySQL这个圈子，能够切身地感受到大家对MySQL 5.7的期待和热情，似乎每个人都迫不及待的想要了解、学习和使用MySQL 5.7。那么，我们不禁要问，MySQL 5.7到底做了哪些改进，引入了哪些新功能，性能又提升了多少，能够让大家翘首以盼，甚至欢呼雀跃呢？

![此处输入图片的描述][1]

下面就跟随我来一起了解一下MySQL 5.7的部分新功能。想要在一篇文章中介绍完MySQL 5.7的所有改进，几乎是不可能的。所以，我会选择一些有特别意思的、特别有用的功能进行介绍。希望通过这篇文章，能够激发大家对MySQL 5.7的学习兴趣，甚至能够吸引大家将自己的业务迁移到MySQL 5.7上。

MySQL 5.7在诸多方面都进行了大幅的改进，本文将从安全性（见2.1节）、灵活性（见2.2节）、易用性（见2.3节）、可用性（见2.4节）和性能（见2.5节）等几个方面进行介绍。最后，在第3节对本文进行了简单的总结。

# 2. MySQL 5.7的新特性

这一节中，将依次介绍MySQL 5.7的各种新特性。由于MySQL 5.7改进较多，因此，本文将这些新特性进行了简单的分类，分为安全性、灵活性、易用性、可用性和性能。接下来，将从各个分类依次进行介绍。


## 2.1 安全性

安全性是数据库永恒的话题，在MySQL 5.7中，有不少安全性相关的改进。包括：

* MySQL数据库初始化完成以后，会产生一个`root@localhost`用户，从MySQL 5.7开始，root用户的密码不再是空，而是随机产生一个密码，这也导致了用户安装5.7时发现的与5.6版本比较大的一个不同点
* MySQL官方已经删除了test数据库，默认安装完后是没有test数据库的，就算用户创建了test库，也可以对test库进行权限控制了
* MySQL 5.7版本提供了更为简单SSL安全访问配置，并且默认连接就采用SSL的加密方式
* 可以为用户设置密码过期策略，一定时间以后，强制用户修改密码

        ALTER USER 'jeffrey'@'localhost' PASSWORD EXPIRE INTERVAL 90 DAY;

* 可以"锁"住用户，用以暂时禁用某个用户

        ALTER USER  'jeffrey'@'localhost' ACCOUNT LOCK;
        ALTER USER l 'jeffrey'@'localhost'  ACCOUNT UNLOCK;

## 2.2 灵活性

在这一节，我将介绍MySQL 5.7的两个全新的功能，即JSON和generate column。充分使用这两个功能，能够极大地提高数据存储的灵活性。

### 2.2.1 JSON

随着非结构化数据存储需求的持续增长，各种非结构化数据存储的数据库应运而生（如MongoDB）。从最新的数据库使用[排行榜][11]来看，MongoDB已经超过了PostgreSQL，其火热程度可见一斑。

各大关系型数据库也不甘示弱，纷纷提供对JSON的支持，以应对非结构化数据库的挑战。MySQL数据库从5.7.8版本开始，也提供了对JSON的支持。其使用方式如下：

    CREATE TABLE t1 (jdoc JSON);
    INSERT INTO t1 VALUES('{"key1": "value1", "key2": "value2"}');

MySQL对支持JSON的做法是，在server层提供了一堆便于操作JSON的函数，至于存储，就是简单地将JSON编码成BLOB，然后交由存储引擎层进行处理，也就是说，MySQL 5.7的JSON支持与存储引擎没有关系，MyISAM 存储引擎也支持JSON 格式。

MySQL支持JSON以后，总是避免不了拿来与MongoDB进行一些比较。但是，MySQL对JSON的支持，至少有两点能够完胜MongoDB：

1. 可以混合存储结构化数据和非结构化数据，同时拥有关系型数据库和非关系型数据库的优点
2. 能够提供完整的事务支持

### 2.2.2 generate column

generated column是MySQL 5.7引入的新特性，所谓generated column，就是数据库中这一列由其他列计算而得。

例如，知道直角三角形的两条直角边，要求直角三角形的面积。很明显，面积可以通过两条直角边计算而得，那么，这时候就可以在数据库中只存放直角边，面积使用generated column，如下所示：

    CREATE TABLE triangle (sidea DOUBLE, sideb DOUBLE, area DOUBLE AS (sidea * sideb / 2));
    insert into triangle(sidea, sideb) values(3, 4);
    select * from triangle;
    +-------+-------+------+
    | sidea | sideb | area |
    +-------+-------+------+
    |     3 |     4 |    6 |
    +-------+-------+------+
    
在MySQL 5.7中，支持两种generated column，即virtual generated column和stored generated column，前者只将generated column保存在数据字典中（表的元数据），并不会将这一列数据持久化到磁盘上；后者会将generated column持久化到磁盘上，而不是每次读取的时候计算所得。很明显，后者存放了可以通过已有数据计算而得的数据，需要更多的磁盘空间，与virtual column相比并没有优势。因此，在不指定generated column的类型时，默认是virtual column，如下所示：

    show create table triangle\G
    *************************** 1. row ***************************
           Table: triangle
    Create Table: CREATE TABLE `triangle` (
      `sidea` double DEFAULT NULL,
      `sideb` double DEFAULT NULL,
      `area` double GENERATED ALWAYS AS (((`sidea` * `sideb`) / 2)) VIRTUAL
    ) ENGINE=InnoDB DEFAULT CHARSET=latin1
    
如果读者觉得generate column提供的功能，也可以在用户代码里面实现，并没有什么了不起的地方，那么，或许还有一个功能能够吸引挑剔的你，那就是为generate column创建索引。在这个例子中，如果我们需要根据面积创建索引以加快查询，就无法在用户代码里面实现，使用generate column就变得非常简单：

    alter table triangle add index ix_area(area);
 
## 2.3 易用性

易用性是数据库永恒的话题，MySQL也在持续不断地提高数据库的易用性。在MySQL 5.7中，有很多易用性方面的改进，小到一个客户端快捷键`ctrl+c`的使用，大到专门提供一个系统库(sys)来帮助DBA和开发人员使用数据库。这一节将重点介绍MySQL 5.7引入的sys库。

* 在linux下，我们经常使用`ctrl+c`来终止一个命令的运行，在MySQL 5.7 之前，如果用户输入了错误的SQL语句，按下`ctrl+c`，虽然能够"结束"SQL语句的运行，但是，也会退出当前会话，MySQL 5.7对这一违反直觉的地方进行了改进，不再退出会话。
* MySQL 5.7可以explain一个正在运行的SQL，这对于DBA分析运行时间较长的语句将会非常有用
* 在MySQL 5.7中，performance_schema提供了更多监控信息，包括内存使用，MDL锁，存储过程等

### 2.3.1 sys schema

sys schema是MySQL 5.7.7中引入的一个系统库，包含了一系列视图、函数和存储过程， 该项目专注于MySQL的易用性。例如，我们可以通过sys schema快速的知道，哪些语句使用了临时表，哪个用户请求了最多的io，哪个线程占用了最多的内存，哪些索引是无用索引等

sys schema中包含了大量的视图，那么，这些视图的信息来自哪里呢？视图中的信息均来自performance schema统计信息。[这里][12]有一个很好的比喻：

>For Linux users I like to compare performance_schema to /proc, and SYS to vmstat.

也就是说，performance schema提供了信息源，但是，没有很好的将这些信息组织成有用的信息，从而没有很好的发挥它们的作用。而sys schema使用performance schema信息，通过视图的方式给出解决实际问题的答案。

例如，下面这些问题，在MySQL 5.7之前，需要借助外部工具才能知道，在MySQL 5.7中，直接查询sys库下相应的表就能得到答案：

- 如何查看数据库中的冗余索引

        select * from sys.schema_redundant_indexes;
    
- 如何获取未使用的索引

        select * from schema_unused_indexes;

- 如何查看使用全表扫描的SQL语句

        select * from statements_with_full_table_scans

## 2.4 可用性

MySQL 5.7在可用性方面的改进也带给人不少惊喜。这里介绍特别有用的几项改进，包括：

* 在线设置[复制的过滤规则][13]不再需要重启MySQL，只需要停止SQL thread，修改完成以后，启动SQL thread
* 在线修改buffer pool的大小

    MySQL 5.7为了支持online buffer pool resize，引入chunk的概念，每个chunk默认是128M，当我们在线修改buffer pool的时候，以chunk为单位进行增长或收缩。这个参数的引入，对innodb_buffer_pool_size的配置有了一定的影响。innodb要求buffer pool size是innodb_buffer_pool_chunk_size* innodb_buffer_pool_instances的倍数，如果不是，将会适当调大innodb_buffer_pool_size，以满足要求，因此，可能会出现buffer pool的实际分配比配置文件中指定的size要大的情况
* Online DDL
    MySQL 5.7支持重命名索引和修改varchar的大小，这两项操作在之前的版本中，都需要重建索引或表
    
        ALTER TABLE t1 ALGORITHM=INPLACE, CHANGE COLUMN c1 c1 VARCHAR(255);

* [在线开启GTID][14]，在之前的版本中，由于不支持在线开启GTID，用户如果希望将低版本的数据库升级到支持GTID的数据库版本，需要先关闭数据库，再以GTID模式启动，所以导致升级起来特别麻烦。MySQL 5.7以后，这个问题不复存在

## 2.5 性能

性能一直都是用户最关心的问题，在MySQL每次新版本中，都会有不少性能提升。在MySQL 5.7中，性能相关的改进非常多，这里仅介绍部分改进，包括临时表相关的性能改进、只读事务的性能优化、连接建立速度的优化和复制性能的改进。

### 2.5.1 临时表的性能改进

MySQL 5.7 为了提高临时表相关的性能，对临时表相关的部分进行了大幅修改，包括引入新的临时表空间；对于临时表的DDL，不持久化相关表定义；对于临时表的DML，不写redo，关闭change buffer等。所有临时表的改动，都基于以下两个[事实][15]：

1. 临时表只在当前会话中可见
2. 临时表的生命周期是当前连接（MySQL宕机或重启，则当前连接结束）

也就是说，对于临时表的操作，不需要其他数据一样严格地进行一致性保证。通过不持久化元信息，避免写redo等方式，减少临时表操作的IO，以提高临时表操作的性能。

### 2.5.2 只读事务性能改进
    
众所周知，在传统的OLTP应用中，读操作远多于写操作，并且，读操作不会对数据库进行修改，如果是非锁定读，读操作也不需要进行加锁。因此，对只读事务进行优化，是一个不错的选择。

在MySQL 5.6中，已经对只读事务进行了许多优化。例如，将MySQL内部实现中的事务链表分为只读事务链表和普通事务链表，这样在创建ReadView的时候，需要遍历事务链表长度就会小很多。

在MySQL 5.7中，首先假设一个事务是一个只读事务，只有在该事务发起了修改操作时，才会将其转换为一个普通事务。MySQL 5.7通过[避免为只读事务分配事务ID][16]，不为只读事务分配回滚段，减少锁竞争等多种方式，优化了只读事务的开销，提高了数据库的整体性能。

### 2.5.3 [加速连接处理][17]

在MySQL 5.7之前，变量的初始化操作（THD、VIO）都是在连接接收线程里面完成的，现在将这些工作下发给工作线程，以减少连接接收线程的工作量，提高连接的处理速度。这个优化对那些频繁建立短连接的应用，将会非常有用。

### 2.5.4 [复制性能的改进][19]

MySQL的复制延迟是一直被诟病的问题之一，欣喜的是，MySQL 5.7版本已经支持"真正"的并行复制功能。MySQL 5.7并行复制的思想简单易懂，简而言之，就是"一个组提交的事务都是可以并行回放的"，因为这些事务都已进入到事务的prepare阶段，则说明事务之间没有任何冲突（否则就不可能提交）。MySQL 5.7以后，复制延迟问题永不存在。

这里需要注意的是，为了兼容MySQL 5.6基于库的并行复制，5.7引入了新的变量slave-parallel-type，该变量可以配置成DATABASE（默认）或LOGICAL_CLOCK。可以看到，MySQL的默认配置是库级别的并行复制，为了充分发挥MySQL 5.7的并行复制的功能，我们需要将slave-parallel-type配置成LOGICAL_CLOCK。

# 3. 总结

1. 从本文中可以看到，MySQL 5.7确实带来了很多激动人心的功能，我们甚至不需要进行任何修改，只需要将业务迁移到MySQL 5.7上，就能带来不少性能的提升。

2. 从本文中还可以看到，虽然MySQL 5.7在易用性上有了很多的改进，但是，也有不少需要注意的地方， 例如：1）在设置innodb的buffer pool时，需要注意chunk的存在，合理设置buffer pool instance否则可能出现实际分配的buffer pool size比预想的大很多的情况；2）多线程复制需要注意将slave_parallel_type设置为LOGICAL_CLOCK，否则，MySQL使用的是库级别的并行复制，对于大多数应用，并没有什么效果。那么，**怎样才是使用MySQL 5.7的正确姿势呢？网易蜂巢是一个不错的选择**，网易蜂巢的RDS（Relational Database Service，简称RDS）项目是一种即开即用、稳定可靠、可弹性伸缩的在线数据库服务。使用RDS提供的服务，就是使用已经调优过的数据库，用户不需要对数据库参数进行任何修改，就能够获得一个性能极好的数据库服务。

# 4. 参考资料

1. [What's New in MySQL 5.7][20]
2. [What Is New in MySQL 5.7][21]
3. [MySQL 5.7 并行复制实现原理与调优][19]

[1]: http://www.sucaijiayuan.com/uploads/allimg/130512/2-1305122234404A.jpg
[11]: http://db-engines.com/en/
[12]: http://mysqlserverteam.com/using-sys-session-as-an-alternative-to-show-processlist/
[13]: http://dev.mysql.com/doc/refman/5.7/en/change-replication-filter.html
[14]: http://mysqlhighavailability.com/enabling-gtids-without-downtime-in-mysql-5-7-6/
[15]: http://dev.mysql.com/worklog/task/?id=6470
[16]: http://dev.mysql.com/worklog/task/?id=6047
[17]: http://dev.mysql.com/worklog/task/?id=6606
[18]: http://dev.mysql.com/doc/refman/5.7/en/sorted-index-builds.html
[19]: http://www.innomysql.com/article/16317.html
[20]: http://mysqlserverteam.com/whats-new-in-mysql-5-7-generally-available/
[21]: http://dev.mysql.com/doc/refman/5.7/en/mysql-nutshell.html
