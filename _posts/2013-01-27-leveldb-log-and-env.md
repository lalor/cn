---
layout: post
title: "LevelDB源码剖析之Env与log::Writer"
description: ""
category: NoSql
tags: [levelDB, Env]
---

本文将介绍levelDB中的log部分，并且涉及到log::Writer，Env，EnvWrapper和PosixEnv，把这两者搅在一起似乎不太合适，不过，也有合理的地方，正是因为我要对levelDB中文件的写操作看个究竟，才牵扯出了Env，EnvWrapper和PosixEnv之间的关系。讲得不太好，不过，对于刚开始看levelDB源码的人肯定还是有不少帮助的，就我自己而言，刚开始看到`status = log_->AddRecord();`，找了半天也知道levelDB把数据写到什么地方去了。


#1. 日志文件介绍

log文件在LevelDb中的主要作用是系统故障恢复时，能够保证不会丢失数据。因为在将记录写入内存的Memtable之前，会先写入Log文件，这样即使系统发生故障，Memtable中的数据没有来得及Dump到磁盘的SSTable文件，LevelDB也可以根据log文件恢复内存的Memtable数据结构内容，不会造成系统丢失数据，在这点上LevelDb和Bigtable是一致的。下面我们带大家看看log文件的具体物理和逻辑布局是怎样的，LevelDb对于一个log文件，会把它切割成以32K为单位的物理Block，每次读取的单位以一个Block作为基本读取单位，下图展示的log文件由3个Block构成，所以从物理布局来讲，一个log文件就是由连续的32K大小Block构成的。

<a href="http://imgur.com/aNXpY"><img src="http://i.imgur.com/aNXpY.png" title="Hosted by imgur.com" alt="" /></a>

#2. 日志文件相关代码

我相信你可以很快的在levelDB中定位到写记录的函数，也就是levelDB在db_impl.cc文件中的Write函数中写入记录

	Status DBImpl::Write(const WriteOptions& options, WriteBatch* my_batch);

写入记录的方式是先写入log文件，然后再写入memtable。我们只分析写log文件的情况。写入日志文件的语句如下：

	status = log_->AddRecord(WriteBatchInternal::Contents(updates));

`log_`的定义如下：

	log::Writer *log_;

Writer是一个类，封装了文件的操作，Writer的定义如下：

	class Writer {
	 public:
	  // Create a writer that will append data to "*dest".
	  // "*dest" must be initially empty.
	  // "*dest" must remain live while this Writer is in use.
	  explicit Writer(WritableFile* dest);
	  ~Writer();
	
	  Status AddRecord(const Slice& slice);
	
	 private:
	  WritableFile* dest_;
	  int block_offset_;       // Current offset in block
	
	  // crc32c values for all supported record types.  These are
	  // pre-computed to reduce the overhead of computing the crc of the
	  // record type stored in the header.
	  uint32_t type_crc_[kMaxRecordType + 1];
	
	  Status EmitPhysicalRecord(RecordType type, const char* ptr, size_t length);
	
	  // No copying allowed
	  Writer(const Writer&);
	  void operator=(const Writer&);
	};

可以看到Writer除了构造函数和析构函数，只有一个公有的成员函数：`AddRecord`
，还有一个私有的辅助成员函数`EmitPhysicalRecord`,
还有一个关键的成员变量：`WritableFile`
，我们来看看这两个成员函数都做了什么。


##2.1 AddRecord 

`AddRecord` 函数的定义如下：

	Status Writer::AddRecord(const Slice& slice) {
	  const char* ptr = slice.data();
	  size_t left = slice.size();
	
	  // Fragment the record if necessary and emit it.  Note that if slice
	  // is empty, we still want to iterate once to emit a single
	  // zero-length record
	  Status s;
	  bool begin = true;
	  do {
	    const int leftover = kBlockSize - block_offset_;
	    assert(leftover >= 0);
	    if (leftover < kHeaderSize) {
	      // Switch to a new block
	      if (leftover > 0) {
	        // Fill the trailer (literal below relies on kHeaderSize being 7)
	        assert(kHeaderSize == 7);
	        dest_->Append(Slice("\x00\x00\x00\x00\x00\x00", leftover));
	      }
	      block_offset_ = 0;
	    }
	
	    // Invariant: we never leave < kHeaderSize bytes in a block.
	    assert(kBlockSize - block_offset_ - kHeaderSize >= 0);
	
	    const size_t avail = kBlockSize - block_offset_ - kHeaderSize;
	    const size_t fragment_length = (left < avail) ? left : avail;
	
	    RecordType type;
	    const bool end = (left == fragment_length);
	    if (begin && end) {
	      type = kFullType;
	    } else if (begin) {
	      type = kFirstType;
	    } else if (end) {
	      type = kLastType;
	    } else {
	      type = kMiddleType;
	    }
	
	    s = EmitPhysicalRecord(type, ptr, fragment_length);
	    ptr += fragment_length;
	    left -= fragment_length;
	    begin = false;
	  } while (s.ok() && left > 0);
	  return s;
	}
	

`AddRecord`成员函数将数据按一定格式存储，然后调用`EmitPhysicalRecord`
成员函数来完成数据的写入。


#2.2 EmitPhysicalRecord

在`EmitPhysicalRecord`中，首先封装记录头，然后计算校验码，再将数据写入到`dest_`中。`dest_`是一个`WritableFile`对象，WritableFile类封装文件的读写操作。

	Status Writer::EmitPhysicalRecord(RecordType t, const char* ptr, size_t n) {
	  assert(n <= 0xffff);  // Must fit in two bytes
	  assert(block_offset_ + kHeaderSize + n <= kBlockSize);
	
	  // Format the header
	  char buf[kHeaderSize];
	  buf[4] = static_cast<char>(n & 0xff);
	  buf[5] = static_cast<char>(n >> 8);
	  buf[6] = static_cast<char>(t);
	
	  // Compute the crc of the record type and the payload.
	  uint32_t crc = crc32c::Extend(type_crc_[t], ptr, n);
	  crc = crc32c::Mask(crc);                 // Adjust for storage
	  EncodeFixed32(buf, crc);
	
	  // Write the header and the payload
	  Status s = dest_->Append(Slice(buf, kHeaderSize));
	  if (s.ok()) {
	    s = dest_->Append(Slice(ptr, n));
	    if (s.ok()) {
	      s = dest_->Flush();
	    }
	  }
	  block_offset_ += kHeaderSize + n;
	  return s;
	}

`AddRecord`和`EmitPhysicalRecord`两个函数只负责将log
数据编码成相应的格式，然后调用下面的语句写入数据。

	Status s = dest_->Append(Slice(buf, kHeaderSize));

你肯定和我一样好奇`dest_`是什么，成员变量`dest_`的定义如下：

	WritableFile* dest_;

它是一个`WritableFile`类型，而`WritableFile`
自己也是一个抽象类。WritableFile的定义如下：

	// A file abstraction for sequential writing.  The implementation
	// must provide buffering since callers may append small fragments
	// at a time to the file.
	class WritableFile {
	 public:
	  WritableFile() { }
	  virtual ~WritableFile();
	
	  virtual Status Append(const Slice& data) = 0;
	  virtual Status Close() = 0;
	  virtual Status Flush() = 0;
	  virtual Status Sync() = 0;
	
	 private:
	  // No copying allowed
	  WritableFile(const WritableFile&);
	  void operator=(const WritableFile&);
	};


所以，我们得看看`log::Writer`
的构造函数被调用的地方，才能知道`WritableFile *dest_`中的`dest_`到底是什么类型。

	log_ = new log::Writer(lfile);

`lfile` 的声明如下：

	WritableFile* lfile;

`lfile` 的定义如下：

    s = options.env->NewWritableFile(LogFileName(dbname, new_log_number),
                                     &lfile);


env_posix.cc文件中`NewWritableFile` 的定义如下：

	  virtual Status NewWritableFile(const std::string& fname,
	                                 WritableFile** result) {
	    Status s;
	    const int fd = open(fname.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0644);
	    if (fd < 0) {
	      *result = NULL;
	      s = IOError(fname, errno);
	    } else {
	      *result = new PosixMmapFile(fname, fd, page_size_);
	    }
	    return s;
	  }

我们先别管`PosixMmapFile` 了，好歹我们已经看到了下面这条语句，知道了`log::Writer`
打开了一个文件，进行写操作。

	    const int fd = open(fname.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0644);

log::Writer是对日志文件的封装，说到底也只是要写一个文件，我们经历了千辛万苦，终于找到了打开文件的操作，这又将我们引入了levelDB的另一片天地：对可移植性的处理。你可能还在纳闷为什么下面的语句会调用env_posix.cc文件中`PosixEnv`类的成员函数，那就一起来分析下`options.env`吧。

	s = options.env->NewWritableFile(LogFileName(dbname, new_log_number),
	                                     &lfile);

#3. Env

levelDB考虑到移植性问题，将系统相关的处理(文件/进程/时间之类)抽象成Env
，用户可以自己实现相应的接口，作为`Options`传入，默认使用自带的实现。

请仔细阅读上面这段话，然后我们来分析一下Env，Env的定义如下：

	class Env {
	 public:
	  Env() { }
	  virtual ~Env();
	
	  // Return a default environment suitable for the current operating
	  // system.  Sophisticated users may wish to provide their own Env
	  // implementation instead of relying on this default environment.
	  //
	  // The result of Default() belongs to leveldb and must never be deleted.
	  static Env* Default();
	
	  // Create a brand new sequentially-readable file with the specified name.
	  // On success, stores a pointer to the new file in *result and returns OK.
	  // On failure stores NULL in *result and returns non-OK.  If the file does
	  // not exist, returns a non-OK status.
	  //
	  // The returned file will only be accessed by one thread at a time.
	  virtual Status NewSequentialFile(const std::string& fname,
	                                   SequentialFile** result) = 0;
	
	  // Create a brand new random access read-only file with the
	  // specified name.  On success, stores a pointer to the new file in
	  // *result and returns OK.  On failure stores NULL in *result and
	  // returns non-OK.  If the file does not exist, returns a non-OK
	  // status.
	  //
	  // The returned file may be concurrently accessed by multiple threads.
	  virtual Status NewRandomAccessFile(const std::string& fname,
	                                     RandomAccessFile** result) = 0;
	
	  // Create an object that writes to a new file with the specified
	  // name.  Deletes any existing file with the same name and creates a
	  // new file.  On success, stores a pointer to the new file in
	  // *result and returns OK.  On failure stores NULL in *result and
	  // returns non-OK.
	  //
	  // The returned file will only be accessed by one thread at a time.
	  virtual Status NewWritableFile(const std::string& fname,
	                                 WritableFile** result) = 0;
	
	  // Returns true iff the named file exists.
	  virtual bool FileExists(const std::string& fname) = 0;
	
	  // Store in *result the names of the children of the specified directory.
	  // The names are relative to "dir".
	  // Original contents of *results are dropped.
	  virtual Status GetChildren(const std::string& dir,
	                             std::vector<std::string>* result) = 0;
	
	  // Delete the named file.
	  virtual Status DeleteFile(const std::string& fname) = 0;
	
	  // Create the specified directory.
	  virtual Status CreateDir(const std::string& dirname) = 0;
	
	  // Delete the specified directory.
	  virtual Status DeleteDir(const std::string& dirname) = 0;
	
	  // Store the size of fname in *file_size.
	  virtual Status GetFileSize(const std::string& fname, uint64_t* file_size) = 0;
	
	  // Rename file src to target.
	  virtual Status RenameFile(const std::string& src,
	                            const std::string& target) = 0;
	
	  // Lock the specified file.  Used to prevent concurrent access to
	  // the same db by multiple processes.  On failure, stores NULL in
	  // *lock and returns non-OK.
	  //
	  // On success, stores a pointer to the object that represents the
	  // acquired lock in *lock and returns OK.  The caller should call
	  // UnlockFile(*lock) to release the lock.  If the process exits,
	  // the lock will be automatically released.
	  //
	  // If somebody else already holds the lock, finishes immediately
	  // with a failure.  I.e., this call does not wait for existing locks
	  // to go away.
	  //
	  // May create the named file if it does not already exist.
	  virtual Status LockFile(const std::string& fname, FileLock** lock) = 0;
	
	  // Release the lock acquired by a previous successful call to LockFile.
	  // REQUIRES: lock was returned by a successful LockFile() call
	  // REQUIRES: lock has not already been unlocked.
	  virtual Status UnlockFile(FileLock* lock) = 0;
	
	  // Arrange to run "(*function)(arg)" once in a background thread.
	  //
	  // "function" may run in an unspecified thread.  Multiple functions
	  // added to the same Env may run concurrently in different threads.
	  // I.e., the caller may not assume that background work items are
	  // serialized.
	  virtual void Schedule(
	      void (*function)(void* arg),
	      void* arg) = 0;
	
	  // Start a new thread, invoking "function(arg)" within the new thread.
	  // When "function(arg)" returns, the thread will be destroyed.
	  virtual void StartThread(void (*function)(void* arg), void* arg) = 0;
	
	  // *path is set to a temporary directory that can be used for testing. It may
	  // or many not have just been created. The directory may or may not differ
	  // between runs of the same process, but subsequent calls will return the
	  // same directory.
	  virtual Status GetTestDirectory(std::string* path) = 0;
	
	  // Create and return a log file for storing informational messages.
	  virtual Status NewLogger(const std::string& fname, Logger** result) = 0;
	
	  // Returns the number of micro-seconds since some fixed point in time. Only
	  // useful for computing deltas of time.
	  virtual uint64_t NowMicros() = 0;
	
	  // Sleep/delay the thread for the perscribed number of micro-seconds.
	  virtual void SleepForMicroseconds(int micros) = 0;
	
	 private:
	  // No copying allowed
	  Env(const Env&);
	  void operator=(const Env&);
	};
	
Env是一个抽象类，它只定义了一些接口，为了知道我们使用的Env
是哪个类的对象，我们来看一下levelDB 在什么地方定义Env 的。


#4. PosixEnv

在option.cc 中可以看到，options 的默认构造函数如下：

	Options::Options()
	    : comparator(BytewiseComparator()),
	      create_if_missing(false),
	      error_if_exists(false),
	      paranoid_checks(false),
	      env(Env::Default()),
	      info_log(NULL),
	      write_buffer_size(4<<20),
	      max_open_files(1000),
	      block_cache(NULL),
	      block_size(4096),
	      block_restart_interval(16),
	      compression(kSnappyCompression),
	      filter_policy(NULL) {
	}
	
也就是说，我们没有指定Env时，系统会通过`env(Env::Default())`给Env
赋值，我们再来看看`Env::Default()`函数的定义：

	static pthread_once_t once = PTHREAD_ONCE_INIT;
	static Env* default_env;
	static void InitDefaultEnv() { default_env = new PosixEnv; }
	
	Env* Env::Default() {
	  pthread_once(&once, InitDefaultEnv);
	  return default_env;
	}
	
上面这几行代码位于env_posix.cc 的最后几行，其实也没做什么，只是返回一个PosixEnv对象，PosixEnv对象是Env
的子类，所以，下面这句话调用的是PosixEnv的成员函数。

	s = options.env->NewWritableFile(LogFileName(dbname, new_log_number),
		                                     &lfile);

#5. EnvWrapper

在levelDB中还实现了一个EnvWrapper类，该类继承自Env，且只有一个成员函数`Env* target_`，该类的所有变量都调用Env类相应的成员变量，我们知道，Env是一个抽象类，是不能定义Env 类型的对象的。我们传给EnvWrapper 的构造函数的类型是PosixEnv，所以，最后调用的都是PosixEnv类的成员变量，你可能已经猜到了，这就是设计模式中的代理模式，EnvWrapper只是进行了简单的封装，它的代理了Env的子类PosixEnv。

EnvWrapper和Env与PosixEnv的关系如下：

<a href="http://imgur.com/1WVgtlE"><img src="http://i.imgur.com/1WVgtlE.png" title="Hosted by imgur.com" alt="" /></a>

完。
