---
layout: post
title: "leveldb编译与调试"
description: ""
category: nosql
tags: [leveldb]
---

最近有两个小伙伴，希望我将调试leveldb的一些心得体会总结一下，以便他们学习的时候也能够进行调试跟踪。因此，这篇文章对leveldb编译调试进行了整理，文章后面还会给出我在学习leveldb过程中，整理的一些零散的学习笔记。希望能够帮助这些好学的小伙伴。

# leveldb编译与调试

这一部分讲解leveldb的编译与调试，可以分为3步：

- 获取leveldb
- 编译DEBUG版本的leveldb
- 编写测试代码，进行单步调试

###  step 1 获取leveldb

假设在我的home目录有个github目录，我们在github目录下进行调试，获取leveldb命令如下：


    cd ~/github
    git clone https://github.com/google/leveldb.git
    cd leveldb

### step 2 编译leveldb

这是关键的一步，打开Makefile，在第12行前后，有一行内容为：`OPT ?= -O2 -DNDEBUG`，注释掉，改成下面这样：

	vim Makefile

	# OPT ?= -O2 -DNDEBUG
	OPT ?= -ggdb3

改完以后保存退出，执行`make`，编译成功以后，会提示

	ar: creating libleveldb.a

### 调试leveldb

最后就剩下调试了，就在当前目录下，新建一个test.cpp文件，调用leveldb的API。

	touch test.cpp

我的测试程序如下：


    #include <assert.h>
    #include <string.h>
    #include <leveldb/db.h>
    #include <iostream>


    int main(){
    	leveldb::DB* db;
    	leveldb::Options options;
    	options.create_if_missing = true;
    	leveldb::Status status = leveldb::DB::Open(options,"/tmp/testdb", &db);
    	assert(status.ok());

    	//write key1,value1
    	std::string key="key";
    	std::string value = "value";

    	status = db->Put(leveldb::WriteOptions(), key,value);
    	assert(status.ok());

        for ( int i = 0; i < 1000000; i++) {
            char str[100];
            snprintf(str, sizeof(str), "%d", i);
            status = db->Put(leveldb::WriteOptions(), str, str);
        }

    	status = db->Get(leveldb::ReadOptions(), key, &value);
    	assert(status.ok());
    	std::cout<<value<<std::endl;
    	std::string key2 = "key2";

    	//move the value under key to key2
    	status = db->Put(leveldb::WriteOptions(),key2,value);
    	assert(status.ok());
    	status = db->Delete(leveldb::WriteOptions(), key);

    	assert(status.ok());

    	status = db->Get(leveldb::ReadOptions(),key2, &value);

    	assert(status.ok());
    	std::cout<<key2<<"==="<<value<<std::endl;

    	status = db->Get(leveldb::ReadOptions(),key, &value);

    	if(!status.ok()) std::cerr<<key<<"  "<<status.ToString()<<std::endl;
    	else std::cout<<key<<"==="<<value<<std::endl;

    	delete db;
    	return 0;
    }

完成以后编译：

	g++ test.cpp -o test -L. -I./include -lpthread -lleveldb


然后就可以调试了：

	gdb test
	break main
	break leveldb::DB::open
	r


# leveldb学习笔记

下面是我自己学习leveldb时整理的一些笔记，希望对刚开始研究leveldb的小伙伴有帮助。


## 源文件

	$ tree -I "*test*"
	.
	├── AUTHORS
	├── build_detect_platform
	├── db #具体逻辑
	│   ├── builder.cc  # 定义了BuildTable函数
	│   ├── builder.h
	│   ├── c.cc        # c封装，暂时不用看
	│   ├── db_bench.cc
	│   ├── dbformat.cc # 搞懂几个key的关系即可，internal，lookupkey
	│   ├── dbformat.h
	│   ├── db_impl.cc # 具体实现,DBImpl::Write是重中之重
	│   ├── db_impl.h
	│   ├── db_iter.cc
	│   ├── db_iter.h
	│   ├── filename.cc   # 管理db目录下的个各个文件
	│   ├── filename.h    # 管理db目录下的个各个文件
	│   ├── log_format.h  # 物理日志格式定义
	│   ├── log_reader.cc # 读物理日志,ReadRecord
	│   ├── log_reader.h
	│   ├── log_writer.cc # 日志封装成相应的格式，并写入物理日志
	│   ├── log_writer.h
	│   ├── memtable.cc   # memtable定义，对记录进行格式化，并调用skiplist的相应接口
	│   ├── memtable.h    #
	│   ├── repair.cc     # 啥？
	│   ├── skiplist.h    # skiplist的定义
	│   ├── snapshot.h    # 成员变量只有SequenceNumber的双链表<http://www.blogjava.net/sandy/archive/2012/03/13/leveldb5.html>
	│   ├── table_cache.cc # 缓存sst文件，保存sst文件与Table的对应关系，
	│   ├── table_cache.h  # 要想全部搞懂Table，还需要弄清楚format.cc
	│   ├── version_edit.cc #啥?
	│   ├── version_edit.h
	│   ├── version_set.cc
	│   ├── version_set.h
	│   ├── write_batch.cc  # 批量写操作的实现，WriteBatch::Iterate
	│   └── write_batch_internal.h # 批量操作辅助文件
	├── doc #帮助文档
	│   ├── bench
	│   │   ├── db_bench_sqlite3.cc
	│   │   └── db_bench_tree_db.cc
	│   ├── benchmark.html
	│   ├── doc.css
	│   ├── impl.html
	│   ├── index.html
	│   ├── log_format.txt
	│   └── table_format.txt
	├── helpers
	│   └── memenv
	│   ├── memenv.cc
	│   └── memenv.h
	├── include        # 接口
	│   └── leveldb
	│   ├── cache.h
	│   ├── c.h
	│   ├── comparator.h
	│   ├── db.h
	│   ├── env.h
	│   ├── filter_policy.h
	│   ├── iterator.h
	│   ├── options.h
	│   ├── slice.h
	│   ├── status.h
	│   ├── table_builder.h
	│   ├── table.h
	│   └── write_batch.h
	├── libleveldb.so.1.4
	├── LICENSE
	├── Makefile
	├── NEWS
	├── port
	│   ├── atomic_pointer.h  # 无锁编程
	│   ├── port_android.cc
	│   ├── port_android.h
	│   ├── port_example.h
	│   ├── port.h
	│   ├── port_posix.cc
	│   ├── port_posix.h
	│   ├── README
	│   └── win
	│   └── stdint.h
	├── README
	├── table
	│   ├── block_builder.cc # 构造block，准备写入
	│   ├── block_builder.h  # 构造block，准备写入
	│   ├── block.cc         # 通过ReadBlock读取，通过迭代器读取各个record
	│   ├── block.h          # 读取block。定义了block，和相应的迭代器
	│   ├── filter_block.cc  # 与block类似，用以处理filter_block
	│   ├── filter_block.h   # 与block类似，用以处理filter_block
	│   ├── format.cc        # 定义了ReadBlock函数[重点]
	│   ├── format.h         # 定义了BlockContent，BlockHandle，Footer
	│   ├── iterator.cc      # 迭代器？
	│   ├── iterator_wrapper.h # 迭代器？
	│   ├── merger.cc        # 定义了MergingIterator迭代器
	│   ├── merger.h
	│   ├── table_builder.cc # 构造Table，Add()、Flush()、WriteBlock
	│   ├── table.cc         # 定义了操作table的方法，InternalGet
	│   ├── two_level_iterator.cc # 定义了TwoLevelIterator迭代器
	│   └── two_level_iterator.h
	├── TODO
	└── util
		├── arena.cc       # 内存分配器
		├── arena.h        # 内存分配器
		├── bloom.cc       # 布隆过滤器
		├── cache.cc       # Cache
		├── coding.cc
		├── coding.h       # 编码
		├── comparator.cc  # 比较函数
		├── crc32c.cc      # crc32校验
		├── crc32c.h       # crc32校验
		├── env.cc         #日志相关
		├── env_posix.cc   # 文件操作封装和线程封装
		├── filter_policy.cc
		├── hash.cc        # hash函数
		├── hash.h         # hash函数
		├── histogram.cc   # 测试文件
		├── histogram.h    # 测试文件
		├── logging.cc     # 日志
		├── logging.h      # 日志头文件
		├── mutexlock.h    # mutex
		├── options.cc     # option
		├── posix_logger.h # 日志文件
		├── random.h       # 随机数生成器
		└── status.cc      # 错误管理，Status

		11 directories, 109 files

1. 流程图<http://www.wzxue.com/leveldb%E5%9B%BE%E8%A7%A3/>
2. 读日志<http://blog.csdn.net/sparkliang/article/details/8627836>
3. SSTable的读写操作(8,9,10)：**重点博客**<http://blog.csdn.net/sparkliang/article/details/8681759>

# 1 Table

### 1.1 Table

block的具体实现在block.cc/.h与block_builder.cc/.h中。前者用于读，后者用于写。读取时，一般先通过前面提到的ReadBlock读取一块至BlockContent中，然后再通过Block的构造函数初始化，并通过Block::Iter子类用于外部遍历或者定位具体某个key。

http://ju.outofmemory.cn/entry/54894

### 1.2 BlockHandle

BlockHandle是一个internal pointer.

	// BlockHandle is a pointer to the extent of a file that stores a data
	// block or a meta block.
	class BlockHandle {
	 public:
	  BlockHandle();

	  // The offset of the block in the file.
	  uint64_t offset() const { return offset_; }
	  void set_offset(uint64_t offset) { offset_ = offset; }

	  // The size of the stored block
	  uint64_t size() const { return size_; }
	  void set_size(uint64_t size) { size_ = size; }

	  void EncodeTo(std::string* dst) const;
	  Status DecodeFrom(Slice* input);

	  // Maximum encoding length of a BlockHandle
	  enum { kMaxEncodedLength = 10 + 10 };

	 private:
	  uint64_t offset_;
	  uint64_t size_;
	};

### 1.3 BlockContents

BlockContents代表了外部block的内容，通过ReadBlock函数获取BlockContent，然后传递给Block类的构造函数，通过Block类的迭代器，访问Block中的Record。

	struct BlockContents {
	  Slice data;           // Actual contents of data
	  bool cachable;        // True iff data can be cached
	  bool heap_allocated;  // True iff caller should delete[] data.data()
	};

# 2 Area

Arena主要提供了两个申请函数：其中一个直接分配内存，另一个可以申请对齐的内存空间。Arena没有直接调用delete/free函数，而是由Arena的析构函数统一释放所有的内存。

应该说这是和leveldb特定的应用场景相关的，比如一个memtable使用一个Arena，当memtable被释放时，由Arena统一释放其内存。

# db_impl

##struct

*    CompactionState [leveldb::DBImpl]
*    IterState [leveldb::__anon1]
*    Output [leveldb::DBImpl::CompactionState]
*    Writer [leveldb::DBImpl]

##function

*    BGWork [leveldb::DBImpl]
*    BackgroundCall [leveldb::DBImpl]
*    BackgroundCompaction [leveldb::DBImpl]
*    BuildBatchGroup [leveldb::DBImpl]
*    CleanupCompaction [leveldb::DBImpl]
*    CleanupIteratorState [leveldb::__anon1]
*    ClipToRange [leveldb]
*    CompactMemTable [leveldb::DBImpl]
*    CompactRange [leveldb::DBImpl]
*    CompactionState [leveldb::DBImpl::CompactionState]
*    DBImpl [leveldb::DBImpl]
*    Delete [leveldb::DB]
*    Delete [leveldb::DBImpl]
*    DeleteObsoleteFiles [leveldb::DBImpl]
*    DestroyDB [leveldb]
*    DoCompactionWork [leveldb::DBImpl]
*    FinishCompactionOutputFile [leveldb::DBImpl]
*    Get [leveldb::DBImpl]
*    GetApproximateSizes [leveldb::DBImpl]
*    GetProperty [leveldb::DBImpl]
*    GetSnapshot [leveldb::DBImpl]
*    InstallCompactionResults [leveldb::DBImpl]
*    MakeRoomForWrite [leveldb::DBImpl]
*    MaybeIgnoreError [leveldb::DBImpl]
*    MaybeScheduleCompaction [leveldb::DBImpl]
*    NewDB [leveldb::DBImpl]
*    NewInternalIterator [leveldb::DBImpl]
*    NewIterator [leveldb::DBImpl]
*    Open [leveldb::DB]
*    OpenCompactionOutputFile [leveldb::DBImpl]
*    Put [leveldb::DB]
*    Put [leveldb::DBImpl]
*    Recover [leveldb::DBImpl]
*    RecoverLogFile [leveldb::DBImpl]
*    ReleaseSnapshot [leveldb::DBImpl]
*    SanitizeOptions [leveldb]  #检查用户的option
*    TEST_CompactMemTable [leveldb::DBImpl]
*    TEST_CompactRange [leveldb::DBImpl]
*    TEST_MaxNextLevelOverlappingBytes [leveldb::DBImpl]
*    TEST_NewInternalIterator [leveldb::DBImpl]
*    Write [leveldb::DBImpl]
*    WriteLevel0Table [leveldb::DBImpl]
*    Writer [leveldb::DBImpl::Writer]
*    current_output [leveldb::DBImpl::CompactionState]
*    ~DB [leveldb::DB]
*    ~DBImpl [leveldb::DBImpl]
*    ~Snapshot [leveldb::Snapshot]



## util

* arena [√]
* Status [√]
* Compaction 线程(minor compaction、major compaction)
* 缓存文件
* 多路归并
* Table Cache [√]
* Block Cache [√]
* Log [√]
	* log info
	* log
* version管理
* 快照 snpashot [√]
*  writeBatch [√]
* Iteration
* 测试TEST
* db_impl [√]
* TableBuilder是什么玩意？

# db_impl

在db_impl.cc中定义了各种结构体，用以保存中间状态

* CompactionState
* IterStat
* Output
* Writer


# filename

管理所有与levelDB相关的文件的文件名

* CurrentFileName
* DescriptorFileName
* InfoLogFileName
* LockFileName
* LogFileName
* MakeFileName
* OldInfoLogFileName
* ParseFileName[重要：解析leveldb拥有的文件]
* SetCurrentFile[重要：切换CurrentFile的内容，写tmp，sync，rename]
* TableFileName
* TempFileName

**对文件进行分类**

	enum FileType {
	  kLogFile,
	  kDBLockFile,
	  kTableFile,
	  kDescriptorFile,
	  kCurrentFile,
	  kTempFile,
	  kInfoLogFile  // Either the current one, or an old one
	};


## C语言可变参数

<http://www.cnblogs.com/haoyuanyuan/p/3221463.html>

* va_list
* va_start
* va_end
* va_args

## open 流程

1. open
	* new DBImpl
		* SanitizeOptions
			* ClipToRange(检查用户输入的参数)
			* CreateDir（创建数据库目录）
			* RenameFile（重命名info log）
			* env->NewLogger->New PosixLogger（创建info log）
		* New MemTable
		* New TableCache
		* New VersionSet
		* New TableCache
		* New VersionSet
	* recover
		* RecoverLogFile
			* WriteLevel0Table
		* New WritableFile impl->(logfile, logfile_number,log_) 物理日志
	* DeleteObsoleteFiles
	* MaybeScheduleCompaction

## put流程

	#-1 WriteLevel0Table
	#0  leveldb::SkipList<char const*, leveldb::MemTable::KeyComparator>::Insert (this=0x8096ddc,
	    key=@0xbffff364: 0x8096e2c "\vkey\001", <incomplete sequence \374>) at ./db/skiplist.h:338
	#1  0x0805b61c in leveldb::MemTable::Add (this=0x8096db8, s=64560, type=leveldb::kTypeValue, key=..., value=...) at db/memtable.cc:105
	#2  0x08067ede in leveldb::(anonymous namespace)::MemTableInserter::Put (this=0xbffff470, key=..., value=...) at db/write_batch.cc:118
	#3  0x08067be6 in leveldb::WriteBatch::Iterate (this=0xbffff55c, handler=0xbffff470) at db/write_batch.cc:59
	#4  0x08068012 in leveldb::WriteBatchInternal::InsertInto (b=0xbffff55c, memtable=0x8096db8) at db/write_batch.cc:133
	#5  0x0804f886 in leveldb::DBImpl::Write (this=0x80962f0, options=..., my_batch=0xbffff55c) at db/db_impl.cc:1145
	#6  0x08050806 in leveldb::DB::Put (this=0x80962f0, opt=..., key=..., value=...) at db/db_impl.cc:1369
	#7  0x0804f56d in leveldb::DBImpl::Put (this=0x80962f0, o=..., key=..., val=...) at db/db_impl.cc:1103
	#8  0x08049f66 in main () at test.cpp:22

## Get流程

	#0  leveldb::SkipList<char const*, leveldb::MemTable::KeyComparator>::Iterator::Seek (this=0xbffff49c,
	   target=@0xbffff4cc: 0xbffff564 "\vkey\001", <incomplete sequence \374>) at ./db/skiplist.h:223
	#1  0x0805b3da in leveldb::MemTable::Get (this=0x8095db8, key=..., value=0xbffff6c0, s=0xbffff6c8) at db/memtable.cc:111
	#2  0x0804ef5c in leveldb::DBImpl::Get (this=0x80952f0, options=..., key=..., value=0xbffff6c0) at db/db_impl.cc:1061
	#3  0x08049fbd in main () at test.cpp:40

## Skiplist

1. skiplist
	* class Iterator
	* Comparator
	* Arena
	* Node Struct

2. function

* Contains
* Equal
* FindGreaterOrEqual
* FindLast
* FindLessThan
* GetMaxHeight
* Insert
* Iterator
* KeyIsAfterNode
* NewNode
* Next
* Next
* NoBarrier_Next
* NoBarrier_SetNext
* Node
* Prev
* RandomHeight
* Seek
* SeekToFirst
* SeekToLast
* SetNext
* SkipList
* Valid
* key

## User Key

* InternalKey
* ParsedInternalKey
* User Key
* LookupKey

InternalKey是一个复合概念，是有几个部分组合成的一个key，ParsedInternalKey就是对InternalKey分拆后的结果。

InternalKey和ParsedInternalKey相互转换的两个函数，如下。

	bool ParseInternalKey (const Slice& internal_key, ParsedInternalKey* result);

	void AppendInternalKey (std::string* result, const ParsedInternalKey& key);


Memtable的查询接口传入的是LookupKey，它也是由User Key和Sequence Number组合而成的，从其构造函数：LookupKey(const Slice& user_key, SequenceNumber s)中分析出LookupKey的格式为：

	| Size (int32变长)| User key (string) | sequence number (7 bytes) | value type (1 byte) |

两点：
* 这里的Size是user key长度+8，也就是整个字符串长度了；
* value type是kValueTypeForSeek，它等于kTypeValue。

# Compaction

1. Get、MakeRoomForWrite、Open、BackgroundCall
1. MaybeScheduleCompaction
2. env_->Schedule(pthreadCall->Env::BGThreadWrapper->调用子类的BGThread->加mu_锁，从queue中取出BGItem，调用相应的函数)
2. DBImpl::BackgroundCall
2. DBImpl::BackgroundCompaction
	3. CompactMemTable
2. DBImpl::DoCompactionWork
MakeRoomForWrite

## Env.h

从Env.h中的定义，就能一窥Env到底封装了什么，对哪些操作进行了封装，主要有以下几个部分：

* Env
* EnvWrapper
* FileLock
* Logger
* RandomAccessFile
* SequentialFile
* WritableFile

Env.cc

1. DoWriteStringToFile
2. Log
3. ReadFileToString

## pthread_create

	struct StartThreadState {
	  void (*user_function)(void*);
	  void* arg;
	};
	}
	static void* StartThreadWrapper(void* arg) {
	  StartThreadState* state = reinterpret_cast<StartThreadState*>(arg);
	  state->user_function(state->arg);
	  delete state;
	  return NULL;
	}

	void PosixEnv::StartThread(void (*function)(void* arg), void* arg) {
	  pthread_t t;
	  StartThreadState* state = new StartThreadState;
	  state->user_function = function;
	  state->arg = arg;
	  PthreadCall("start thread",
	              pthread_create(&t, NULL,  &StartThreadWrapper, state));
	}


[1]: 读取SSTable <http://blog.csdn.net/sparkliang/article/details/8681759>
[2]: BuilderTable <http://www.blogjava.net/sandy/archive/2012/03/12/leveldb4.html>
