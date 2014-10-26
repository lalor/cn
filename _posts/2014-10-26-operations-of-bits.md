---
layout: post
title: "Bit Twiddling Hacks"
description: ""
category: 程序语言
tags: [C语言, 位操作]
---

本文介绍了一些有趣的Bit operation的练习，可以作为面试时，bit
operation的面试题，也可以作为应聘者准备面试的素材。难度各异，如果第一次遇到这些奇怪的
问题，觉得难以理解，也不用太在意。此外，请原谅我在一篇文章中中英文混用的情况，
我已经深刻的反省和检讨过了。

####1.不用sizeof判断int占几个字节

题目很简单，不用再过多解释。此题对整数类型，如char，short和long都适用。代码如下所示。


	#include <stdio.h>
	int main(int argc, char* argv[])
	{
	    unsigned int a = ~0;
	    int i = 1;
	    while ( (a = a >> 1) )
	    {
	        i++;
	    }
	    printf("%d byte %d bits\n", i / 8, i);
	    return 0;
	}

####2. 交换整型的奇数位和偶数位

问题定义：Write a program to swap odd and even bits in an integer with as few
instructions as possible(e.g, bit 0 and bit 1 are swapped, bit 2 and bit 3 are
swapped, etc)。代码如下所示。

	#include <stdio.h>

	int SwapOddEvenBit(int x)
	{
	    return ( ((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
	}
	int main(int argc, char* argv[])
	{
	    int a = 171;
	    printf("%d\n", SwapOddEvenBit(a));
	    return 0;
	}

####3. 将整数的某几位改成给成的位序列

问题定义：You are given two 32-bit numbers, N and M, and two bit positions,
i and j. write a metod to set all bits between i and j in N equal to M(e.g,
M becomes a substring of N located at i and starting at j).

EXAMPLE
Input: N = 10000000000 M = 10101, i = 2, j = 6
Output: N = 10001010100

SOLUTION：This code operates by clearing all bits in N between positon i and j ,
and then ORing to put M in there.

	unsigned int updateBits(unsigned int  n, unsigned int m, int i, int j)
	{
	    int max = ~0;

	    unsigned int left = max - ((1 << j) -1);
	    unsigned int right = ((1 << i) -1);

	    unsigned int mask = left | right;

	    return (n & mask) | (m << i);
	}


#### 4.  编写一个C表达式，使它生成一个字，由x的最低有效字节和y中剩下的字节组成。

例如，对于运算数x=0x89ABCDEF和y=0x76543210，得到的结果应为0x765432EF

	return (x & 0xFF) | (y & ~0xFF)

####5. 编写一个C表达式，在下列描述的条件下产生1,而在其他情况下得到0。假设x是int型。

* x的任何位都等于1

		return !(~x)

* x的任何位都等于0

		return !x

* x的最高有效字节中的位都等于1

		n = ((sizeof(int) - 1 ) << 3)
		!( ~(x >> n))

* x的最低有效字节中的位都等于0

		!(x&0xff)

####6. 编写一个函数`int_shifts_are_logical()`，在对int类型的数使用算术右移的机器上运行时，这个函数返回1,其他情况下，返回0

	int int_shifts_are_logical()
	{
		x = ~0;
		return (x >> 1) = x;
	}

####7. 判断是否给定的数，奇数位不都全是0

		int any_even_one(unsigned x)
		{
			return x & (0xAAAAAAAA)
		}


####8. 对于给定的无符号整数，判断它的二进制表示中，1的个数为奇数

	/* Return 1 when x contains an even number of 1s; 0 otherwise.
	Assume w = 32*/

	int even_ones(unsigned x)
	{
		x ^= x >> 16;
		x ^= x >> 8;
		x ^= x >> 4;
		x ^= x >> 2;
		x ^= x >> 1;
		return ( ! x&1 );
	}

####9. 旋转整数的bit，对于给定的无符号整数x和n，将x的二进制表示向右旋转n位

	/* Do rotating right shift. Assume 0 <= n < w
	* Example when x = 0x12345678 and w = 32:
	* n = 4 -> 0x81234567, n = 20 -> 0x45678123
	*/
	unsigned rotate_right(unsigned x, int n)
	{
		int w = sizeof(x) << 3;
		int max = ~0;
		left = max - ((1 << n) - 1);
		right = ( 1 << n ) - 1;
		return ((x & left) >> n) | (x & right << (w - n));
	}

####10.Detect if two integers have opposite signs

	int x, y;               // input values to compare signs

	bool f = ((x ^ y) < 0); // true iff x and y have opposite signs
	Manfred Weis suggested I add this entry on November 26, 2009.

####11. Determining if an integer is a power of 2

	unsigned int v; // we want to see if v is a power of 2
	bool f;         // the result goes here

	f = (v & (v - 1)) == 0;
	Note that 0 is incorrectly considered a power of 2 here. To remedy this, use:
	f = v && !(v & (v - 1));

####12. Counting bits set, Brian Kernighan's way

	unsigned int v; // count the number of bits set in v
	unsigned int c; // c accumulates the total bits set in v
	for (c = 0; v; c++)
	{
	  v &= v - 1; // clear the least significant bit set
	}


后续如果遇到比较好的题目，还会往里面添加。这里选择的都不是特别难的题目，
毕竟太难的我自己也理解不了。如果对于bit操作特别感兴趣，那么，我有一下两份材料推荐：

\[1\]: [CSAPP DATA LAB](http://csapp.cs.cmu.edu/public/labs.html)

\[2\]: [Hacker's Delight](http://graphics.stanford.edu/~seander/bithacks.html#CopyIntegerSign)
