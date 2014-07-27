---
layout: post
title: "旋转数组"
description: ""
category: 算法
tags: [C语言, 算法]
---

#1. 问题描述

将一个具有n个元素的向量，向左旋转i个位置。

例如，具有8个元素的向量abcdefgh，向左旋转3个元素的结果是defghabc。

此题有很多解法，下面只讨论两个优美高效的解法。

#2. 经典解法

经典解法来自于《编程珠玑》，这也是大多数人使用的方法。这个解法的思路如下：

首先，将这个问题看做是把数组ab转换成ba，但同时也假定我们具有在数组中转置指定指定部分元素的函数。我们先从ab开始，转置a得到a^r b，再转置b得到 a^r b^r ，然后再转置整个a^r b^r 得到(a^r b^r )^r ,即ba，这就导致了下面产生旋转作用的代码;注释显示了abcdefgh向左旋转3个元素的结果。

	reverse(0, i-1) /* cba defgh */
	reverse(i, n-1) /* cba hgfed */
	reverse(0, n-1) /* defghabc */

完整的代码如下：

	#include <stdio.h>
	#include <string.h>
	
	void reverse(char *base, int left, int right)
	{
	    char *p = base + left;
	    char *q = base + right;
	
	    while ( p < q )
	    {
	        char temp = *p;
	        *p++ = *q;
	        *q-- = temp;
	    }
	}
	
	
	int main(int argc, char* argv[])
	{
	
	    char str[] = "abcdefgh";
	    int n = strlen(str);
	    int i = 3;
	
	    reverse(str, 0, i-1); printf("%s\n", str);
	    reverse(str, i, n-1); printf("%s\n", str);
	    reverse(str, 0, n-1); printf("%s\n", str);
	    return 0;
	}

#3. 创新解法

下面再来第二种解法，这种解法来自于C++ STL的reverse调用的源码，代码如下：


	template <class ForwardIterator>  
	  void rotate ( ForwardIterator first, ForwardIterator 	middle,  	
	           	     ForwardIterator last )  	
	{  	
	  ForwardIterator next = middle;  	
	  w	hile (first!=next)  	
	  {	  	
	   	 swap (*first++,*next++);  	
	   	 if (next==last) next=middle;  	
	    else if (first == middle) middle=next;  	
	  }  
	}  


上面的伪代码可能不太好理解，我们来分析一下。

假设数组有N个元素，旋转i位，且(N>i)，我们首先考虑i < N-i的情况，我们可以把数组分成三部分，分别是 ab1 b2，其中，a的长度等于b1的长度,然后算法执行一轮以后，就得到 b1ab2，剩下要做的事情就是交换ab2的顺序，且中点应该在ab2交接的地方，所以有middle=next，假设此时b2的长度小于a，则将划分为a1a2b2，然后再执行一次while循环，整个数组就变成了b1b2a2a1,现在是要交换a2和a1的顺序即可。

交换a2和a1的顺序其实和交换a和b的顺序是一样的，只是元素更少了。

完整的代码如下所示：

	#include <stdio.h>
	#include <string.h>
	
	void swap(char *p, char *q)
	{
	    if ( p == NULL || q == NULL) return;
	
	    char temp = *p;
	    *p = *q;
	    *q = temp;
	}
	
	
	void rotate(char* first, char* middle, char* last)
	{
	    char *next = middle;
	    while( first != next)
	    {
	        swap( first++, next++);
	        if (next == last){ next = middle;}
	        else if ( first == middle ) { middle = next;}
	    }
	    return;
	}
	
	
	int main(int argc, char* argv[])
	{
	
	   char str[] = "abcdefgh";
	   int n = strlen(str);
	   int i = 3;
	   rotate( str, str + i, str + n);
	   printf("%s\n", str);
	   return 0;
	}