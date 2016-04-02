---
layout: post
title: "MySQL的内存计算公式"
description: ""
category: 数据库
tags: [MySQL]
---


本文将讨论MySQL内存相关的一些选项，包括：1）全局的buffer，如innodb_buffer_pool_size；2）线程级的选项，如binlog_cache_size；3）为特定操作分配的缓冲区，如sort_buffer_size。

我们只讨论内存相关的选项，其他一些如innodb_open_files、thread_cache_size、table_definition_cache、table_open_cache这类限制文件描述符和线程数量的选项不在讨论之列。

# 1. MySQL的理论内存上限

最近我们在做的一个项目，需要检查MySQL的理论内存上限，同事在[这里][1]找到一个内存计算公式，如下所示：

    key_buffer_size + query_cache_size + tmp_table_size + innodb_buffer_pool_size + innodb_additional_mem_pool_size  + innodb_log_buffer_size + max_connections * (sort_buffer_size + read_buffer_size + read_rnd_buffer_size + join_buffer_size + thread_stack + binlog_cache_size)

这个公式可以说是错的，因为，这个公式并不能非常准确的描述MySQL的实际内存使用情况，实际内存使用情况远比这个公式要复杂，随着具体的应用场景不同而存在差异。这个公式也可以说是对的，因为，该公式大致估算了一个MySQL实例可能需要的内存，可以作为一个参考值。

那么，在实际应用中，我们怎么判断具体的内存使用情况？根据什么条件来调整这些参数？调整为多少比较合适呢？要回答这些问题，还得从原理讲起。

# 2. MySQL内存参数的含义

在这一节，我们首先对内存参数进行了一个分类，然后对各个参数的含义进行了详细的讨论。

## 2.1 内存参数分类
1. 服务器级别

    服务器级别的buffer是全局且唯一的，影响所有的连接和查询，需要注意的是，服务器级别的buffer中，大部分是服务器启动的时候分配的，小部分是后来分配的。如query_cache，初始值是0，后续不断增长，直至最大。

    * query_cache_size
    * innodb_additional_mem_pool_size
    * innodb_buffer_pool_size
    * innodb_log_buffer_size
    * key_buffer_size

2. 线程级别
    这些buffer是线程级别的，对于每个线程都会分配，因此，占用的内存情况为max_connection * (thread_options)，线程级别的选项有：

    * net_buffer_length
    * thread_stack
    * query_prealloc_size
    * binlog_cache_size
    * binlog_stmt_cache_size

3. 为特定操作分配的缓冲区
    当服务器执行特殊操作时，根据需要分配缓冲区。因此，很难计算缓冲区的具体大小。好在这些缓冲区都是session级别的，我们可以保持全局较小的取值，如果需要，再修改session级别的缓冲区大小。
    1. 对每个线程分配一次

        * read_rnd_buffer_size
        * sort_buffer_size
        * myisam_mmap_size
        * myisam_sort_buffer_size
        * bulk_insert_buffer_size
        * preload_buffer_size

    2. 对每个线程分配多次（可能）

        * join_buffer_size
        * read_buffer_size
        * tmp_table_size

## 2.2 内存参数的含义

各个变量的含义比较好掌握，官方参考手册上都有，但是，什么时候应该修改，修改为多少才是合适的值？手册上并没有给出，本文将回答这个问题。

* query_cache_size
    这是MySQL的查询缓存，用以缓冲SQL语句的结果，如果下次有相同的SQL语句，并且，结果还没有invalid掉，则直接返回查询缓存中的结果即可。这是理想情况，实际情况query_cache可能导致激烈的锁竞争，使得性能反而下降，MySQL 5.7已经可以关闭query_cache了。

* innodb_additional_mem_pool_size
    该缓存用以存放数据字典和内部数据结构的信息，一般情况下，表越多，该选项也应该越大，该选项过小时，Innodb会在错误日志中打印错误信息，可以等到有错误日志以后再调整。

* innodb_buffer_pool_size
    Innodb为存储数据、索引、undo、自适应索引等分配的内存大小，影响innodb性能最重要的选项，一般设置为物理内存的80%。

* innodb_log_buffer_size
    Innodb重做日志（redo）的大小，一般取默认值即可。

* key_buffer_size
    MyISAM表缓存索引的缓存，建议不用MyISAM表。

* net_buffer_length
    服务器在客户端连接建立以后创建的缓存大小，用来保持请求和结果。根据需要，这个大小可以增长至max_allowed_packet。

* thread_stack
    每个线程的栈大小，如果该变量设置过小，将会限制SQL语句的复杂性、存储过程的递归深度，以及服务器上其他内存消耗型的操作。对于大部分安装来说，默认取值即可。如果有类似"Thread stack overrun"，则需要增大该值。

* query_prealloc_size
    此缓存为语句解析和执行而分配，如果运行复杂查询，增加缓存是合理的，这样mysqld不会在执行查询的时候在分配内存上耗时。

* binlog_cache_size
    缓存binlog的缓冲区，如果大于该值，缓存中的binlog将写到磁盘的临时文件中。

* binlog_stmt_cache_size
    缓存非事务表的binlog。

* [read_rnd_buffer_size][2]
    存放排序和发送结果至客户端之间，读取结果的大小，大的值能提高order by的性能

* sort_buffer_size
    每个线程需要排序的时候会分配此缓存，通过检查sort_merge_passes状态变量来判断是否需要增加该缓存的大小。sort_buffer_size缓存经常会分配，所以，大的GLOBAL值会降低性能而不是增加性能。因此，最好不要设置得太大，在需要的时候通过set session增加即可。

* join_buffer_size
    连接操作分配的缓冲区，为了检查是否需要增加join_buffer_size的取值，可以检查Select_sacn状态变量，它包括第一张表执行完整扫描的连接次数，同样，select_full_range_join，它包含使用范围搜索的连接次数。

* read_buffer_size
    为表顺序扫描分配的缓存

* tmp_table_size
    临时表的最大值，服务器默认设置为max_heap_table_size和tmp_table_size两者中较小的一个，如果有足够的内存，并且created_tmp_disk_tables状态变量再增大，则可以适当调大，把需要临时表的所有结果保持在内存中，以提高性能。

[1]: http://www.mysqlcalculator.com/
[2]:https://www.percona.com/blog/2007/07/24/what-exactly-is-read_rnd_buffer_size/
