---
layout: post
title: "几种常见的字符串匹配算法"
description: ""
category: 算法
tags: [KMP, BM, 算法, 字符串匹配]
---

本文将介绍几种常见的字符串匹配算法，大部分材料都是网
络上收集而来。不过都经过我审察学习过，保证是不错的学习素材。

#1. 朴素算法

朴素算法是最简单的字符串匹配算法，也是人们接触得最多的字符串匹配算法。代码一看就懂，在此不在赘述。

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



朴素算法有两层循环，外层循环执行N-M+1次，内层循环执行M次，所以它的时间复杂度为O(（N-M+1)\*M)。

参考资料：<http://www.geeksforgeeks.org/searching-for-patterns-set-1-naive-pattern-searching/>


#2. Rabin-Karp算法

我们再来看一个时间复杂度为O(（N-M+1)\*M)的字符串匹配算法，即Rabin-Karp算法。Rabin-Karp算法的预处理时间是O(m)，
匹配时间OO(（N-M+1)\*M)，既然与朴素算法的匹配时间一样，而且还多了一些预处理时间，那为什么我们
还要学习这个算法呢？

虽然Rain-Karp在最坏的情况下与朴素的世间复杂度一样，但是实际应用中往往比朴素算法快很多。而且该算法的
期望匹配时间是O(N+M)(参照《算法导论》)。

在朴素算法中，我们需要挨个比较所有字符，才知道目标字符串中是否包含子串。那么，
是否有别的方法可以用来判断目标字符串是否包含子串呢？

答案是肯定的，确实存在一种更快的方法。为了避免挨个字符对目标字符串和子串进行比较，
我们可以尝试一次性判断两者是否相等。因此，我们需要一个好的哈希函数（hash function）。
通过哈希函数，我们可以算出子串的哈希值，然后将它和目标字符串中的子串的哈希值进行比较。
这个新方法在速度上比暴力法有显著提升。

Rabin-Karp算法的思想：

0. 假设子串的长度为M,目标字符串的长度为N
1. 计算子串的hash值
2. 计算目标字符串中每个长度为M的子串的hash值（共需要计算N-M+1次）
3. 比较hash值
4.  如果hash值不同，字符串必然不匹配，如果hash值相同，还需要使用朴素算法再次判断

为了快速的计算出目标字符串中每一个子串的hash值，Rabin-Karp算法并不是对目标字符串的
每一个长度为M的子串都重新计算hash值，而是在前几个字串的基础之上， 计算下一个子串的
hash值，这就加快了hash之的计算速度，将朴素算法中的内循环的世间复杂度从O(M)将到了O(1)。

关于hash函数的详细内容，可以参考[这里][2]或者《算法导论》。

下面是一个Rabin-Karp算法的实现：

    /* Following program is a C implementation of the Rabin Karp Algorithm 
       given in the CLRS book */
     
    #include<stdio.h>
    #include<string.h>
     
    // d is the number of characters in input alphabet
    #define d 256 
      
    /*  pat  -> pattern
        txt  -> text
        q    -> A prime number
    */
    void search(char *pat, char *txt, int q)
    {
        int M = strlen(pat);
        int N = strlen(txt);
        int i, j;
        int p = 0;  // hash value for pattern
        int t = 0; // hash value for txt
        int h = 1;
      
        // The value of h would be "pow(d, M-1)%q"
        for (i = 0; i < M-1; i++)
            h = (h*d)%q;
      
        // Calculate the hash value of pattern and first window of text
        for (i = 0; i < M; i++)
        {
            p = (d*p + pat[i])%q;
            t = (d*t + txt[i])%q;
        }
      
        // Slide the pattern over text one by one 
        for (i = 0; i <= N - M; i++)
        {
            
            // Chaeck the hash values of current window of text and pattern
            // If the hash values match then only check for characters on by one
            if ( p == t )
            {
                /* Check for characters one by one */
                for (j = 0; j < M; j++)
                {
                    if (txt[i+j] != pat[j])
                        break;
                }
                if (j == M)  // if p == t and pat[0...M-1] = txt[i, i+1, ...i+M-1]
                {
                    printf("Pattern found at index %d \n", i);
                }
            }
             
            // Calulate hash value for next window of text: Remove leading digit, 
            // add trailing digit           
            if ( i < N-M )
            {
                t = (d*(t - txt[i]*h) + txt[i+M])%q;
                 
                // We might get negative value of t, converting it to positive
                if(t < 0) 
                  t = (t + q); 
            }
        }
    }
      
    /* Driver program to test above function */
    int main()
    {
        char *txt = "GEEKS FOR GEEKS";
        char *pat = "GEEK";
        int q = 101;  // A prime number
        search(pat, txt, q);
        getchar();
        return 0;
    }



参考资料：<http://www.geeksforgeeks.org/searching-for-patterns-set-3-rabin-karp-algorithm/>


#3. KMP算法

KMP算法是一个比较难以理解的算法。国内非顶尖的大学数据结构大都使用的是清华严老师的
《数据结构》，这本书在第四章讲到了KMP算法，我认真看了两遍没有看懂。到现在我终于明白
了以后，我想说，真的是严老师实在讲得太烂了。

KMP算法要讲清楚很不容易，网络上一搜一大堆材料，看完还是似懂非懂。我不准备再讲一遍，
我也不相信自己会讲得比网络上大多数文章讲得好，在此给大家推荐阮一峰老师的《[字符串匹配的KMP算法][1]》。

学习KMP算法请牢记两点：

1. KMP算法的思想就是，当子串与目标字符串不匹配时，其实你已经知道了前面已经匹配成功那
一部分的字符（包括子串与目标字符串）。以阮一峰的文章为例，当空格与D不匹配时，你其实
知道前面六个字符是"ABCDAB"。KMP算法的想法是，设法利用这个已知信息，不要把"搜索位置"
移回已经比较过的位置，继续把它向后移，这样就提高了效率

    ![img1](http://image.beekka.com/blog/201305/bg2013050107.png)

2. 部分匹配表是模式自己与自己的匹配


下面推荐几个KMP算法的练习：


#4. Boyer-Moore算法

待补充。

[1]:http://www.ruanyifeng.com/blog/2013/05/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm.html
[2]:http://net.pku.edu.cn/~course/cs101/2007/resource/Intro2Algorithm/book6/chap34.htm
