---
layout: post
title: "MySQL的filesort"
description: ""
category: 数据库
tags: [MySQL]
---


# 利用索引进行排序

1. 如果获取的列能够从索引中全部得到，则可以通过索引进行排序（最左前缀匹配），否则，还需要filesort

        mysql> show create table userinfo\G
        *************************** 1. row ***************************
               Table: userinfo
        Create Table: CREATE TABLE `userinfo` (
          `uid` int(11) NOT NULL,
          `name` varchar(64) NOT NULL,
          `address` varchar(128) DEFAULT NULL,
          `create_time` datetime DEFAULT NULL,
          `email` varchar(128) DEFAULT NULL,
          PRIMARY KEY (`uid`),
          KEY `idx_uni` (`create_time`,`name`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
        1 row in set (0.00 sec)
        
        mysql> explain select uid, name, create_time from userinfo where create_time = '2015-01-01' order by name desc\G
        *************************** 1. row ***************************
                   id: 1
          select_type: SIMPLE
                table: userinfo
                 type: ref
        possible_keys: idx_uni
                  key: idx_uni
              key_len: 6
                  ref: const
                 rows: 384
                Extra: Using where; Using index
        1 row in set (0.00 sec)
                
        mysql> explain select * from userinfo where create_time = '2015-01-01' order by name desc\G
        *************************** 1. row ***************************
                   id: 1
          select_type: SIMPLE
                table: userinfo
                 type: ref
        possible_keys: idx_uni
                  key: idx_uni
              key_len: 6
                  ref: const
                 rows: 384
                Extra: Using where
        1 row in set (0.01 sec)


    可以看到，第一个查询中的extra列，存在using index，也就是说，可以通过索引来避免进行数据的排序。

2. 想要利用索引的已有顺序来满足order by的请求，order by各个字段必须包含在同一个索引中，并且，各个字段的排序规则应该完整一致

        mysql> explain select name from userinfo  order by create_time asc, name desc\G
        *************************** 1. row ***************************
                   id: 1
          select_type: SIMPLE
                table: userinfo
                 type: index
        possible_keys: NULL
                  key: idx_uni
              key_len: 264
                  ref: NULL
                 rows: 512
                Extra: Using index; Using filesort
        1 row in set (0.00 sec)

    可以看到，由于我们期望对create_time进行升序排序，对desc进行降序排序，索引无法满足，导致我们依然需要使用filesort进行排序。


# filesort算法

当排序的结果集较小时，直接在排序缓冲区中进行排序，然后返回给客户端；当排序缓冲区较大时，则读取部分匹配的行到排序缓冲区，然后进行快速排序，将排好的结果存入临时文件，重复该过程，最后对临时文件中的数据进行归并排序，得到最终结果。

这里涉及两个参数：
* sort_buffer_size 排序缓冲的大小，需要记住，这个缓冲区是线程级别的，所以也不能设置得太大
* max_length_for_data 当查询的列的长度总和大于该值时，只取出需要排序的列进行排序，然后再读取其他列；当查询的列的长度总和小于该值时，取出所有列进行排序，减少一次回表操作。所以告诫我们，select的时候，不要获取我们不需要的列，影响性能。

# 优化排序算法

众所周知，从一个很大的集合里面，找出最大的k个元素或最小的k个元素，最适合的算法就是使用大根堆或小根堆。

如果order by后面有limit语句，则MySQL会通过check_if_pq_applicable选择是否使用堆排序。函数check_if_pq_applicable可读性很差，简单来说，需要满足一下几个条件：

1. limit中的记录(n+m)小于匹配记录数的1/3，因为测试发现，堆排序比快排慢三倍，如果要获取的就大于所有记录的1/3，还不如直接使用快排
2. 当排序缓冲区不能容纳m+n行数据时，使用快排
