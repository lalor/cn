---
layout: post
title: "LevelDB源码剖析之Status"
description: ""
category: NoSql
tags: [levelDB, Status]
---

levelDB中的所有操作的返回值都用一个名为Status的类来存储, Status有什么特别的呢？


#Status 介绍

leveldb::Status表示levelDB的一个返回状态，通常的错误处理（如:errno）是返回一个错误号，然后根据错误号可以获得出错的描述信息。
leveldb将错误号和错误信息封装成Status类，来统一进行处理。 


#Status 声明

leveldb::Status 的声明如下：

	class Status {
	 public:
	  // Create a success status.
	  Status() : state_(NULL) { }
	  ~Status() { delete[] state_; }
	
	  // Copy the specified status.
	  Status(const Status& s);
	  void operator=(const Status& s);
	
	  // Return a success status.
	  static Status OK() { return Status(); }
	
	  // Return error status of an appropriate type.
	  static Status NotFound(const Slice& msg, const Slice& msg2 = Slice()) {
	    return Status(kNotFound, msg, msg2);
	  }
	  static Status Corruption(const Slice& msg, const Slice& msg2 = Slice()) {
	    return Status(kCorruption, msg, msg2);
	  }
	  static Status NotSupported(const Slice& msg, const Slice& msg2 = Slice()) {
	    return Status(kNotSupported, msg, msg2);
	  }
	  static Status InvalidArgument(const Slice& msg, const Slice& msg2 = Slice()) {
	    return Status(kInvalidArgument, msg, msg2);
	  }
	  static Status IOError(const Slice& msg, const Slice& msg2 = Slice()) {
	    return Status(kIOError, msg, msg2);
	  }
	
	  // Returns true iff the status indicates success.
	  bool ok() const { return (state_ == NULL); }
	
	  // Returns true iff the status indicates a NotFound error.
	  bool IsNotFound() const { return code() == kNotFound; }
	
	  // Returns true iff the status indicates a Corruption error.
	  bool IsCorruption() const { return code() == kCorruption; }
	
	  // Returns true iff the status indicates an IOError.
	  bool IsIOError() const { return code() == kIOError; }
	
	  // Return a string representation of this status suitable for printing.
	  // Returns the string "OK" for success.
	  std::string ToString() const;
	
	 private:
	  // OK status has a NULL state_.  Otherwise, state_ is a new[] array
	  // of the following form:
	  //    state_[0..3] == length of message
	  //    state_[4]    == code
	  //    state_[5..]  == message
	  const char* state_;
	
	  enum Code {
	    kOk = 0,
	    kNotFound = 1,
	    kCorruption = 2,
	    kNotSupported = 3,
	    kInvalidArgument = 4,
	    kIOError = 5
	  };
	
	  Code code() const {
	    return (state_ == NULL) ? kOk : static_cast<Code>(state_[4]);
	  }
	
	  Status(Code code, const Slice& msg, const Slice& msg2);
	  static const char* CopyState(const char* s);
	};
	
	inline Status::Status(const Status& s) {
	  state_ = (s.state_ == NULL) ? NULL : CopyState(s.state_);
	}
	inline void Status::operator=(const Status& s) {
	  // The following condition catches both aliasing (when this == &s),
	  // and the common case where both s and *this are ok.
	  if (state_ != s.state_) {
	    delete[] state_;
	    state_ = (s.state_ == NULL) ? NULL : CopyState(s.state_);
	  }
	}
	
	}  // namespace leveldb
	

#Status 定义

leveldb::Status 的定义如下：

	const char* Status::CopyState(const char* state) {
	  uint32_t size;
	  memcpy(&size, state, sizeof(size));
	  char* result = new char[size + 5];
	  memcpy(result, state, size + 5);
	  return result;
	}
	
	Status::Status(Code code, const Slice& msg, const Slice& msg2) {
	  assert(code != kOk);
	  const uint32_t len1 = msg.size();
	  const uint32_t len2 = msg2.size();
	  const uint32_t size = len1 + (len2 ? (2 + len2) : 0);
	  char* result = new char[size + 5];
	  memcpy(result, &size, sizeof(size));
	  result[4] = static_cast<char>(code);
	  memcpy(result + 5, msg.data(), len1);
	  if (len2) {
	    result[5 + len1] = ':';
	    result[6 + len1] = ' ';
	    memcpy(result + 7 + len1, msg2.data(), len2);
	  }
	  state_ = result;
	}
	
	std::string Status::ToString() const {
	  if (state_ == NULL) {
	    return "OK";
	  } else {
	    char tmp[30];
	    const char* type;
	    switch (code()) {
	      case kOk:
	        type = "OK";
	        break;
	      case kNotFound:
	        type = "NotFound: ";
	        break;
	      case kCorruption:
	        type = "Corruption: ";
	        break;
	      case kNotSupported:
	        type = "Not implemented: ";
	        break;
	      case kInvalidArgument:
	        type = "Invalid argument: ";
	        break;
	      case kIOError:
	        type = "IO error: ";
	        break;
	      default:
	        snprintf(tmp, sizeof(tmp), "Unknown code(%d): ",
	                 static_cast<int>(code()));
	        type = tmp;
	        break;
	    }
	    std::string result(type);
	    uint32_t length;
	    memcpy(&length, state_, sizeof(length));
	    result.append(state_ + 5, length);
	    return result;
	  }
	}
	

#Status 解析

可以看到Status就只有一个成员变量`const char *state_`，是一个指向字符串的指针，对该字符串进行了封装：

	// OK status has a NULL state_.  Otherwise, state_ is a new[] array
	// of the following form:
	//    state_[0..3] == length of message
	//    state_[4]    == code
	//    state_[5..]  == message
	const char* state_;

对Status 定义了一堆的静态成员函数，通过调用这些静态成员函数来构造Status的对象，Status 的使用方法如下：

	Status WriteBatch::Iterate(Handler* handler) const {
	  Slice input(rep_);
	  if (input.size() < kHeader) {
	    return Status::Corruption("malformed WriteBatch (too small)");
	  }
	
	  input.remove_prefix(kHeader);
	  Slice key, value;
	  int found = 0;
	  while (!input.empty()) {
	    found++;
	    char tag = input[0];
	    input.remove_prefix(1);
	    switch (tag) {
	      case kTypeValue:
	        if (GetLengthPrefixedSlice(&input, &key) &&
	            GetLengthPrefixedSlice(&input, &value)) {
	          handler->Put(key, value);
	        } else {
	          return Status::Corruption("bad WriteBatch Put");
	        }
	        break;
	      case kTypeDeletion:
	        if (GetLengthPrefixedSlice(&input, &key)) {
	          handler->Delete(key);
	        } else {
	          return Status::Corruption("bad WriteBatch Delete");
	        }
	        break;
	      default:
	        return Status::Corruption("unknown WriteBatch tag");
	    }
	  }
	  if (found != WriteBatchInternal::Count(this)) {
	    return Status::Corruption("WriteBatch has wrong count");
	  } else {
	    return Status::OK();
	  }
	}

Status中通过静态成员函数构造对象，通过ToString()成员函数来获取错误消息。

	leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
	if (!status.ok())
	{
		cerr << status.ToString() << endl;
	}


#从Status中学到如下知识

1. 使用enum 来对错误信息编码
2. 将消息长度，消息类型和消息一起编码到一个char \*数组，使用memcpy
   复制数组，加快程序的运行速度
3. sprintf不安全，请用snprintf
4. Slice::compare 比较两个已知长度的字符串，依然使用memcmp
5. 将赋值构造函数和拷贝构造函数的共同部分定义成一个辅助函数
6. 通过静态成员函数来返回对象
