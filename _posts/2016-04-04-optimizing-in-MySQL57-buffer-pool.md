---
layout: post
title: "MySQL 5.7对buffer pool的改进"
description: ""
category: 数据库
tags: [MySQL]
---

本文将介绍MySQL 5.7对innodb buffer pool的改进。

## 1. 整体介绍

buffer pool是数据库的重要组件，用以缓存磁盘中的数据页，降低事务执行过程中的io代价，提高事务的执行速度。buffer pool能够显著影响数据库的整体性能，因此，MySQL的每个版本都对buffer pool进行了一些改进。

MySQL 5.7对buffer pool的改进包括三个方面：1）buffer pool的dump和load；2）在线修改buffer pool；3）优化buffer pool的刷盘操作。

## 2. buffer pool的dump和load

buffer pool的dump是指，将缓冲区中的数据页的页号dump到外存中（datadir目录下的 ib_buffer_pool文件），buffer pool的load是指，将ib_buffer_pool文件中保存的页号对应的数据页读入内存。该功能适用于数据库重启的场景，能够实现buffer pool快速预热，避免数据库重启以后，由于缓冲区失效，导致性能显著下降。

buffer pool的dump和load在MySQL 5.6中就已经有了，MySQL 5.7的改进主要有两点：

1. 将默认开启buffer pool的dump和load

        mysql> show variables like 'innodb_buffer_pool%at%';
        +-------------------------------------+-------+
        | Variable_name                       | Value |
        +-------------------------------------+-------+
        | innodb_buffer_pool_dump_at_shutdown | ON    |
        | innodb_buffer_pool_load_at_startup  | ON    |
        +-------------------------------------+-------+
        2 rows in set (0.00 sec)

2. 可以通过参数`innodb_buffer_pool_dump_pct`指定dump缓冲区中的多少页（only the hottest N% of the pages from each buffer pool)，默认dump缓冲区中最热的25%的数据页。(这一点我很奇怪，欢迎大家讨论，innodb将LRU链表分为冷区和热区，其中，热区占5/8，冷区占3/8，那为什么默认dump缓冲区中最热的25%，而不是5/8)

## 3. 在线修改buffer pool

### 3.1 innodb_buffer_poool_chunk_size
为了支持online buffer pool resize，innodb引入了一个新的参数，即innodb_buffer_pool_chunk_size，默认是128M，用以表示内存一个chunk，当我们在线修改buffer pool的时候，以chunk为单位进行增长或收缩。

这个参数的引入，对innodb_buffer_pool_size的配置有了一定的影响，innodb要求innodb_buffer_pool_size是innodb_buffer_pool_chunk_size* innodb_buffer_pool_instances的倍数，如果不是，将会适当调大innodb_buffer_pool_size，以满足要求。

### 3.2 online innodb buffer pool resize

MySQL 5.7.5 开始，可以在线修改buffer pool，如下所示：

    SET GLOBAL innodb_buffer_pool_size=1073741824;

修改buffer pool是由后台线程完成，并且，需要等待所有活跃事务完成以后，才能开始。在修改innodb buffer pool的过程中，buffer pool中的数据页也不能访问（除收缩buffer pool时，被抛弃的那部分页）。

可以通过下面的语句查看进度：

    mysql> SHOW STATUS WHERE Variable_name='InnoDB_buffer_pool_resize_status';
    +----------------------------------+----------------------------------+
    | Variable_name                    | Value                            |
    +----------------------------------+----------------------------------+
    | Innodb_buffer_pool_resize_status | Resizing also other hash tables. |
    +----------------------------------+----------------------------------+

与此同时，还会在错误日志中打印信息，如下所示：

    2016-04-04T07:48:56.486037Z 40 [Note] InnoDB: Requested to resize buffer pool. (new size: 1073741824 bytes)
    2016-04-04T07:48:56.487117Z 0 [Note] InnoDB: Resizing buffer pool from 134217728 to 1073741824 (unit=16777216).
    2016-04-04T07:48:56.487147Z 0 [Note] InnoDB: Disabling adaptive hash index.
    2016-04-04T07:48:56.487760Z 0 [Note] InnoDB: disabled adaptive hash index.
    2016-04-04T07:48:56.487771Z 0 [Note] InnoDB: Withdrawing blocks to be shrunken.
    2016-04-04T07:48:56.487777Z 0 [Note] InnoDB: Latching whole of buffer pool.
    2016-04-04T07:48:56.487792Z 0 [Note] InnoDB: buffer pool 0 : resizing with chunks 8 to 64.
    2016-04-04T07:48:56.556752Z 0 [Note] InnoDB: buffer pool 0 : 56 chunks (57343 blocks) were added.
    2016-04-04T07:48:56.556791Z 0 [Note] InnoDB: Resizing hash tables.
    2016-04-04T07:48:56.559169Z 0 [Note] InnoDB: buffer pool 0 : hash tables were resized.
    2016-04-04T07:48:56.559203Z 0 [Note] InnoDB: Resizing also other hash tables.
    2016-04-04T07:48:56.580180Z 0 [Note] InnoDB: Resized hash tables at lock_sys, adaptive hash index, dictionary.
    2016-04-04T07:48:56.580207Z 0 [Note] InnoDB: Completed to resize buffer pool from 134217728 to 1073741824.
    2016-04-04T07:48:56.580215Z 0 [Note] InnoDB: Re-enabled adaptive hash index.
    2016-04-04T07:48:56.580236Z 0 [Note] InnoDB: Completed resizing buffer pool at 160404 15:48:56.

### 3.3 内部实现

错误日志中详细的描述了resize buffer pool的具体步骤：

1. 计算新的innodb_pool_size，每个buffer_pool_instance包含几个页
2. 关闭自适应哈希索引
3. 如果是收缩buffer pool，开始回收blocks
4. 获取所有buffer pool instance的buffer pool mutex和hash table mutex，buffer pool不可用
5. 如果是收缩buffer pool，则释放多余的chunk，如果是增长buffer pool，则分配chunk
6. resize哈希表
7. 释放所有mutex，buffer pool可用
7. 打开自适应哈希索引

在我的测试环境中，将buffer pool从128M修改为1G，微妙时间内完成，比修改配置文件，再重启数据库速度快很多。不过，从上面的步骤中可以知道，在resize innodb buffer pool期间，buffer pool是加锁的，也就是说，在此期间，无法进行任何操作。因此，在线修改buffer pool也需要谨慎。


# 4. 优化buffer pool的刷盘操作

MySQL 5.7对buffer pool的刷新改进有以下三点：

1. 可以通过参数innodb_page_cleaners修改多个page clean thread线程数，最多不能操作innodb_buffer_pool_instance
2. 通过使用Hazard Pointers，减少了扫描buffer pool的代价，Hazard Pointer可以查看《[锁无关的数据结构与Hazard指针——操纵有限的资源][1]》
3. 优化了刷盘策略，详细信息可以看这里[WL7868][8]。


[1]: http://blog.csdn.net/pongba/article/details/589864
[8]: http://dev.mysql.com/worklog/task/?id=7868
