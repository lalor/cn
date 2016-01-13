---
layout: post
title: "MySQL查询计划key_len全知道"
description: ""
category: 数据库
tags: [MySQL, 数据库]
---

本文首先介绍了MySQL的查询计划中ken_len的含义；然后介绍了key_len的计算方法；最后通过一个伪造的例子，来说明如何通过key_len来查看联合索引有多少列被使用。

### key_len的含义

在MySQL中，可以通过explain查看SQL语句所走的路径，如下所示：

    mysql> create table t(a int primary key, b int not null, c int not null, index(b));
    Query OK, 0 rows affected (0.01 sec)
    mysql> explain select b from t ;
    +----+-------------+-------+-------+---------------+------+---------+------+------+-------------+
    | id | select_type | table | type  | possible_keys | key  | key_len | ref  | rows | Extra       |
    +----+-------------+-------+-------+---------------+------+---------+------+------+-------------+
    |  1 | SIMPLE      | t     | index | NULL          | b    | 4       | NULL |    1 | Using index |
    +----+-------------+-------+-------+---------------+------+---------+------+------+-------------+
    1 row in set (0.00 sec)

其中，key_len表示使用的索引长度，是以字节为单位。在上面的例子中，由于int型占用4个字节，而索引中只包含了1列，所以，key_len是4。

下面是联合索引的情况：

    mysql> alter table t add index ix(b, c);
    Query OK, 0 rows affected (0.03 sec)
    Records: 0  Duplicates: 0  Warnings: 0
    mysql> explain select b, c from t ;
    +----+-------------+-------+-------+---------------+------+---------+------+------+-------------+
    | id | select_type | table | type  | possible_keys | key  | key_len | ref  | rows | Extra       |
    +----+-------------+-------+-------+---------------+------+---------+------+------+-------------+
    |  1 | SIMPLE      | t     | index | NULL          | ix   | 8       | NULL |    1 | Using index |
    +----+-------------+-------+-------+---------------+------+---------+------+------+-------------+
    1 row in set (0.00 sec)

联合索引ix包含了2列，并且都使用到了，所以，这里ken_len是8。

到这里，我们已经可以理解key_len的含义了，似乎已经没有什么可讲的了，但是，MySQL中key_len的计算还有很多需要注意的地方。

例如，我们将b这一列的NOT NULL约束去掉，然后ken_len就和我们预期不一样了，如下所示：

    mysql> alter table t modify b int;
    Query OK, 0 rows affected (0.01 sec)
    Records: 0  Duplicates: 0  Warnings: 0
    
    mysql> explain select b from t;
    +----+-------------+-------+-------+---------------+------+---------+------+------+-------------+
    | id | select_type | table | type  | possible_keys | key  | key_len | ref  | rows | Extra       |
    +----+-------------+-------+-------+---------------+------+---------+------+------+-------------+
    |  1 | SIMPLE      | t     | index | NULL          | b    | 5       | NULL |    1 | Using index |
    +----+-------------+-------+-------+---------------+------+---------+------+------+-------------+
    1 row in set (0.00 sec)

###MySQL中key_len计算规则

MySQL中，key_len的计算规则如下：

1. *如果列可以为空，则在数据类型占用字节的基础上加1*，如int型，不能为空key_len为4，可以为空key_len为5
1. *如果列是变长的，则在数据列所占字节的基数上再加2*，如varbinary(10)，不能为空，则key_len为10  + 2 ，可以为空则key_len为10+2+1
1. *如果是字符型，则还需要考虑字符集*，如某列的定义是varchar(10)，且是utf8，不能为空，则key_len为10 * 3 + 2，可以为空则key_len为10*3+2+1

此外，decimal列的计算方法与上面一样，如果可以为空，则在数据类型占用字节的基础上加1，但是，decimal本身所占用字节数，计算就比较复杂。

根据官方文档可以知道，decimal定义为decimal(M,D)，其中，M是总的位数，D是小数点后保留的位数。小数点前与小数点后的数字分开存储，且以9位数为1组，用4个字节保存，如果低于9位数，需要的字节数如下：

    Leftover Digits Number of Bytes
    -----------------------------
    |0              |0          |
    |1-2            |1          |
    |3-4            |2          |
    |5-6            |3          |
    |7-9            |4          |
    -----------------------------

例如：

* decimal(20，6）==>  小数点左边14位，小数点右边6位 ==> 小数点左边分组为5 + 9，需要3个字节+4个字节存储，小数点一个分组，需要3个字节存储 ==> 总共需要10个字节
* decimal(18，9）==> 小数点左边9位数，小数点右边9位数==> 分别使用4个字节存储 ==> 共需要 8个字节
* decimal(18，2）==> 小数点左边16位数，小数点右边2位数 ==> 分组为7 + 9，需要8个字节存储，小数点右边1个字节存储 ==> 共需要9个字节

### 通过key_len分析联合索引

如下所示，我们定义了一个表t，表t包含a、b、c、d共4列：

    mysql> show create table t\G
    *************************** 1. row ***************************
           Table: t
    Create Table: CREATE TABLE `t` (
      `a` int(11) NOT NULL,
      `b` int(11) DEFAULT NULL,
      `c` int(11) DEFAULT NULL,
      `d` int(11) DEFAULT NULL,
      PRIMARY KEY (`a`),
      KEY `ix_x` (`b`,`d`,`c`)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8
    1 row in set (0.00 sec)

现在要执行SQL语句如下：

    select a from t where b = 5 and d = 10 order by c;

假设我们有一个索引ix_x(b，d，c），通过explain得到如下输出：

    mysql> explain select a from t where b = 5 and d = 10 order by c;
    +----+-------------+-------+------+---------------+------+---------+-------------+------+--------------------------+
    | id | select_type | table | type | possible_keys | key  | key_len | ref         | rows | Extra                    |
    +----+-------------+-------+------+---------------+------+---------+-------------+------+--------------------------+
    |  1 | SIMPLE      | t     | ref  | ix_x          | ix_x | 10      | const,const |    1 | Using where; Using index |
    +----+-------------+-------+------+---------------+------+---------+-------------+------+--------------------------+
    1 row in set (0.00 sec)

可以看到，查询语句使用了联合索引中的b和d两列来过滤数据。

如果我们定义的联合索引不是ix_x(b，d，c），通过explain得到的输入如下：

    mysql> alter table t drop index ix_x;
    mysql> alter table t add index ix_x(b, c, d);
    mysql> explain select a from t where b = 5 and d = 10 order by c;
    +----+-------------+-------+------+---------------+------+---------+-------+------+--------------------------+
    | id | select_type | table | type | possible_keys | key  | key_len | ref   | rows | Extra                    |
    +----+-------------+-------+------+---------------+------+---------+-------+------+--------------------------+
    |  1 | SIMPLE      | t     | ref  | ix_x          | ix_x | 5       | const |    2 | Using where; Using index |
    +----+-------------+-------+------+---------------+------+---------+-------+------+--------------------------+
    1 row in set (0.00 sec)

key_len为5，也就是说，只用到了联合索引中的第一列，可以看到，虽然联合索引包含了我们要查询的所有列，但是，由于定义的顺序问题，SQL语句并不能够充分利用索引。
