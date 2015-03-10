---
layout: post
title: "What's New in MySQL 5.6(performance_schema)"
description: ""
category: 数据库
tags: [mysql]
---

## 1.介绍

在MySQL数据库系统中，有三个系统库，分别是MySQL、informance_schema和Performance_schema，其中，informance_schema和performance_schema用以查看MySQL的运行状况、系统变量、当前连接以及资源消耗等统计信息。

MySQL Performance Schema在MySQL 5.5引入，用于查看关键性能指标。MySQL 5.6增强了Performance Schema的功能，提供了DBA和开发者常见问题的答案。包括：

* Statements/Stages

    * 哪些是资源密集型查询？时间花在哪里？

* Table/Index I/O, Table Locks

    * 哪些表和索引负载最高或者争用最多？

* Users/Hosts/Accounts

    * 哪些用户、主机与帐号最费资源？

* Network I/O

    * 网络负载如何？会话空闲时间如何？

* Summaries

    * Aggregated statistics grouped by statement, thread, user, host, account or object.


官方文档和英文文档都称里面的表为“instrumentation”，中文是在是不知道应该怎么称呼，暂且称之为“监视器”吧，就像它们的作用一样——统计MySQL的各种信息，供用户诊断问题。

在MySQL 5.5中，performance_schema共有17张表，如下所示：

    mysql> show tables;
    +----------------------------------------------+
    | Tables_in_performance_schema                 |
    +----------------------------------------------+
    | cond_instances                               |
    | events_waits_current                         |
    | events_waits_history                         |
    | events_waits_history_long                    |
    | events_waits_summary_by_instance             |
    | events_waits_summary_by_thread_by_event_name |
    | events_waits_summary_global_by_event_name    |
    | file_instances                               |
    | file_summary_by_event_name                   |
    | file_summary_by_instance                     |
    | mutex_instances                              |
    | performance_timers                           |
    | rwlock_instances                             |
    | setup_consumers                              |
    | setup_instruments                            |
    | setup_timers                                 |
    | threads                                      |
    +----------------------------------------------+
    17 rows in set (0.00 sec)

在MySQL 5.6中，共有52张表，如下所示：

    mysql> show tables;
    +----------------------------------------------------+
    | Tables_in_performance_schema                       |
    +----------------------------------------------------+
    | accounts                                           |
    | cond_instances                                     |
    | events_stages_current                              |
    | events_stages_history                              |
    | events_stages_history_long                         |
    | events_stages_summary_by_account_by_event_name     |
    | events_stages_summary_by_host_by_event_name        |
    | events_stages_summary_by_thread_by_event_name      |
    | events_stages_summary_by_user_by_event_name        |
    | events_stages_summary_global_by_event_name         |
    | events_statements_current                          |
    | events_statements_history                          |
    | events_statements_history_long                     |
    | events_statements_summary_by_account_by_event_name |
    | events_statements_summary_by_digest                |
    | events_statements_summary_by_host_by_event_name    |
    | events_statements_summary_by_thread_by_event_name  |
    | events_statements_summary_by_user_by_event_name    |
    | events_statements_summary_global_by_event_name     |
    | events_waits_current                               |
    | events_waits_history                               |
    | events_waits_history_long                          |
    | events_waits_summary_by_account_by_event_name      |
    | events_waits_summary_by_host_by_event_name         |
    | events_waits_summary_by_instance                   |
    | events_waits_summary_by_thread_by_event_name       |
    | events_waits_summary_by_user_by_event_name         |
    | events_waits_summary_global_by_event_name          |
    | file_instances                                     |
    | file_summary_by_event_name                         |
    | file_summary_by_instance                           |
    | host_cache                                         |
    | hosts                                              |
    | mutex_instances                                    |
    | objects_summary_global_by_type                     |
    | performance_timers                                 |
    | rwlock_instances                                   |
    | session_account_connect_attrs                      |
    | session_connect_attrs                              |
    | setup_actors                                       |
    | setup_consumers                                    |
    | setup_instruments                                  |
    | setup_objects                                      |
    | setup_timers                                       |
    | socket_instances                                   |
    | socket_summary_by_event_name                       |
    | socket_summary_by_instance                         |
    | table_io_waits_summary_by_index_usage              |
    | table_io_waits_summary_by_table                    |
    | table_lock_waits_summary_by_table                  |
    | threads                                            |
    | users                                              |
    +----------------------------------------------------+
    52 rows in set (0.00 sec)

正如你所看到的，MySQL 5.6比MySQL 5.5有了大幅提升，performance_schema中的instrumentation是原来的三倍！这对于熟悉MySQL的人来说，自然是好事，不管有用没用，暴露越多的内部信息，总是更加放心，说不定这些信息什么时候就能够用的着了。但是，对于新手来说，却不尽然，光看到这么多的表就头疼，更别说使用这些表来帮组自己定位问题，所以，下面对performance_schema进行了简单的分类，以帮助大家快速熟悉performance_schema提供的新功能。

## 2.分类

MySQL 5.6的Performance_schema里面有52张表，想记住每张表的作用并不是一件容易的事情，但是，如果把它们按照功能进行区分，就会清楚直观得多。

Performance_schema中的表，按功能大致可以分为以下几类：

* Basic low-level instrumentation

    - Mutex waits
    - Condition waits
    - R/W lock waits
    - File I/O events

* Summary tables
* Table and index I/O
* Table locks
* Network I/O
* Statements
* Stages
* User, account, host
* Host cache

按照官方文档的说法，MySQL 5.6不但在功能上对Performance_schema进行了大幅提升，在性能上和可用性上也进行了较大的改进：

1. 减少了Performance_schema的开销，使得performance\_schema占用的资源更少，因此，在MySQL 5.6 默认配置中，开启Performance\_schema（MySQL 5.5中默认关闭）；
2. 对各个配置项进行了优化，基本上默认的配置就能满足绝大多数DBA和开发人员的要求。

前面根据performance_schema提供的功能进行了简单的分类，从Instrumentation的粒度上来看，它们之间的关系如下所示：

![img1](/cn/image/performance_schema01.png)

下面是根据各个表的具体作用进行分类：

![img2](/cn/image/performance_schema02.png)

## 3.示例

前面已经介绍了MySQL 5.6 的Performance_schema，相信通过上面的两张图，能够对Performance_schema有一个感性的认识，下面就来看一下，如何通过Performance_schema解决实际问题。

* ACCOUNTS NOT PROPERLY CLOSING CONNECTIONS

     Works since 5.6

        SELECT ess.user, ess.host
             , (a.total_connections - a.current_connections) - ess.count_star as not_closed
             , ((a.total_connections - a.current_connections) - ess.count_star) * 100 /
               (a.total_connections - a.current_connections) as pct_not_closed
          FROM performance_schema.events_statements_summary_by_account_by_event_name ess
          JOIN performance_schema.accounts a on (ess.user = a.user and ess.host = a.host)
         WHERE ess.event_name = 'statement/com/quit'
           AND (a.total_connections - a.current_connections) > ess.count_star ;


* UNUSED INDEXES

    Works since 5.6

        SELECT object_schema, object_name, index_name
          FROM performance_schema.table_io_waits_summary_by_index_usage
         WHERE index_name IS NOT NULL
           AND count_star = 0
         ORDER BY object_schema, object_name ;


* WHO CREATED TEMPORARY (DISK) TABLESWorks

    since 5.6

        SELECT user, host, event_name, count_star AS cnt, sum_created_tmp_disk_tables AS tmp_disk_tables, sum_created_tmp_tables AS tmp_tables
          FROM performance_schema.events_statements_summary_by_account_by_event_name
         WHERE sum_created_tmp_disk_tables > 0
            OR sum_created_tmp_tables > 0 ;


        SELECT schema_name, substr(digest_text, 1, 40) AS statement, count_star AS cnt, sum_created_tmp_disk_tables AS tmp_disk_tables, sum_created_tmp_tables AS tmp_tables
          FROM performance_schema.events_statements_summary_by_digest
         WHERE sum_created_tmp_disk_tables > 0
            OR sum_created_tmp_tables > 0;


* ACCOUNTS WHICH NEVER CONNECTED SINCE LAST START-UP

    Works since 5.6

        SELECT DISTINCT m_u.user, m_u.host
          FROM mysql.user m_u
          LEFT JOIN performance_schema.accounts ps_a ON m_u.user = ps_a.user AND m_u.host = ps_a.host
         WHERE ps_a.user IS NULL
        ORDER BY m_u.user;


* USERS WHICH NEVER CONNECTED SINCE LAST START-UPWorks

     since 5.6

        SELECT DISTINCT m_u.user
          FROM mysql.user m_u
          LEFT JOIN performance_schema.users ps_u ON m_u.user = ps_u.user
         WHERE ps_u.user IS NULL
         ORDER BY m_u.user;


* TOTALLY UNUSED ACCOUNTS (NEVER CONNECTED SINCE LAST RESTART AND NOT USED TO CHECK STORED PROGRAM OR VIEW PRIVILEGES) SINCE LAST START-UPWorks

    since 5.6

        SELECT DISTINCT m_u.user, m_u.host
          FROM mysql.user m_u
          LEFT JOIN performance_schema.accounts ps_a ON m_u.user = ps_a.user AND ps_a.host = m_u.host
          LEFT JOIN information_schema.views is_v ON is_v.DEFINER = CONCAT(m_u.User, '@', m_u.Host) AND is_v.security_type = 'DEFINER'
          LEFT JOIN information_schema.routines is_r ON is_r.DEFINER = CONCAT(m_u.User, '@', m_u.Host) AND is_r.security_type = 'DEFINER'
          LEFT JOIN information_schema.events is_e ON is_e.definer = CONCAT(m_u.user, '@', m_u.host)
          LEFT JOIN information_schema.triggers is_t ON is_t.definer = CONCAT(m_u.user, '@', m_u.host)
         WHERE ps_a.user IS NULL
           AND is_v.definer IS NULL
           AND is_r.definer IS NULL
           AND is_e.definer IS NULL
           AND is_t.definer IS NULL
         ORDER BY m_u.user, m_u.host;


* SHOW FULL PROCESSLISTWorks

    since 5.5 (5.1?)

    But with filter on Sleep and sorting by time to find the evil query...

        SELECT id, user, host, db, command, time, state, LEFT(info, 80) AS info
          FROM information_schema.processlist
         WHERE command NOT IN ('Sleep', 'Binlog Dump')
         ORDER BY time ASC;


    Non blocking version, since 5.6:

        SELECT PROCESSLIST_ID AS id, PROCESSLIST_USER AS user, PROCESSLIST_HOST AS host, PROCESSLIST_DB AS db
             , PROCESSLIST_COMMAND AS command, PROCESSLIST_TIME AS time, PROCESSLIST_STATE AS state, LEFT(PROCESSLIST_INFO, 80) AS info
          FROM performance_schema.threads
         WHERE PROCESSLIST_ID IS NOT NULL
           AND PROCESSLIST_COMMAND NOT IN ('Sleep', 'Binlog Dump')
         ORDER BY PROCESSLIST_TIME ASC ;


* STORAGE ENGINES PER SCHEMA

    For defining backup strategy, preparing migration to InnoDB or Galera Cluster for MySQL, etc.

    Works since 5.5 (5.1?)

        SELECT table_schema AS `schema`, engine, COUNT(*) AS `tables`
             , ROUND(SUM(data_length)/1024/1024, 0) AS data_mb, ROUND(SUM(index_length)/1024/1024, 0) index_mb
          FROM information_schema.tables
         WHERE table_schema NOT IN ('mysql', 'information_schema', 'performance_schema')
           AND engine IS NOT NULL
         GROUP BY table_schema, engine ;

        +---------------------+--------+--------+---------+----------+
        | schema              | engine | tables | data_mb | index_mb |
        +---------------------+--------+--------+---------+----------+
        | mantis              | MyISAM |     31 |       0 |        0 |
        | mpm                 | InnoDB |      3 |       0 |        0 |
        | mysql_sequences     | InnoDB |      2 |       0 |        0 |
        | mysql_sequences     | MEMORY |      1 |       0 |        0 |
        | otrs                | InnoDB |     73 |      13 |        4 |
        | quartz              | InnoDB |     12 |       0 |        0 |
        | tracking            | MyISAM |      1 |       0 |        0 |
        +---------------------+--------+--------+---------+----------+


* TABLES WITHOUT A PRIMARY KEY

    Galera Cluster, InnoDB, M/S replication with row based replication does not work well with tables without a Primary Key. To find those the following query helps:

    Works since 5.5 (5.1?)

        SELECT DISTINCT t.table_schema, t.table_name
          FROM information_schema.tables AS t
          LEFT JOIN information_schema.columns AS c ON t.table_schema = c.table_schema AND t.table_name = c.table_name AND c.column_key = "PRI"
         WHERE t.table_schema NOT IN ('information_schema', 'mysql', 'performance_schema')
           AND c.table_name IS NULL AND t.table_type != 'VIEW';

        +--------------+--------------------+
        | table_schema | table_name         |
        +--------------+--------------------+
        | test         | t_wo_pk            |
        | test         | t_wo_pk_with_Index |
        +--------------+--------------------+

