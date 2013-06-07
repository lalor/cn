---
layout: post
title: "LevelDB源码剖析之snapshot原理"
description: ""
category: NoSql
tags: [levelDB, Snapshot]
---

**注意：**本文转载自小和尚的[藏经阁][2]，无任何修改。

Snapshot（快照）关于指定数据集合的一个完全可用拷贝，该拷贝包括相应数据在某个时间点（拷贝开始的时间点）的映像。快照可以是其所表示的数据的一个副本，也可以是数据的一个复制品。

快照的作用主要是能够进行在线数据备份与恢复[1][1]

leveldb也提供了快照。对于整个key-value存储状态，Snapshot提供了一致性只读视图。ReadOptions::snapshot不为NULL时表示读取数据库的某一个特定版本。如果Snapshot为NULL，则读取数据库的当前版本。

通常Snapshots通过方法DB::GetSnapshot()创建：

	leveldb::ReadOptions options;
	options.snapshot = db->GetSnapshot();
	... apply some updates to db ...
	leveldb::Iterator* iter = db->NewIterator(options);
	... read using iter to view the state when the snapshot was created
	...
	delete iter;
	db->ReleaseSnapshot(options.snapshot);


注意，当snapshot不再使用时，需要用DB::RealeaseSnapshot释放。这样底层就可以释放支持Snapshot的资源。

写操作也可以返回一个应用了一系列更新之后的Snapshot：

	leveldb::Snapshot* snapshot;
	leveldb::WriteOptions write_options;
	write_options.post_write_snapshot = &snapshot;
	leveldb::Status status = db->Write(write_options, ...);
	... perform other mutations to db ...
	
	leveldb::ReadOptions read_options;
	read_options.snapshot = snapshot;
	leveldb::Iterator* iter =
	db->NewIterator(read_options);
	... read as of the state just after the Write call returned ...
	delete iter;
	 
	db->ReleaseSnapshot(snapshot);

leveldb中的Snapshot是如何实现的呢？

我们知道leveldb是只增加的数据库,在一定的时间范围内leveldb并不删除数据，它记录了所有的操作。比如:

	  table["liming"] = 18
	  del table["liming"]
	   
	  table["wangdong"] = 85
	  table["wangdong"] = 30


转换成leveldb内部操作为：

	  liming 1 kTypeValue     : 18
	  liming 2 kTypeDeletion
	  wandong 3 ktypeValue    : 85
	  wandong 4 kTypeValue    : 30

内部按照key非递减，sequence非递增，kTypeValue非递增排序(保证kTypeDeletion在前面)进行排序（存储在SkipList中），那么在这些数据在skiplist中为：

	  //userkey sequence type : value
	  liming 2 kTypeDeletion
	  liming 1 kTypeValue     : 18
	   
	  wandong 4 kTypeValue    : 30
	  wandong 3 ktypeValue    : 85

这时我们获得了快照号4(最后更新的sequence号），我们只读取`seqence <= 4`的元素，如果查找`table["wandong"]`, 那么找到`wandong 4 kTypeValue : 30`，可以获得`table["wandong"]`为 30，我们只取sequence最大的值作为数据的最新状态（自动忽略`wandong 3 ktypeValue : 85`), 如果查找`table["liming"]`,找到liming的最新数据`liming 2 kTypeDeletion`，表示已经删掉了liming，则数据库中没有liming'.

如果之后又有更新比如：

	  table["liming"] = 19
	  table["wandong"]= 21
	  table["jim"] = 30

这时skiplist中的内容变为：

	  liming 5 kTypeValue     : 19
	  liming 2 kTypeDeletion
	  liming 1 kTypeValue     : 18
	   
	  jim    7 kTypeValue     : 30
	   
	  wandong 6 kTypeValue    : 21
	  wandong 4 kTypeValue    : 30
	  wandong 3 ktypeValue    : 85

这时我们查找`table["liming"]`, 由于我们指读取`sequence <= 4`的数据所以我们会自动忽略新插入的元素liming 5 kTypeValue : 19', jim 7 kTypeValue : 30,wandong 6 kTypeValue : 21, 所以我们snapshot为4时，数据库中仍没有liming, table["wandong"] 为 30.

另外LevelDb中的SkipList中的指针为AtomicPointer，对它的读写操作是原子的。多线程写需要在外部加锁，因此最多只有一个线程写SkipList,

但是多个线程可以不用加锁并发的读取SkipList，Snapshot不关心新插入的key-value（因为他们的sequence>snapshot的sequence编号）。线程写SkipList的时候，仍然可以多线程读SkipList，并不会有问题。

[1]: http://baike.baidu.com/view/16510.htm
[2]: http://blog.xiaoheshang.info/?p=339
