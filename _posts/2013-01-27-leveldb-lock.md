---
layout: post
title: "LevelDB源码剖析之确保系统中只有一个levelDB实例"
description: ""
category: NoSql
tags: [levelDB, lock]
---


如何保证应用程序只有一个实例？以前学MFC的时候就遇到过这个问题，当时只要简单的设置下参数就可以了，没有深究，现在研究levelDB又遇到这个问题，所以，我决定打破沙锅问到底，看看到底是怎么实现的！

如果你月薪一万，别人问你工资多少，你告诉别人你月薪一万，那没有什么特别的，谁也不会在意，如果你月薪一万，却告诉别人你月收入五位数，那，给对方的感觉是完全不一样的，尤其是相亲的时候:-)

我一直好奇如何保证一个应用程序只能有一个实例，当我知道了以后，反而有那么一点点失望，因为它就那么回事，真没有什么高深的东西，你不知道，只是因为你不知道，或者说是因为你知道得太少，而不是这东西太难了。废话真多，下面就来看一下如何保证应用程序只能有一个实例的吧。

直接把目光定位到db_impl.cc文件的下DB::Open函数里的这行代码：

	Status s = impl->Recover(&edit); // Handles create_if_missing, error_if_exists

Recover函数完成levelDB 启动时的一些基本检查，主要包括下面两部分：

1. 根据传入的db 路径，对LOCK 文件做flock 来判断是否已经有db
   实例启动，一份数据同时只能有一个db 实例操作。
2. 根据option 内的create_if_missing / error_if_exists
   来确定数据目录已经存在的时候要做的处理
 
下面是Recover函数的部分代码：

	  // Ignore error from CreateDir since the creation of the DB is
	  // committed only when the descriptor is created, and this directory
	  // may already exist from a previous failed creation attempt.
	  env_->CreateDir(dbname_);
	  assert(db_lock_ == NULL);
	  Status s = env_->LockFile(LockFileName(dbname_), &db_lock_);
	  if (!s.ok()) {
	    return s;
	  }
	
	  if (!env_->FileExists(CurrentFileName(dbname_))) {
	    if (options_.create_if_missing) {
	      s = NewDB();
	      if (!s.ok()) {
	        return s;
	      }
	    } else {
	      return Status::InvalidArgument(
	          dbname_, "does not exist (create_if_missing is false)");
	    }
	  } else {
	    if (options_.error_if_exists) {
	      return Status::InvalidArgument(
	          dbname_, "exists (error_if_exists is true)");
	    }
	  }
	
可以看到，它就做了我们上面说的两件事。我们主要来看看LockFile函数。在看LockFile
函数前，先把LockFileName 弄清楚了，它的定义如下：

	std::string LockFileName(const std::string& dbname) {
	  return dbname + "/LOCK";
	}

假如我们的数据库名子为testdb ，那么，这个函数的功能就是返回一个testdb/LOCK
字符串，用于我们定义testdb 目录下的LOCK
文件，LOCK文件是一个空文件，它的存在就是帮助实现一个应用程序只存在一个实例。

LockFile 是`env_`的成员函数，我们知道，`env_`是[一个指向PosixEnv][1]
的指针，所以，我们直接把注意力转移到PosixEnv 类的LockFile 文件即可。

	  virtual Status LockFile(const std::string& fname, FileLock** lock) {
	    *lock = NULL;
	    Status result;
	    int fd = open(fname.c_str(), O_RDWR | O_CREAT, 0644);
	    if (fd < 0) {
	      result = IOError(fname, errno);
	    } else if (!locks_.Insert(fname)) {
	      close(fd);
	      result = Status::IOError("lock " + fname, "already held by process");
	    } else if (LockOrUnlock(fd, true) == -1) {
	      result = IOError("lock " + fname, errno);
	      close(fd);
	      locks_.Remove(fname);
	    } else {
	      PosixFileLock* my_lock = new PosixFileLock;
	      my_lock->fd_ = fd;
	      my_lock->name_ = fname;
	      *lock = my_lock;
	    }
	    return result;
	  }


LockFile
首先打开文件，如果文件不存在，就创建，打开文件以后判断文件是否打开失败，如果打开失败，就返回错误信息，否则，就执行其他语句，我们把注意力集中到下面这条语句。

	} else if (LockOrUnlock(fd, true) == -1) {

顾名思意，它要锁住当前文件，怎么做到的呢？我们看看这个函数的定义就知道了。

	static int LockOrUnlock(int fd, bool lock) {
	  errno = 0;
	  struct flock f;
	  memset(&f, 0, sizeof(f));
	  f.l_type = (lock ? F_WRLCK : F_UNLCK);
	  f.l_whence = SEEK_SET;
	  f.l_start = 0;
	  f.l_len = 0;        // Lock/unlock entire file
	  return fcntl(fd, F_SETLK, &f);
	}

如果你看过《[Unix环境高级编程][2]》，你一定跟我一样，看到这里就笑了，然后心里默念：原来如此！！

我们只看关键的语句，从函数的名字就可以看出来，这个函数要实现对文件的加锁解锁功能，当文件已经被加锁了，我们就解锁，当文件没有锁的时候，我们就加锁。

	f.l_type = (lock ? F_WRLCK : F_UNLCK);

然后通过fcntl 系统调用来对文件加锁，当我们对文件加了F_WRLCK(独占锁)
锁以后，其他程序打开这个文件的时候，就会出错。

当我们有一个levelDB实例运行的时候，第二个levelDB实例在调用LockFile
函数企图打开LOCK文件时，由于第一个levelDB 实例已经将该文件加锁了，第二个levelDB实例打开LOCK文件时就会出错，出错以后就直接返回了。这样就实现了应用程序只有一个实例在运行。

	int fd = open(fname.c_str(), O_RDWR | O_CREAT, 0644);

有加锁操作，自然也有解锁操作了，在~DBImpl()函数中有这么一条语句：

	  if (db_lock_ != NULL) {
	    env_->UnlockFile(db_lock_);
	  }

对于以前不知道如何保证应用程序只能有一个实例的你，是不是有点惊讶，怎么这么简单，都是以前学过的知识！

[1]: http://mingxinglai.com/cn/2013/01/leveldb-log-and-env/
[2]: http://book.douban.com/subject/1788421/
