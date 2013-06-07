---
layout: post
title: "LevelDB源码剖析之Cache缓冲区与hash表"
description: ""
category: NoSql
tags: [levelDB, Cache, hash]
---

#Cache

levelDB实现了自己的缓冲区替换算法，Cache的使用如下：

	 Cache* cache_;//声明
	
	  CacheTest() : cache_(NewLRUCache(kCacheSize)) {//定义
	    current_ = this; 
	  }
	
	  ~CacheTest() {
	    delete cache_;//删除
	  }
	
	  int Lookup(int key) {//查找
	    Cache::Handle* handle = cache_->Lookup(EncodeKey(key));
	    const int r = (handle == NULL) ? -1 : DecodeValue(cache_->Value(handle));
	    if (handle != NULL) {
	      cache_->Release(handle);
	    }
	    return r;
	  }
	
	  void Insert(int key, int value, int charge = 1) {//插入
	    cache_->Release(cache_->Insert(EncodeKey(key), EncodeValue(value), charge,
	                                   &CacheTest::Deleter));
	  }
	
	  void Erase(int key) {//删除
	    cache_->Erase(EncodeKey(key));
	  }

Cache的定义如下：

	class Cache {
	 public:
	  Cache() { }
	
	  // Destroys all existing entries by calling the "deleter"
	  // function that was passed to the constructor.
	  virtual ~Cache();
	
	  // Opaque handle to an entry stored in the cache.
	  struct Handle { };
	
	  // Insert a mapping from key->value into the cache and assign it
	  // the specified charge against the total cache capacity.
	  //
	  // Returns a handle that corresponds to the mapping.  The caller
	  // must call this->Release(handle) when the returned mapping is no
	  // longer needed.
	  //
	  // When the inserted entry is no longer needed, the key and
	  // value will be passed to "deleter".
	  virtual Handle* Insert(const Slice& key, void* value, size_t charge,
	                         void (*deleter)(const Slice& key, void* value)) = 0;
	
	  // If the cache has no mapping for "key", returns NULL.
	  //
	  // Else return a handle that corresponds to the mapping.  The caller
	  // must call this->Release(handle) when the returned mapping is no
	  // longer needed.
	  virtual Handle* Lookup(const Slice& key) = 0;
	
	  // Release a mapping returned by a previous Lookup().
	  // REQUIRES: handle must not have been released yet.
	  // REQUIRES: handle must have been returned by a method on *this.
	  virtual void Release(Handle* handle) = 0;
	
	  // Return the value encapsulated in a handle returned by a
	  // successful Lookup().
	  // REQUIRES: handle must not have been released yet.
	  // REQUIRES: handle must have been returned by a method on *this.
	  virtual void* Value(Handle* handle) = 0;
	
	  // If the cache contains entry for key, erase it.  Note that the
	  // underlying entry will be kept around until all existing handles
	  // to it have been released.
	  virtual void Erase(const Slice& key) = 0;
	
	  // Return a new numeric id.  May be used by multiple clients who are
	  // sharing the same cache to partition the key space.  Typically the
	  // client will allocate a new id at startup and prepend the id to
	  // its cache keys.
	  virtual uint64_t NewId() = 0;
	
	 private:
	  void LRU_Remove(Handle* e);
	  void LRU_Append(Handle* e);
	  void Unref(Handle* e);
	
	  struct Rep;
	  Rep* rep_;
	
	  // No copying allowed
	  Cache(const Cache&);
	  void operator=(const Cache&);
	};

从levelDB的源代码看到，Cache
是一个抽象类，我们是不能定义一个抽象类的对象的，通过NewLRUCache
函数我们可以看到，定义的是一个SharedLRUCache 对象。

	Cache* NewLRUCache(size_t capacity) {
	  return new SharedLRUCache(capacity);
	}


#SharedLRUCache 

SharedLRUCache到底是什么呢？我们为什么需要它？这是因为levelDB是多线程的，每个线程访问缓冲区的时候都会将缓冲区锁住，为了多线程访问，尽可能快速，减少锁开销，ShardedLRUCache内部有16个LRUCache，查找Key时首先计算key属于哪一个分片，分片的计算方法是取32位hash值的高4位，然后在相应的LRUCache中进行查找，这样就大大减少了多线程的访问锁的开销。 

	static const int kNumShardBits = 4;
	static const int kNumShards = 1 << kNumShardBits;
	
	class ShardedLRUCache : public Cache {
	 private:
	  LRUCache shard_[kNumShards];
	  port::Mutex id_mutex_;
	  uint64_t last_id_;
	
	  static inline uint32_t HashSlice(const Slice& s) {
	    return Hash(s.data(), s.size(), 0);
	  }
	
	  static uint32_t Shard(uint32_t hash) {
	    return hash >> (32 - kNumShardBits);
	  }
	
	 public:
	  explicit ShardedLRUCache(size_t capacity)
	      : last_id_(0) {
	    const size_t per_shard = (capacity + (kNumShards - 1)) / kNumShards;
	    for (int s = 0; s < kNumShards; s++) {
	      shard_[s].SetCapacity(per_shard);
	    }
	  }
	  virtual ~ShardedLRUCache() { }
	  virtual Handle* Insert(const Slice& key, void* value, size_t charge,
	                         void (*deleter)(const Slice& key, void* value)) {
	    const uint32_t hash = HashSlice(key);
	    return shard_[Shard(hash)].Insert(key, hash, value, charge, deleter);
	  }
	  virtual Handle* Lookup(const Slice& key) {
	    const uint32_t hash = HashSlice(key);
	    return shard_[Shard(hash)].Lookup(key, hash);
	  }
	  virtual void Release(Handle* handle) {
	    LRUHandle* h = reinterpret_cast<LRUHandle*>(handle);
	    shard_[Shard(h->hash)].Release(handle);
	  }
	  virtual void Erase(const Slice& key) {
	    const uint32_t hash = HashSlice(key);
	    shard_[Shard(hash)].Erase(key, hash);
	  }
	  virtual void* Value(Handle* handle) {
	    return reinterpret_cast<LRUHandle*>(handle)->value;
	  }
	  virtual uint64_t NewId() {
	    MutexLock l(&id_mutex_);
	    return ++(last_id_);
	  }
	};
	
	}  // end anonymous namespace
	

我们来看一下SharedLRUCache类，该类的关键成员变量`LRUCache
shard_[kNumShards]`是一个数组，数组每个成员是一个LRUCache
，这才是真正的缓冲区。

SharedLRUCache 只做一件事，就是计算Hash 值，选择LRUCache ，代码如下：

	static inline uint32_t HashSlice(const Slice& s) {
	  return Hash(s.data(), s.size(), 0);
	}
	
	static uint32_t Shard(uint32_t hash) {
	  return hash >> (32 - kNumShardBits);
	}


#hash函数

计算Hash值的算法看起来很简单，可以直接拿来用：

	uint32_t Hash(const char* data, size_t n, uint32_t seed) {
	  // Similar to murmur hash
	  const uint32_t m = 0xc6a4a793;
	  const uint32_t r = 24;
	  const char* limit = data + n;
	  uint32_t h = seed ^ (n * m);
	
	  // Pick up four bytes at a time
	  while (data + 4 <= limit) {
	    uint32_t w = DecodeFixed32(data);
	    data += 4;
	    h += w;
	    h *= m;
	    h ^= (h >> 16);
	  }
	
	  // Pick up remaining bytes
	  switch (limit - data) {
	    case 3:
	      h += data[2] << 16;
	      // fall through
	    case 2:
	      h += data[1] << 8;
	      // fall through
	    case 1:
	      h += data[0];
	      h *= m;
	      h ^= (h >> r);
	      break;
	  }
	  return h;
	}
	

我们现在来看SharedLRUCache
类的完整定义，再次强调，它的主要作用就是计算hash值，选择LRUCache
，然后调用LRUCache 的函数即可。代码如下：

	class ShardedLRUCache : public Cache {
	 private:
	  LRUCache shard_[kNumShards];
	  port::Mutex id_mutex_;
	  uint64_t last_id_;
	
	  static inline uint32_t HashSlice(const Slice& s) {
	    return Hash(s.data(), s.size(), 0);
	  }
	
	  static uint32_t Shard(uint32_t hash) {
	    return hash >> (32 - kNumShardBits);
	  }
	
	 public:
	  explicit ShardedLRUCache(size_t capacity)
	      : last_id_(0) {
	    const size_t per_shard = (capacity + (kNumShards - 1)) / kNumShards;
	    for (int s = 0; s < kNumShards; s++) {
	      shard_[s].SetCapacity(per_shard);
	    }
	  }
	  virtual ~ShardedLRUCache() { }
	  virtual Handle* Insert(const Slice& key, void* value, size_t charge,
	                         void (*deleter)(const Slice& key, void* value)) {
	    const uint32_t hash = HashSlice(key);
	    return shard_[Shard(hash)].Insert(key, hash, value, charge, deleter);
	  }
	  virtual Handle* Lookup(const Slice& key) {
	    const uint32_t hash = HashSlice(key);
	    return shard_[Shard(hash)].Lookup(key, hash);
	  }
	  virtual void Release(Handle* handle) {
	    LRUHandle* h = reinterpret_cast<LRUHandle*>(handle);
	    shard_[Shard(h->hash)].Release(handle);
	  }
	  virtual void Erase(const Slice& key) {
	    const uint32_t hash = HashSlice(key);
	    shard_[Shard(hash)].Erase(key, hash);
	  }
	  virtual void* Value(Handle* handle) {
	    return reinterpret_cast<LRUHandle*>(handle)->value;
	  }
	  virtual uint64_t NewId() {
	    MutexLock l(&id_mutex_);
	    return ++(last_id_);
	  }
	};
	

#LRUCache

我们再来看看LRUCache是怎么实现的，它的定义如下：

	// A single shard of sharded cache.
	class LRUCache {
	 public:
	  LRUCache();
	  ~LRUCache();
	
	  // Separate from constructor so caller can easily make an array of LRUCache
	  void SetCapacity(size_t capacity) { capacity_ = capacity; }
	
	  // Like Cache methods, but with an extra "hash" parameter.
	  Cache::Handle* Insert(const Slice& key, uint32_t hash,
	                        void* value, size_t charge,
	                        void (*deleter)(const Slice& key, void* value));
	  Cache::Handle* Lookup(const Slice& key, uint32_t hash);
	  void Release(Cache::Handle* handle);
	  void Erase(const Slice& key, uint32_t hash);
	
	 private:
	  void LRU_Remove(LRUHandle* e);
	  void LRU_Append(LRUHandle* e);
	  void Unref(LRUHandle* e);
	
	  // Initialized before use.
	  size_t capacity_;
	
	  // mutex_ protects the following state.
	  port::Mutex mutex_;
	  size_t usage_;
	  uint64_t last_id_;
	
	  // Dummy head of LRU list.
	  // lru.prev is newest entry, lru.next is oldest entry.
	  LRUHandle lru_;
	
	  HandleTable table_;
	};
	

`capacity_`是当前缓冲区的容量，`usage_`是已经使用的缓冲区空间，如果`usage_ >
capacity_`那就要选择一页驱逐出去了。

LRUCache
有两个关键的成员函数，分别是`lru_`和`table_`，`lru_`是`LRUHandle`类型的节点，该节点是一个傀儡节点，傀儡节点便于我们操作双向链表，也就是说，LRUCache 中的没一个元素，都是一个`LRUHandle`类型的对象，`table_`是hash 表，便于快速判断数据是否在缓冲区中，hash 表中的元素是`LRUHandle *`。LRUHandle 的定义如下：

	// An entry is a variable length heap-allocated structure.  Entries
	// are kept in a circular doubly linked list ordered by access time.
	struct LRUHandle {
	  void* value;
	  void (*deleter)(const Slice&, void* value);
	  LRUHandle* next_hash;
	  LRUHandle* next;
	  LRUHandle* prev;
	  size_t charge;      // TODO(opt): Only allow uint32_t?
	  size_t key_length;
	  uint32_t refs;
	  uint32_t hash;      // Hash of key(); used for fast sharding and comparisons
	  char key_data[1];   // Beginning of key
	
	  Slice key() const {
	    // For cheaper lookups, we allow a temporary Handle object
	    // to store a pointer to a key in "value".
	    if (next == this) {
	      return *(reinterpret_cast<Slice*>(value));
	    } else {
	      return Slice(key_data, key_length);
	    }
	  }
	};


还没弄懂charge和 key_data[1]的作用。


#图示

让我们暂停一下，先理清他们的关系。使用者再使用的时候，定义一个`Cache*`
指针，Cache类是一个抽象类，是不能定义这种类型的变量的，所以调用`NewLRUCache`函数返回一个SharedLRUCache对象，SharedLRUCache 定义了一个LRUCache 数组，这才是真正的缓冲区，也就是说，levelDB 将Cache 进行了封装，它的内部，其实有16个缓冲区，每个缓冲区都有独立的LRU 链表和Hash 表，便于查找，替换。

LRUCache中维护了一个双向链表，链表的元素类型为LRUHandle
，文字描述就这样，图示如下：

<a href="http://imgur.com/Gtnn06N"><img src="http://i.imgur.com/Gtnn06N.jpg" title="Hosted by imgur.com" alt="" /></a>


#hash表

hash表的实现比较简单，也比较优美，其中，length是指hash表桶的数量，elems
是元素的个数，当元素的个数大于桶的数量，就重新hash ，这样的话，hash
的平均查找代价为O(1)。

	class HandleTable {
	 public:
	  HandleTable() : length_(0), elems_(0), list_(NULL) { Resize(); }
	  ~HandleTable() { delete[] list_; }
	
	  LRUHandle* Lookup(const Slice& key, uint32_t hash) {
	    return *FindPointer(key, hash);
	  }
	
	  LRUHandle* Insert(LRUHandle* h) {
	    LRUHandle** ptr = FindPointer(h->key(), h->hash);
	    LRUHandle* old = *ptr;
	    h->next_hash = (old == NULL ? NULL : old->next_hash);
	    *ptr = h;
	    if (old == NULL) {
	      ++elems_;
	      if (elems_ > length_) {
	        // Since each cache entry is fairly large, we aim for a small
	        // average linked list length (<= 1).
	        Resize();
	      }
	    }
	    return old;
	  }
	
	  LRUHandle* Remove(const Slice& key, uint32_t hash) {
	    LRUHandle** ptr = FindPointer(key, hash);
	    LRUHandle* result = *ptr;
	    if (result != NULL) {
	      *ptr = result->next_hash;
	      --elems_;
	    }
	    return result;
	  }
	
	 private:
	  // The table consists of an array of buckets where each bucket is
	  // a linked list of cache entries that hash into the bucket.
	  uint32_t length_;
	  uint32_t elems_;
	  LRUHandle** list_;
	
	  // Return a pointer to slot that points to a cache entry that
	  // matches key/hash.  If there is no such cache entry, return a
	  // pointer to the trailing slot in the corresponding linked list.
	  LRUHandle** FindPointer(const Slice& key, uint32_t hash) {
	    LRUHandle** ptr = &list_[hash & (length_ - 1)];
	    while (*ptr != NULL &&
	           ((*ptr)->hash != hash || key != (*ptr)->key())) {
	      ptr = &(*ptr)->next_hash;
	    }
	    return ptr;
	  }
	
	  void Resize() {
	    uint32_t new_length = 4;
	    while (new_length < elems_) {
	      new_length *= 2;
	    }
	    LRUHandle** new_list = new LRUHandle*[new_length];
	    memset(new_list, 0, sizeof(new_list[0]) * new_length);
	    uint32_t count = 0;
	    for (uint32_t i = 0; i < length_; i++) {
	      LRUHandle* h = list_[i];
	      while (h != NULL) {
	        LRUHandle* next = h->next_hash;
	        Slice key = h->key();
	        uint32_t hash = h->hash;
	        LRUHandle** ptr = &new_list[hash & (new_length - 1)];
	        h->next_hash = *ptr;
	        *ptr = h;
	        h = next;
	        count++;
	      }
	    }
	    assert(elems_ == count);
	    delete[] list_;
	    list_ = new_list;
	    length_ = new_length;
	  }
	};
	

刚开始我很难理解如何在Hash表查找代价较大的时候重新hash, 其实就是判断记录hash
表的长度和元素个数，这样就能知道平均查找代价，当平均查找代价比较高的时候，就重新hash 。

理解得还不够透彻，有几个地方还有疑问，但是我从阅读levelDB
源代码中学到了如下知识：

#技巧

* 对一个2的指数求余
		LRUHandle** ptr = &new_list[hash & (new_length - 1)];
* 变长的Hash Table
	cache.cc 文件中的void Resize();
* SharedLRUCache 中用最高的4位来做桶hash
* HASH 值的计算(Similar to mrumru hash)
	hash.cc文件中的Hash


