---
layout: post
title: "LevelDB源码剖析之Varint"
description: ""
category: NoSql
tags: [levelDB, Varint]
---

#Varint

想知道牛人都是怎么节省空间的吗？想知道如何将整型编码成变长整型吗？存储为变长整型？那你如何解码呢？你怎么知道变长整型到底有几位呢？下面就来分析一下levelDB中如何将int存储为varint，尽可能的减少存储空间吧。

Varint是一种紧凑的表示数字的方法。它用一个或多个字节来表示一个数字，值越小的数字使用越少的字节数。这能减少用来表示数字的字节数。比如对于int32类型的数字，一般需要4个byte来表示。但是采用Varint，对于很小的int32类型的数字，则可以用1个byte来表示。当然凡事都有好的也有不好的一面，采用Varint表示法，大的数字则需要5个byte来表示。从统计的角度来说，一般不会所有的消息中的数字都是大数，因此大多数情况下，采用Varint 后，可以用更少的字节数来表示数字信息。下面就详细介绍一下 Varint。


Varint中的每个byte的最高位bit有特殊的含义，如果该位为
1，表示后续的byte也是该数字的一部分，如果该位为0，则结束。其他的7
个bit都用来表示数字。因此小于128的数字都可以用一个byte表示。大于
128的数字，比如300，会用两个字节来表示：1010 1100 0000 0010

#Varint 的编码

正常情况下，int需要32位，varint用一个字节的最高为做标识位，所以，一个字节只能存储7位，如果整数特别大，可能需要5个字节才能存放`{5 * 8 - 5(标识位) > 32}`，下面的if语句有5个分支，正好对应varint占用1到5个字节的情况。

	char* EncodeVarint32(char* dst, uint32_t v) {
	  // Operate on characters as unsigneds
	  unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
	  static const int B = 128;
	  if (v < (1<<7)) {
	    *(ptr++) = v;
	  } else if (v < (1<<14)) {
	    *(ptr++) = v | B;
	    *(ptr++) = v>>7;
	  } else if (v < (1<<21)) {
	    *(ptr++) = v | B;
	    *(ptr++) = (v>>7) | B;
	    *(ptr++) = v>>14;
	  } else if (v < (1<<28)) {
	    *(ptr++) = v | B;
	    *(ptr++) = (v>>7) | B;
	    *(ptr++) = (v>>14) | B;
	    *(ptr++) = v>>21;
	  } else {
	    *(ptr++) = v | B;
	    *(ptr++) = (v>>7) | B;
	    *(ptr++) = (v>>14) | B;
	    *(ptr++) = (v>>21) | B;
	    *(ptr++) = v>>28;
	  }
	  return reinterpret_cast<char*>(ptr);
	}



#Varint 解码

理解了编码的原理，再来看解码就很轻松了，直接调用GetVarint32Ptr
函数，该函数处理value < 128的情况，即varint只占一个字节的情况，对于varint 大于一个字节的情况，GetVarint32Ptr调用GetVarint32PtrFallback来处理。

	inline const char* GetVarint32Ptr(const char* p,
	                                  const char* limit,
	                                  uint32_t* value) {
	  if (p < limit) {
	    uint32_t result = *(reinterpret_cast<const unsigned char*>(p));
	    if ((result & 128) == 0) {
	      *value = result;
	      return p + 1;
	    }
	  }
	  return GetVarint32PtrFallback(p, limit, value);
	}


在GetVarint32Ptr和GetVarint32PtrFallback函数中，参数p
是指向一个包含varint的字符串，limit在调用的时候都是赋值为limit= p + 5,
这是因为varint最多占用5个字节。value用于存储返回的int值。

	const char* GetVarint32PtrFallback(const char* p,
	                                   const char* limit,
	                                   uint32_t* value) {
	  uint32_t result = 0;
	  for (uint32_t shift = 0; shift <= 28 && p < limit; shift += 7) {
	    uint32_t byte = *(reinterpret_cast<const unsigned char*>(p));
	    p++;
	    if (byte & 128) {
	      // More bytes are present
	      result |= ((byte & 127) << shift);
	    } else {
	      result |= (byte << shift);
	      *value = result;
	      return reinterpret_cast<const char*>(p);
	    }
	  }
	  return NULL;
	}

# Varint64 的编码与解码

对于64 位的整型，我们最多需要需要(`10 * 8 - 10 > 64`)10位来保存，如果像EncodeVarint32一样编码，则需要写10个if
分支，大牛肯定没有这么勤快写10个分支的，所以他们写出了下面这样的代码：

	char* EncodeVarint64(char* dst, uint64_t v) {
	  static const int B = 128;
	  unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
	  while (v >= B) {
	    *(ptr++) = (v & (B-1)) | B;
	    v >>= 7;
	  }
	  *(ptr++) = static_cast<unsigned char>(v);
	  return reinterpret_cast<char*>(ptr);
	}
	
其实EncodeVarint32也可以这么写，我估计他们之所以没有这么做，就是怕像我一样的菜鸟看到以后会抓狂。
解码也比刚才少了几行代码，并没有更难理解：

	const char* GetVarint64Ptr(const char* p, const char* limit, uint64_t* value) {
	  uint64_t result = 0;
	  for (uint32_t shift = 0; shift <= 63 && p < limit; shift += 7) {
	    uint64_t byte = *(reinterpret_cast<const unsigned char*>(p));
	    p++;
	    if (byte & 128) {
	      // More bytes are present
	      result |= ((byte & 127) << shift);
	    } else {
	      result |= (byte << shift);
	      *value = result;
	      return reinterpret_cast<const char*>(p);
	    }
	  }
	  return NULL;
	}

我现在还记得第一次看到varint时的心情：当时我就震精了。
