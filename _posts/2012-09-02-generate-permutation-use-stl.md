---
layout: post
title: "STL：从N个元素中取出M个的所有组合"
description: ""
category: 算法
tags: [STL, algorithm, 排列]
---

**题目要求：**输出从N个不同元素中取出M个元素的所有组合

这也算是一个非常经典的面试题,考的就是排列组合,高老先生在《计算机程序设计艺术》里足足写了一卷书来讲解排列组合,懒惰的博主还没来得及膜拜。看到这题首先能想到的就是使用回溯,程序如下：


    #include <iostream>
    #include <algorithm>
    #include <vector>
    #include <iterator>
    using namespace std;
    
    const size_t M = 3;
    
    void premutate(std::vector<int> &v, int data[], int num = 0)
    {
    
        if (num == M) 
        {//output
            copy(data, data + num, ostream_iterator<int>(cout, " "));
            cout << endl;
            return;
        }
        else
        {
            for (vector<int>::iterator it = v.begin(); it != v.end(); it++) 
            {
                //我们注意到,对有序的数排列的时候,排列后面的数都比前面的大
                //我们利用这个特性,快速的判断一个数是否已经在排列当中了
                if ( num != 0 && data[num - 1] >= *it )
                {
                    continue;
                }
                else
                {
                    data[num] = *it;
                    premutate(v, data, num + 1);
                }
            }
        }
    }
    
    int main(int argc, char* argv[])
    {
        int  elements[] = {1,2,3,4,5};
        const size_t N = sizeof(elements) / sizeof(elements[0]);
        std::vector<int> selectors(elements, elements + N);
        int data[N] = {0};
    
        sort(selectors.begin(), selectors.end());
        premutate(selectors, data);     
        return 0;
    }




下面是使用STL来解决这个问题,思路是对序列{1,1,1,0,0,0,0}做全排列,对于每个排列,输出数字1对应位置上的元素,代码如下：


    #include <assert.h>
    #include <algorithm>
    #include <iostream>
    #include <iterator>
    #include <vector>
    
    int main()
    {
      int values[] = { 1, 2, 3, 4, 5, 6, 7 };
      int elements[] = { 1, 1, 1, 0, 0, 0, 0 };
      const size_t N = sizeof(elements)/sizeof(elements[0]);
      assert(N == sizeof(values)/sizeof(values[0]));
      std::vector<int> selectors(elements, elements + N);
    
      int count = 0;
      do
      {
        std::cout << ++count << ": ";
        for (size_t i = 0; i < selectors.size(); ++i)
        {
          if (selectors[i])
          {
            std::cout << values[i] << ", ";
          }
        }
        std::cout << std::endl;
      } while (prev_permutation(selectors.begin(), selectors.end()));
    }


当我令N=10,M=5时在我的电脑上进行测试,循环执行100000遍,回溯法耗时37秒,STL耗时31秒,大家可能觉得回溯法中排序耗掉了很多时间,把排序这一行注释掉以后,运行时间跟需要排序几乎是一样的(原因还没有找到)。我们能够得出如下**结论：**使用STL对这个问题是一个理想的解决方案,速度快,开发效率高,不容易出错（这里的回溯排序以后可能与原先想要的结果不一样）。当然了,前提是你要知道这个问题可以用STL来做。完。




