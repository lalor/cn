---
layout: post
title: "STL中各种容器的删除操作"
description: ""
category: 程序语言
tags: [erase, remove, STL]
---

在应用中，我们通常不可避免地要对容器中的某些特定元素进行删除操作。这看起来并不是什么困难的问题。我们先写一个循环来迭代容器中的元素，如果迭代元素是要删除的元素，则删除之。代码如下所示：

    vector<int> intContainer;  
    ..... 
    for(vector<int>::iterator is = intContainer.begin();
            it != intContainer.end();
            ++it)  
    {  
            if ( *it == 25)   
                        intContainer.erase(it);  
    }  


写出此代码的原意是将vector中值为25的元素删除，但不幸的是，这样做是错误的，这么做会带来诡异的未定义行为。因为当一个容器的一个元素被删除时，指向那个元素的所有迭代器将失效。当intContainer.erase(it)返回时，it已经失效。在for循环中对于失效的it执行自增操作，这是一件多么不靠谱的事情啊。既然这样行不通，那么我们可以求助于STL提供的remove算法。借助remove算法来达到删除元素的目的。

    vector<int> intContainer;  
    .....
    size_t before_size = intContainer.size();  
    remove(intContainer.begin(), intContainer.end(), 25);  
    size_t after_size = intContainer.size();


运行程序以后发现before_size和after_size是一样的，说明元素并没有被真正删除。写出以上程序，是处于对remove算法的不了解而致。STL中remove算法会将不该删除的元素前移，然后返回一个迭代器，该迭代器指向的是那个应该删除的元素，仅此而已。所以如果要真正删除这一元素，在调用remove之后还必须调用erase，这就是STL容器元素删除的"erase_remove"的惯用法。

    vector<int> intContainer;  
    .....
    intContainer.erase( remove(intContainer.begin(),
                intContainer.end(), 25),
            intContainer.end());


erase-remove的惯用法适用于连续内存容器，比如vector,deque和string，它也同样适用于list，但是并不是我们推荐的方法，因为使用list成员函数remove会更高效，代码如下：

     list<int> list_int;  
     ....  
     list_int.remove(25);  


如果是关联容器呢？标准关联容器没有remove成员函数，使用STL算法的remove函数时编译同不过。所以上述remove形式对于标准关联容器并不适用。在这种情况下，解决办法就是调用erase：

    map<int, int> mapContainer;  
    ...  
    mapContainer.erase(25);  


对于标准关联容器，这样的元素删除方式是简单有效的，时间复杂度为O(logn).

当我们需要删除的不是某一个元素，而是具备某一条件的元素的时候，我们只需要将remove替换成remove_if即可 

    bool Is2BeRemove(int value)  
    {  
            return value < 25;  
    }  
    vector<int> nVec;  
    list<int> nList;  
    ....  
      
    nVec.erase(remove_if(nVec.begin(), nVec.end(), Is2BeRemove),
            nVec.end());  
    nList.remove_if(Is2BeRemove);


**总结如下**

###删除容器中具有特定值的元素：

如果容器是vector、string或者deque，使用erase-remove的惯用法。如果容器是list，使用list::remove。如果容器是标准关联容器，使用它的erase成员函数。

###删除容器中满足某些条件的元素：

如果容器是vector、string或者deque，使用erase-remove_if的惯用法。如果容器是list，使用list::remove_if。如果容器是标准关联容器，使用remove_copy_if&swap 组合算法，或者自己设计个遍历删除算法。

**参考资料：**李健《[编写高质量C++代码](http://book.douban.com/subject/7059901/)》第七章，用好STL这个大轮子
