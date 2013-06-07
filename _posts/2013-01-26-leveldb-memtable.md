---
layout: post
title: "LevelDB源码剖析之MemTable"
description: ""
category: NoSql
tags: [levelDB, memtable]
---

#MemTable 

在levelDB中所有KV数据都是存储在Memtable，Immutable Memtable和SSTable中的，Immutable
Memtable从结构上讲和Memtable是完全一样的，区别仅仅在于其是只读的，不允许写入操作，而Memtable则是允许写入和读取的。当Memtable写入的数据占用内存到达指定数量，则自动转换为Immutable
Memtable，等待Dump到磁盘中，系统会自动生成新的Memtable供写操作写入新数据，理解了Memtable，那么Immutable
Memtable自然不在话下。

LevelDb的MemTable提供了将KV数据写入，删除以及读取KV记录的操作接口，但是事实上Memtable并不存在真正的删除操作，删除某个Key的Value在Memtable内是作为插入一条记录实施的，但是会打上一个Key的删除标记，真正的删除操作是Lazy的，会在以后的Compaction过程中去掉这个KV。
需要注意的是，LevelDb的Memtable中KV对是根据Key大小有序存储的，在系统插入新的KV时，LevelDb要把这个KV插到合适的位置上以保持这种Key有序性。其实，LevelDb的Memtable类只是一个接口类，真正的操作是通过背后的SkipList来做的，包括插入操作和读取操作等，所以Memtable的核心数据结构是一个SkipList。

#MemTable 声明

db数据在内存中的存储格式。写操作的数据都会先写到memtable中。memtable的size
有限制最大值(write_buffer_size)。memtable的实现是skiplist，当一个memtable
size达到阀值时，会变成只读的memtable(immutable memtable)，同时生成一个新的
memtable供新的写入。后台的compact进程会负责将immutable memtable dump
成sstable。所以，同时最多会存在两个memtable（正在写的memtable和immutable
memtable)。
	
	class MemTable {
	 public:
	  // MemTables are reference counted.  The initial reference count
	  // is zero and the caller must call Ref() at least once.
	  explicit MemTable(const InternalKeyComparator& comparator);
	
	  // Increase reference count.
	  void Ref() { ++refs_; }
	
	  // Drop reference count.  Delete if no more references exist.
	  void Unref() {
	    --refs_;
	    assert(refs_ >= 0);
	    if (refs_ <= 0) {
	      delete this;
	    }
	  }
	
	  // Returns an estimate of the number of bytes of data in use by this
	  // data structure.
	  //
	  // REQUIRES: external synchronization to prevent simultaneous
	  // operations on the same MemTable.
	  size_t ApproximateMemoryUsage();
	
	  // Return an iterator that yields the contents of the memtable.
	  //
	  // The caller must ensure that the underlying MemTable remains live
	  // while the returned iterator is live.  The keys returned by this
	  // iterator are internal keys encoded by AppendInternalKey in the
	  // db/format.{h,cc} module.
	  Iterator* NewIterator();
	
	  // Add an entry into memtable that maps key to value at the
	  // specified sequence number and with the specified type.
	  // Typically value will be empty if type==kTypeDeletion.
	  void Add(SequenceNumber seq, ValueType type,
	           const Slice& key,
	           const Slice& value);
	
	  // If memtable contains a value for key, store it in *value and return true.
	  // If memtable contains a deletion for key, store a NotFound() error
	  // in *status and return true.
	  // Else, return false.
	  bool Get(const LookupKey& key, std::string* value, Status* s);
	
	 private:
	  ~MemTable();  // Private since only Unref() should be used to delete it
	
	  struct KeyComparator {
	    const InternalKeyComparator comparator;
	    explicit KeyComparator(const InternalKeyComparator& c) : comparator(c) { }
	    int operator()(const char* a, const char* b) const;
	  };
	  friend class MemTableIterator;
	  friend class MemTableBackwardIterator;
	
	  typedef SkipList<const char*, KeyComparator> Table;
	
	  KeyComparator comparator_;
	  int refs_;
	  Arena arena_;
	  Table table_;
	
	  // No copying allowed
	  MemTable(const MemTable&);
	  void operator=(const MemTable&);
	};
	
MemTable只是一层封装，实际的数据结构是一个SkipList`typedef SkipList<const char*, KeyComparator> Table;`
MemTable有一个公有的构造函数，Ref()和unRef()
和一个返回迭代器的成员函数，除此之外，就只有3个公有的成员函数了，分别是：

1. size_t ApproximateMemoryUsage();
2. void Add(SequenceNumber seq, ValueType type, const Slice& key, const Slice& value);
3. bool Get(const LookupKey& key, std::string\* value, Status\* s);

其中，第一个成员函数只是简单的封装，最后调用的是Arena中的成员函数

	size_t MemTable::ApproximateMemoryUsage() { return arena_.MemoryUsage(); }

Arena是一个内存管理类，可以参考[这里][1]。

下面来分析另外两个成员函数：

* void Add(SequenceNumber seq, ValueType type, const Slice& key, const Slice& value);

		void MemTable::Add(SequenceNumber s, ValueType type,
		                   const Slice& key,
		                   const Slice& value) {
		  // Format of an entry is concatenation of:
		  //  key_size     : varint32 of internal_key.size()
		  //  key bytes    : char[internal_key.size()]
		  //  value_size   : varint32 of value.size()
		  //  value bytes  : char[value.size()]
		  size_t key_size = key.size();
		  size_t val_size = value.size();
		  size_t internal_key_size = key_size + 8;
		  const size_t encoded_len =
		      VarintLength(internal_key_size) + internal_key_size +
		      VarintLength(val_size) + val_size;
		  char* buf = arena_.Allocate(encoded_len);
		  char* p = EncodeVarint32(buf, internal_key_size);
		  memcpy(p, key.data(), key_size);
		  p += key_size;
		  EncodeFixed64(p, (s << 8) | type);
		  p += 8;
		  p = EncodeVarint32(p, val_size);
		  memcpy(p, value.data(), val_size);
		  assert((p + val_size) - buf == encoded_len);
		  table_.Insert(buf);
		}

MemTable::add只是简单的将SequenceNumber和ValueType
以及消息编码成一个字符串，存放在buf数组中，然后调用table.Insert(buf)插入数据。

这里要解释的是buf的内容：
它包括

<a href="http://imgur.com/OfabI12"><img src="http://i.imgur.com/OfabI12.png" title="Hosted by imgur.com" alt="" /></a>

其中，internal_key只是一个结构体，封装了key，SequenceNumber和Type;
因为SequenceNumber和Type一起存放在一个64位的整型里面，所以才有：

		internal_key_size = key.size + 8


该函数里的其他语句就是将internal_key_size和val.isze()编码成varint
，然后把数据依次存放到buf中。

* bool Get(const LookupKey& key, std::string\* value, Status\* s); 

		bool MemTable::Get(const LookupKey& key, std::string* value, Status* s) {
		  Slice memkey = key.memtable_key();
		  Table::Iterator iter(&table_);
		  iter.Seek(memkey.data());
		  if (iter.Valid()) {
		    // entry format is:
		    //    klength  varint32
		    //    userkey  char[klength]
		    //    tag      uint64
		    //    vlength  varint32
		    //    value    char[vlength]
		    // Check that it belongs to same user key.  We do not check the
		    // sequence number since the Seek() call above should have skipped
		    // all entries with overly large sequence numbers.
		    const char* entry = iter.key();
		    uint32_t key_length;
		    const char* key_ptr = GetVarint32Ptr(entry, entry+5, &key_length);
		    if (comparator_.comparator.user_comparator()->Compare(
		            Slice(key_ptr, key_length - 8),
		            key.user_key()) == 0) {
		      // Correct user key
		      const uint64_t tag = DecodeFixed64(key_ptr + key_length - 8);
		      switch (static_cast<ValueType>(tag & 0xff)) {
		        case kTypeValue: {
		          Slice v = GetLengthPrefixedSlice(key_ptr + key_length);
		          value->assign(v.data(), v.size());
		          return true;
		        }
		        case kTypeDeletion:
		          *s = Status::NotFound(Slice());
		          return true;
		      }
		    }
		  }
		  return false;
		}
	
理解MemTable 更多的是理解消息封装和编码，在MemTable::Get
函数中又出现了LookupKey ，我们暂时不要管他，先看函数体。
函数体通过在Table 中查找key,找到就返回一个迭代器，然后获取迭代器的值，解码得到信息。

通过下面的语句获取internal_key_size ，因为varint
最多不会超过5个字节，所以有起点是entry 终点是 entry +
5,具体怎么在一个字符串数组中获取一个int ，请参考[这里][2]。

    const char* key_ptr = GetVarint32Ptr(entry, entry+5, &key_length);


再次比较返回的key与user_key 是否相同，这里之所以会有 key_length - 8
是因为key_length包含了key.size()和SequenceNumber(8个字节,这8个字节里前7个字节是序列号，最后一个字节是消息类型)。

    if (comparator_.comparator.user_comparator()->Compare(
            Slice(key_ptr, key_length - 8),
            key.user_key()) == 0) {
      // Correct user key


对key后面的8个字节解码，然后获取这8个字节的最后一个字节，判断消息是什么类型：

      const uint64_t tag = DecodeFixed64(key_ptr + key_length - 8);
      switch (static_cast<ValueType>(tag & 0xff)) {
        case kTypeValue: {
          Slice v = GetLengthPrefixedSlice(key_ptr + key_length);
          value->assign(v.data(), v.size());
          return true;
        }
        case kTypeDeletion:
          *s = Status::NotFound(Slice());
          return true;
      }


如果消息不是kTypeDeletion类型，那么消息就是有效的，接下来就获取value的size,再获取value的值。

[1]: http://mingxinglai.com/cn/2013/01/leveldb-arena/
[2]: http://mingxinglai.com/cn/2013/01/leveldb-varint32/
