---
layout: post
title: "几种常见的字符串匹配算法"
description: ""
category: 算法
tags: [KMP, 字符串匹配]
---

本文将介绍几种常见的字符串匹配算法，包括朴素算法，大名鼎鼎的KMP算法，Rabin-karp算法。

#朴素算法

朴素算法比较简单，效率也比较低，在实际应用中，几乎是用不到的。下面是朴素算法的一个实现。

	#include<stdio.h>
	#include<string.h>
	void search(char *pat, char *txt)
	{
	    int M = strlen(pat);
	    int N = strlen(txt);
	  
	    /* A loop to slide pat[] one by one */
	    for (int i = 0; i <= N - M; i++)
	    {
	        int j;
	  
	        /* For current index i, check for pattern match */
	        for (j = 0; j < M; j++)
	        {
	            if (txt[i+j] != pat[j])
	                break;
	        }
	        if (j == M)  // if pat[0...M-1] = txt[i, i+1, ...i+M-1]
	        {
	           printf("Pattern found at index %d \n", i);
	        }
	    }
	}
	  
	/* Driver program to test above function */
	int main()
	{
	   char *txt = "AABAACAADAABAAABAA";
	   char *pat = "AABA";
	   search(pat, txt);
	   getchar();
	   return 0;
	}

参考资料：[geeksforgeeks.org][1]

#KMP 算法

KMP是一个严重考验理解能力的算法，很高兴，事实又证明了我的理解能力一般得不能再一般。
因为我学了三遍才勉强算是掌握了。

第一次接触KMP算法是在严蔚敏的《数据结构（C语言）》的第四章，当时硬着头皮看了一个下午，
没有弄懂。遂请教老师，被三言两语打发之，现在想想，他应该也不懂。

之后在学习《算法导论》时认真学习了一下，并做了几道题目，算是懂了个百分之五十。

网上讲KMP算法的有很多，但是，真正讲得好的少之又少，而像阮总这样画一堆图的，一个都没有。
阮总讲的KMP, 毫无疑问，是现在网络上所有将KMP算法最好的一个。

我也是通过阮总的[文章][2]才真正理解了这个算法。 建议所有还没有弄懂的朋友，都去看一下阮一峰的《[字符串匹配的KMP算法][2]》。

我觉得KMP算法的重点有三：

1.  对应于阮一峰的《[字符串匹配的KMP算法][2]》第七点，就是在空格与D不匹配的时候，我们已经
2.  第二个重点就是部分匹配表的生成，要理解，部分匹配表是模式自己与自己的匹配
3. 第三个重点就是有了部分匹配表以后，要知道，一次应该后移几位

		移位次数 ＝ 已匹配的字符数 - 对应的部分匹配值

下面是KMP算法的实现：


KMP算法虽然能达到O(M+N)的时间复杂度，但在实际使用中，KMP算法的性能并不如下面将要介绍的BM算法。

其他参考资料：《[Linux内核中的KMP实现][3]》

#Boyer-Moore算法

#Rabin-karp算法
 
#Morris-Pratt算法

http://www.stoimen.com/blog/2012/04/09/computer-algorithms-morris-pratt-string-searching/

[1]: http://www.geeksforgeeks.org/searching-for-patterns-set-1-naive-pattern-searching/
[2]: http://www.ruanyifeng.com/blog/2013/05/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm.html
[3]: http://wangcong.org/blog/archives/2090
