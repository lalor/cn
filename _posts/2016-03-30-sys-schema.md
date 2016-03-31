---
layout: post
title: "MySQL 5.7的sys schema"
description: ""
category: 数据库
tags: [MySQL]
---




本文详细地介绍了MySQL 5.7新引入的sys schema。首先，本文概要地介绍了sys schema的作用和定位；其次，分别介绍了sys schema中的视图、函数和存储过程；接下来，通过两个例子来演示sys schema的用法，便于大家理解sys schema带来的实实在在的好处；最后讨论了sys schema还可以增加的内容。

## 1. sys schema的介绍

sys schema是MySQL 5.7.7中引入的一个系统库，包含了一系列视图、函数和存储过程，
该项目专注于MySQL的易用性，例如，我们可以通过sys schema快速的知道，哪些语句使用了临时表，哪个用户请求了最多的io，哪个线程占用了最多的内存，哪些索引是无用索引等。

引入sys schema以后，MySQL的易用性将会得到极大地提升，MySQL的用户分析问题和定位问题，将更多的依赖sys schema，减少外部工具的使用。

前面说过，sys schema中包含了大量的视图（只有sys_config是innodb表），那么，这些视图的信息来自哪里呢？视图中的信息均来自performance schema和information schema中的统计信息。[MySQL Server blog][1]中有一个很好的比喻：

> For Linux users I like to compare performance_schema to /proc, and SYS to vmstat.

也就是说，performance schema和information schema中提供了信息源，但是，没有很好的将这些信息组织成有用的信息，从而没有很好的发挥它们的作用。而sys schema使用performance schema和information schema中的信息，通过视图的方式给出解决实际问题的答案。这就是sys schema的作用和目的，也是为什么sys schema值得我们花点时间学习的原因。


## 2. sys schema中的视图、函数和存储过程

可以通过以下语句快速查看sys schema包含的视图、函数和存储过程

     show full tables from sys
     show function status where db = 'sys';
     show procedure status where db = 'sys'

接下来将依次给出所有的视图、函数和存储过程，并进行简单的分析，希望能够达到抛砖引玉的效果。

### 2.1 视图

sys schema中的视图（和一张表）如下，通过名称就很容易猜到具体是做什么用的。

    mysql> select table_name , table_type, engine from information_schema.tables where table_schema = 'sys' order by table_name;
    +-----------------------------------------------+------------+--------+
    | table_name                                    | table_type | engine |
    +-----------------------------------------------+------------+--------+
    | host_summary                                  | VIEW       | NULL   |
    | host_summary_by_file_io                       | VIEW       | NULL   |
    | host_summary_by_file_io_type                  | VIEW       | NULL   |
    | host_summary_by_stages                        | VIEW       | NULL   |
    | host_summary_by_statement_latency             | VIEW       | NULL   |
    | host_summary_by_statement_type                | VIEW       | NULL   |
    | innodb_buffer_stats_by_schema                 | VIEW       | NULL   |
    | innodb_buffer_stats_by_table                  | VIEW       | NULL   |
    | innodb_lock_waits                             | VIEW       | NULL   |
    | io_by_thread_by_latency                       | VIEW       | NULL   |
    | io_global_by_file_by_bytes                    | VIEW       | NULL   |
    | io_global_by_file_by_latency                  | VIEW       | NULL   |
    | io_global_by_wait_by_bytes                    | VIEW       | NULL   |
    | io_global_by_wait_by_latency                  | VIEW       | NULL   |
    | latest_file_io                                | VIEW       | NULL   |
    | memory_by_host_by_current_bytes               | VIEW       | NULL   |
    | memory_by_thread_by_current_bytes             | VIEW       | NULL   |
    | memory_by_user_by_current_bytes               | VIEW       | NULL   |
    | memory_global_by_current_bytes                | VIEW       | NULL   |
    | memory_global_total                           | VIEW       | NULL   |
    | metrics                                       | VIEW       | NULL   |
    | processlist                                   | VIEW       | NULL   |
    | ps_check_lost_instrumentation                 | VIEW       | NULL   |
    | schema_auto_increment_columns                 | VIEW       | NULL   |
    | schema_index_statistics                       | VIEW       | NULL   |
    | schema_object_overview                        | VIEW       | NULL   |
    | schema_redundant_indexes                      | VIEW       | NULL   |
    | schema_tables_with_full_table_scans           | VIEW       | NULL   |
    | schema_table_lock_waits                       | VIEW       | NULL   |
    | schema_table_statistics                       | VIEW       | NULL   |
    | schema_table_statistics_with_buffer           | VIEW       | NULL   |
    | schema_unused_indexes                         | VIEW       | NULL   |
    | session                                       | VIEW       | NULL   |
    | session_ssl_status                            | VIEW       | NULL   |
    | statements_with_errors_or_warnings            | VIEW       | NULL   |
    | statements_with_full_table_scans              | VIEW       | NULL   |
    | statements_with_runtimes_in_95th_percentile   | VIEW       | NULL   |
    | statements_with_sorting                       | VIEW       | NULL   |
    | statements_with_temp_tables                   | VIEW       | NULL   |
    | statement_analysis                            | VIEW       | NULL   |
    | sys_config                                    | BASE TABLE | InnoDB |
    | user_summary                                  | VIEW       | NULL   |
    | user_summary_by_file_io                       | VIEW       | NULL   |
    | user_summary_by_file_io_type                  | VIEW       | NULL   |
    | user_summary_by_stages                        | VIEW       | NULL   |
    | user_summary_by_statement_latency             | VIEW       | NULL   |
    | user_summary_by_statement_type                | VIEW       | NULL   |
    | version                                       | VIEW       | NULL   |
    | waits_by_host_by_latency                      | VIEW       | NULL   |
    | waits_by_user_by_latency                      | VIEW       | NULL   |
    | waits_global_by_latency                       | VIEW       | NULL   |
    | wait_classes_global_by_avg_latency            | VIEW       | NULL   |
    | wait_classes_global_by_latency                | VIEW       | NULL   |

这里还没有列出全部的视图，对于每一个视图，都有两种形式：

    mysql> select table_name from information_schema.tables where table_schema = 'sys' and table_name like '%memory%';
    +-------------------------------------+
    | table_name                          |
    +-------------------------------------+
    | memory_by_host_by_current_bytes     |
    | memory_by_thread_by_current_bytes   |
    | memory_by_user_by_current_bytes     |
    | memory_global_by_current_bytes      |
    | memory_global_total                 |
    | x$memory_by_host_by_current_bytes   |
    | x$memory_by_thread_by_current_bytes |
    | x$memory_by_user_by_current_bytes   |
    | x$memory_global_by_current_bytes    |
    | x$memory_global_total               |
    +-------------------------------------+
    10 rows in set (0.00 sec)

前一种是便于人类阅读的形式，格式化了时间单位和字节单位，另外一种形式是指以"x$"开头的视图名称，是为了便于工具处理。如下所示：

    mysql> select * from host_summary_by_file_io;
    +-------------+------+------------+
    | host        | ios  | io_latency |
    +-------------+------+------------+
    | background  | 2143 | 266.48 ms  |
    | 172.17.42.1 | 1748 | 116.52 ms  |
    +-------------+------+------------+
    2 rows in set (0.00 sec)
    
    mysql> select * from x$host_summary_by_file_io;
    +-------------+------+--------------+
    | host        | ios  | io_latency   |
    +-------------+------+--------------+
    | background  | 2148 | 266558291670 |
    | 172.17.42.1 | 1748 | 116518395300 |
    +-------------+------+--------------+
    2 rows in set (0.00 sec)
    
我们再看一下`host_summary_by_file_io`的定义，如下所示：

    mysql> show create table host_summary_by_file_io\G
    *************************** 1. row ***************************
                    View: host_summary_by_file_io
             Create View: CREATE ALGORITHM=TEMPTABLE DEFINER=`mysql.sys`@`localhost` SQL SECURITY INVOKER VIEW `host_summary_by_file_io` AS select if(isnull(`performance_schema`.`events_waits_summary_by_host_by_event_name`.`HOST`),'background',`performance_schema`.`events_waits_summary_by_host_by_event_name`.`HOST`) AS `host`,sum(`performance_schema`.`events_waits_summary_by_host_by_event_name`.`COUNT_STAR`) AS `ios`,`sys`.`format_time`(sum(`performance_schema`.`events_waits_summary_by_host_by_event_name`.`SUM_TIMER_WAIT`)) AS `io_latency` from `performance_schema`.`events_waits_summary_by_host_by_event_name` where (`performance_schema`.`events_waits_summary_by_host_by_event_name`.`EVENT_NAME` like 'wait/io/file/%') group by if(isnull(`performance_schema`.`events_waits_summary_by_host_by_event_name`.`HOST`),'background',`performance_schema`.`events_waits_summary_by_host_by_event_name`.`HOST`) order by sum(`performance_schema`.`events_waits_summary_by_host_by_event_name`.`SUM_TIMER_WAIT`) desc
    character_set_client: utf8
    collation_connection: utf8_general_ci
    1 row in set (0.00 sec)

可以看到，定义非常复杂，如果不是sys schema提供了这么好用的视图，我们自己几乎无法从这么多信息中获取到有用的信息。
    
### 2.2 函数

如果你仔细看了上面的定义视图的SQL语句，你可能注意它使用了一个叫做`sys`.`format_time`的函数，这个函数也是随着sys schema一起发布，用以格式化时间单位。sys schema还包含了其他一些有用的函数，如下所示：

    mysql> select ROUTINE_SCHEMA, ROUTINE_NAME, ROUTINE_TYPE from ROUTINES where ROUTINE_TYPE='FUNCTION';
    +----------------+----------------------------------+--------------+
    | ROUTINE_SCHEMA | ROUTINE_NAME                     | ROUTINE_TYPE |
    +----------------+----------------------------------+--------------+
    | sys            | extract_schema_from_file_name    | FUNCTION     |
    | sys            | extract_table_from_file_name     | FUNCTION     |
    | sys            | format_bytes                     | FUNCTION     |
    | sys            | format_path                      | FUNCTION     |
    | sys            | format_statement                 | FUNCTION     |
    | sys            | format_time                      | FUNCTION     |
    | sys            | list_add                         | FUNCTION     |
    | sys            | list_drop                        | FUNCTION     |
    | sys            | ps_is_account_enabled            | FUNCTION     |
    | sys            | ps_is_consumer_enabled           | FUNCTION     |
    | sys            | ps_is_instrument_default_enabled | FUNCTION     |
    | sys            | ps_is_instrument_default_timed   | FUNCTION     |
    | sys            | ps_is_thread_instrumented        | FUNCTION     |
    | sys            | ps_thread_account                | FUNCTION     |
    | sys            | ps_thread_id                     | FUNCTION     |
    | sys            | ps_thread_stack                  | FUNCTION     |
    | sys            | ps_thread_trx_info               | FUNCTION     |
    | sys            | sys_get_config                   | FUNCTION     |
    | sys            | version_major                    | FUNCTION     |
    | sys            | version_minor                    | FUNCTION     |
    | sys            | version_patch                    | FUNCTION     |
    +----------------+----------------------------------+--------------+
    21 rows in set (0.00 sec)

### 2.3 存储过程

下面的是sys schema包含的存储过程。可以看到，大部分存储过程都是PS开头的，PS是performance schema的简写，这些存储过程都是用来控制performance schema统计的行为。例如，ps_truncate_all_tables 的作用就是truncate所有performance schema中summary相关的表。

如下所示：

    mysql> select ROUTINE_SCHEMA, ROUTINE_NAME, ROUTINE_TYPE from ROUTINES where ROUTINE_TYPE='PROCEDURE';                              
    +----------------+-------------------------------------+--------------+
    | ROUTINE_SCHEMA | ROUTINE_NAME                        | ROUTINE_TYPE |
    +----------------+-------------------------------------+--------------+
    | sys            | create_synonym_db                   | PROCEDURE    |
    | sys            | diagnostics                         | PROCEDURE    |
    | sys            | execute_prepared_stmt               | PROCEDURE    |
    | sys            | ps_setup_disable_background_threads | PROCEDURE    |
    | sys            | ps_setup_disable_consumer           | PROCEDURE    |
    | sys            | ps_setup_disable_instrument         | PROCEDURE    |
    | sys            | ps_setup_disable_thread             | PROCEDURE    |
    | sys            | ps_setup_enable_background_threads  | PROCEDURE    |
    | sys            | ps_setup_enable_consumer            | PROCEDURE    |
    | sys            | ps_setup_enable_instrument          | PROCEDURE    |
    | sys            | ps_setup_enable_thread              | PROCEDURE    |
    | sys            | ps_setup_reload_saved               | PROCEDURE    |
    | sys            | ps_setup_reset_to_default           | PROCEDURE    |
    | sys            | ps_setup_save                       | PROCEDURE    |
    | sys            | ps_setup_show_disabled              | PROCEDURE    |
    | sys            | ps_setup_show_disabled_consumers    | PROCEDURE    |
    | sys            | ps_setup_show_disabled_instruments  | PROCEDURE    |
    | sys            | ps_setup_show_enabled               | PROCEDURE    |
    | sys            | ps_setup_show_enabled_consumers     | PROCEDURE    |
    | sys            | ps_setup_show_enabled_instruments   | PROCEDURE    |
    | sys            | ps_statement_avg_latency_histogram  | PROCEDURE    |
    | sys            | ps_trace_statement_digest           | PROCEDURE    |
    | sys            | ps_trace_thread                     | PROCEDURE    |
    | sys            | ps_truncate_all_tables              | PROCEDURE    |
    | sys            | statement_performance_analyzer      | PROCEDURE    |
    | sys            | table_exists                        | PROCEDURE    |
    +----------------+-------------------------------------+--------------+
    26 rows in set (0.00 sec)
    
## 3. sys schema示例

这一节简单的演示sys schema的用法，便于大家理解sys schema带来的实实在在的好处，然后讨论了sys schema还可以增加的内容。

### 3.1 sys schema的用法

我们来看几个与索引相关的例子：

1. 如何查看数据库中的冗余索引，在MySQL 5.7之前，需要使用percona的 pt-duplicate-key-checker，现在直接查询sys.schema_redundant_indexes就可以了:

        select * from sys.schema_redundant_indexes;

2. 如何获取未使用的索引

    在MySQL 5.7之前，我们也可以通过information schema查看未使用的索引：
    
          SELECT object_schema, object_name, index_name
            FROM performance_schema.table_io_waits_summary_by_index_usage
           WHERE index_name IS NOT NULL
             AND count_star = 0
           ORDER BY object_schema, object_name ;
   
   有了sys schema以后，直接查询schema_unused_indexes即可:
   
        select * from schema_unused_indexes;
   
3. 如何查看使用全表扫描的SQL语句（MySQL 5.7之前我不知道怎么看额(⊙o⊙)…）
    有了sys schema以后，直接查询statements_with_full_table_scans即可：

        select * from statements_with_full_table_scans

### 3.2 sys schema可以引入的视图

我发现，还有一些非常有用的信息，可以通过information schema 和performance schema获取到，但是，在sys schema中却没有。有鉴于此，个人感觉未来sys schema中的视图会继续增加。例如：

* 查看没有主键的表

          SELECT DISTINCT t.table_schema, t.table_name
            FROM information_schema.tables AS t
            LEFT JOIN information_schema.columns AS c ON t.table_schema = c.table_schema AND t.table_name = c.table_name AND c.column_key = "PRI"
           WHERE t.table_schema NOT IN ('information_schema', 'mysql', 'performance_schema')
             AND c.table_name IS NULL AND t.table_type != 'VIEW';
         
* 查看是谁创建的临时表

          SELECT user, host, event_name, count_star AS cnt, sum_created_tmp_disk_tables AS tmp_disk_tables, sum_created_tmp_tables AS tmp_tables
            FROM performance_schema.events_statements_summary_by_account_by_event_name
           WHERE sum_created_tmp_disk_tables > 0
              OR sum_created_tmp_tables > 0 ;
          
* 没有正确关闭数据库连接的用户

          SELECT ess.user, ess.host
               , (a.total_connections - a.current_connections) - ess.count_star as not_closed
               , ((a.total_connections - a.current_connections) - ess.count_star) * 100 /
                 (a.total_connections - a.current_connections) as pct_not_closed
            FROM performance_schema.events_statements_summary_by_account_by_event_name ess
            JOIN performance_schema.accounts a on (ess.user = a.user and ess.host = a.host)
           WHERE ess.event_name = 'statement/com/quit'
             AND (a.total_connections - a.current_connections) > ess.count_star ;
 
这些都是比较有用的信息，适合加入到sys schema中。

## 4. 总结

本文详细地介绍了sys schema的作用和组成，并通过几个例子演示了sys schema的易用性和不足，相信通过这篇文章，大家对sys schema能够有一个较完整的了解，也愿意花时间去深入了解sys schema中的各个视图。

此外，sys schema可以应用在MySQL 5.6上，具体信息见[这里][2]。

[1]: http://mysqlserverteam.com/using-sys-session-as-an-alternative-to-show-processlist/
[2]: https://github.com/MarkLeith/mysql-sys/blob/master/README.md

