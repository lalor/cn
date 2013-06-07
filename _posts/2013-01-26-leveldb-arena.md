---
layout: post
title: "LevelDB源码剖析之Arena内存管理"
description: ""
category: NoSql
tags: [levelDB, Arena]
---

#levelDB 中的内存管理

本文将分析levelDB 中内存管理类Arena
的实现，通过分析该类的实现，我学到了如何封装内存的分配操作(通过统一的接口来分配不同大小的内存，而不用考虑内存释放)，以及如何预先分配一整块内存来解决频繁分配小块内存浪费时间，直接分配大块内存浪费内存的问题，还学到了如何保证内存对齐。真好。

	static const int kBlockSize = 4096;

Arena每次按kBlockSize(`static const int kBlockSize = 4096;`)单位向系统申请内存，提供地址对齐的内存，记录内存使用。当memtable
申请内存时，如果size不大于kBlockSize的四分之一，就在当前空闲的内存block
中分配，否则，直接向系统申请。这个策略是为了更好的服务小内存的申请，避免个别大内存使用影响。

Arena类的声明如下：

	class Arena {
	 public:
	  Arena();
	  ~Arena();
	
	  // Return a pointer to a newly allocated memory block of "bytes" bytes.
	  char* Allocate(size_t bytes);
	
	  // Allocate memory with the normal alignment guarantees provided by malloc
	  char* AllocateAligned(size_t bytes);
	
	  // Returns an estimate of the total memory usage of data allocated
	  // by the arena (including space allocated but not yet used for user
	  // allocations).
	  size_t MemoryUsage() const {
	    return blocks_memory_ + blocks_.capacity() * sizeof(char*);
	  }
	
	 private:
	  char* AllocateFallback(size_t bytes);
	  char* AllocateNewBlock(size_t block_bytes);
	
	  // Allocation state
	  char* alloc_ptr_;
	  size_t alloc_bytes_remaining_;
	
	  // Array of new[] allocated memory blocks
	  std::vector<char*> blocks_;
	
	  // Bytes of memory in blocks allocated so far
	  size_t blocks_memory_;
	
	  // No copying allowed
	  Arena(const Arena&);
	  void operator=(const Arena&);
	};
	
上面就是Arena类的申明，可以看到，除了构造函数和析构函数，只有3个公有的成员函数，也就是说，使用者只能使用这3个函数(Allocate, allocatedAligned, MemoryUsage())，此外，Arena 类还有两个私有的成员函数和4个成员变量，我们分别来看看它们的用法。

Arena的关键成员函数就是一个`vector<char*> blocks_` 向量，该向量存放若干个指针，每个指针指向一块内存，结构图如下：

<a href="http://imgur.com/ZB75F7t"><img src="http://i.imgur.com/ZB75F7t.png" title="Hosted by imgur.com" alt="" /></a>

成员变量blocks_memory用于记录内存的使用情况，所以，MemoryUsage()
函数的定义如下：

	  // Returns an estimate of the total memory usage of data allocated
	  // by the arena (including space allocated but not yet used for user
	  // allocations).
	  size_t MemoryUsage() const {
	    return blocks_memory_ + blocks_.capacity() * sizeof(char*);
	  }

我们先不管AllocateAligned()函数，那么，Arena的使用者可以调用的成员函数只有Allocate()了，我们来看看它是怎么定义的：

	inline char* Arena::Allocate(size_t bytes) {
	  // The semantics of what to return are a bit messy if we allow
	  // 0-byte allocations, so we disallow them here (we don't need
	  // them for our internal use).
	  assert(bytes > 0);
	  if (bytes <= alloc_bytes_remaining_) {
	    char* result = alloc_ptr_;
	    alloc_ptr_ += bytes;
	    alloc_bytes_remaining_ -= bytes;
	    return result;
	  }
	  return AllocateFallback(bytes);
	}

该函数返回bytes字节的内存，如果预先分配的内存(`alloc_bytes_remaining_`)大于bytes,也就是说，预先分配的内存能够满足现在的需求，那就返回预先分配的内存，而不用向系统申请，如果(`alloc_bytes_remaining_ < bytes`)，也就是说，预先分配的内存不能不够用，那就调用AllocateFallback来分配内存，AllocateFallback的实现如下：

	char* Arena::AllocateFallback(size_t bytes) {
	  if (bytes > kBlockSize / 4) {
	    // Object is more than a quarter of our block size.  Allocate it separately
	    // to avoid wasting too much space in leftover bytes.
	    char* result = AllocateNewBlock(bytes);
	    return result;
	  }
	
	  // We waste the remaining space in the current block.
	  alloc_ptr_ = AllocateNewBlock(kBlockSize);
	  alloc_bytes_remaining_ = kBlockSize;
	
	  char* result = alloc_ptr_;
	  alloc_ptr_ += bytes;
	  alloc_bytes_remaining_ -= bytes;
	  return result;
	}

可以看到，AllocateFallback里有两种情况，一种是`bytes > kBlockSize / 4`的情况，另一种是`bytes <= kBlockSize / 4`
的情况，对这两种情况的处理是不一样的。两种情况都会调用AllocateNewBlock()函数，我们先来看一下它到底做了什么：

	char* Arena::AllocateNewBlock(size_t block_bytes) {
	  char* result = new char[block_bytes];
	  blocks_memory_ += block_bytes;
	  blocks_.push_back(result);
	  return result;
	}
	
这个函数只是调用new分配内存,然后将指针存放到`vector<char *> blocks_`容器中，并记录到目前为止共分配的内存(`blocks_memory_  += block_bytes`)的字节数，我们再来看AllocateFallback 中的两种情况：

第一种情况，直接调用AllocateNewBlock
分配bytes 字节的内存，没有多分配，第二种情况调用AllocateNewBlock
分配kBlockSize字节的内存，然后保留未使用的部分，留着下次使用，这里需要注意：

在Allocate()中，对于`bytes > alloc_bytes_remaining_ `的情况，我们需要调用AllocateFallback ，但是，在AllocateFallback 中的`bytes <  kBlockSize / 4 `的情况，我们直接分配了一块，令alloc_ptr(该指针指向预先分配，但未使用的内存)指向新分配的块，也就是说，有一部分内存（`alloc_bytes_remaining_`字节)被我们浪费了。

下面再来看看Arena的另一个成员函数AllocateAligned()，看它是如何保证分配的内存是对齐的，AllocateAligned函数的定义如下：

	char* Arena::AllocateAligned(size_t bytes) {
	  const int align = sizeof(void*);    // We'll align to pointer size
	  assert((align & (align-1)) == 0);   // Pointer size should be a power of 2
	  size_t current_mod = reinterpret_cast<uintptr_t>(alloc_ptr_) & (align-1);
	  size_t slop = (current_mod == 0 ? 0 : align - current_mod);
	  size_t needed = bytes + slop;
	  char* result;
	  if (needed <= alloc_bytes_remaining_) {
	    result = alloc_ptr_ + slop;
	    alloc_ptr_ += needed;
	    alloc_bytes_remaining_ -= needed;
	  } else {
	    // AllocateFallback always returned aligned memory
	    result = AllocateFallback(bytes);
	  }
	  assert((reinterpret_cast<uintptr_t>(result) & (align-1)) == 0);
	  return result;
	}


首先获取一个指针的大小`const int align = sizeof(void*)`，很明显，在32位系统下是4 ,64位系统下是8 ，为了表述方便，我们假设是32位系统，即align ＝ 4, 然后将我们使用的char * 指针地址转换为一个无符号整型(reinterpret_cast<[uintptr_t][1]>(result):It is an unsigned int that is guaranteed to be the same size as a pointer.)，通过与操作来获取`size_t current_mod = reinterpret_cast<uintptr_t>(alloc_ptr_) & (align-1);`当前指针模4的值，有了这个值以后，我们就容易知道，还差 `slop = align - current_mod`多个字节，内存才是对其的，所以有了`result = alloc_ptr + slop`

最后再看一下Arena的析构函数和构造函数：

	Arena::Arena() {
	  blocks_memory_ = 0;
	  alloc_ptr_ = NULL;  // First allocation will allocate a block
	  alloc_bytes_remaining_ = 0;
	}
	
	Arena::~Arena() {
	  for (size_t i = 0; i < blocks_.size(); i++) {
	    delete[] blocks_[i];
	  }
	}

没什么好解释的吧，只是简单的初始化和清理内存的工作。

[1]: http://stackoverflow.com/questions/1845482/what-is-uintptr-t-data-type
