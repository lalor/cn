---
layout: post
title: "几个位操作(bit operation)的练习题"
description: ""
category: 程序语言
tags: [C语言, 位操作]
---

#1.不用sizeof判断int占几个字节

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

#2. 交换整型的奇数位和偶数位

问题定义：Write a program to swap odd and even bits in an integer with as few instructions as possible(e.g, bit 0 and bit 1 are swapped, bit 2 and bit 3 are swapped, etc)。代码如下所示。

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

#3. 将整数的某几位改成给成的位序列

问题定义：You are given two 32-bit numbers, N and M, and two bit positions, i and j. write a metod to set all bits between i and j in N equal to M(e.g, M becomes a substring of N located at i and starting at j).

EXAMPLE  
Input: N = 10000000000 M = 10101, i = 2, j = 6  
Output: N = 10001010100

SOLUTION：This code operates by clearing all bits in N between positon i and j , and then ORing to put M in there.

	unsigned int updateBits(unsigned int  n, unsigned int m, int i, int j)
	{
	    int max = ~0;

	    unsigned int left = max - ((1 << j) -1);
	    unsigned int right = ((1 << i) -1);

	    unsigned int mask = left | right;

	    return (n & mask) | (m << i);
	}

