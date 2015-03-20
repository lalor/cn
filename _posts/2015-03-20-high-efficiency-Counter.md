---
layout: post
title: "设计一个高效的计数器"
description: ""
category: 程序设计
tags: [并发编程]
---

在软件系统中，经常需要使用全局的计数器，用以统计操作的次数、资源的消耗和其他一些统计信息。在高并发的系统中，如果有很多线程都需要修改同一个全局计数器，那么，这个计数器就很有可能成为性能瓶颈。本文将一步步引导读者设计出一个高效的计数器。

### 1. 简单的计数器

下面这段代码定义了一个全局变量count，用以统计函数fun的调用次数，这就是一个最简单的、典型的计数器，相信很多人都写过这样的代码。


    int count = 0;

    void fun() {
        //do something
        count++;
    }

    int main() {
        //do something
        fun();
        //do something
        printf("%d\n", count);
    }


### 2. 并发情况下的计数器

上面这段代码在fun函数中修改全局变量count的值，用以统计fun的调用次数，这段代码非常简单，太简单了以至于在生产环境基本不会这么使用。对于上面这段代码，如果存在多个线程并发调用fun函数会出现什么情况呢？


    #include <pthread.h>
    #include <semaphore.h>
    #include <stdio.h>
    #include "myspinlock.h"

    // 是否使用spinlock包含全局变量，默认是不使用，改为1则为使用

    #define USE_SPINLOCK 0

    sem_t beginSemaArr[500];

    sem_t endSema;

    int global_count;
    int add_loops = 10000;

    spinlock slock = 0;

    void *threadAddFunc(void *param)
    {
        long thread_id = (long)param;

        for (;;)
        {
            sem_wait(&beginSemaArr[thread_id]);  // Wait for signal

            for (int i = 0; i < add_loops; i++)
    	{
    #if USE_SPINLOCK
    		spin_lock(&slock);
    		global_count++;
    		spin_unlock(&slock);
    #else
    		global_count++;
    #endif
    	}

            sem_post(&endSema);  // Notify transaction complete
        }
        return NULL;  // Never returns
    };

    int main()
    {
    	// Initialize the semaphores
    	for (int i = 0; i < 500; i++)
    		sem_init(&beginSemaArr[i], 0, 0);

        sem_init(&endSema, 0, 0);

        // Spawn the threads
        pthread_t threadArr[500];
    	for (int i = 0; i < 500; i++)
    		pthread_create(&threadArr[i], NULL, threadAddFunc, (void *)i);

        // Repeat the experiment ad infinitum
        for (int iterations = 1; ; iterations++)
        {
            // 重设 global_count
            global_count = 0;

            // Signal threads
    		for (int j = 0; j < 500; j++)
    			sem_post(&beginSemaArr[j]);

            // Wait for threads
    		for (int j = 0; j < 500; j++)
    			sem_wait(&endSema);

            // threads all finish, print global_count
    	if (global_count != 5000000)
    		printf("iter %d: global_count = %d\n", iterations, global_count);
        }
        return 0;  // Never returns
    }


对于一个全局计数器，如果不加锁保护，那么，它可能不会像你预期的一样运行，对于上面这段代码，如果不加锁保护，有很大的概率最后的结果会小于5000000，这是因为: globalcount++不是一个[原子](http://preshing.com/20130618/atomic-vs-non-atomic-operations/)操作。这段代码在[这里](http://vdisk.weibo.com/s/dBzv2siaGkcF)，可以尝试运行一下。

参考：《[锁的意义](http://hedengcheng.com/?p=803)》

### 3. 原子操作

那么，如何解决上面提到的问题呢？这就是并发编程的入门知识了，是每一个学习操作系统课程的同学都应该知道的知识，即对globalcount++操作加上用锁进行保护就可以了，加上锁保护以后，这就是一个可用的全局计数器了。

故事到这里就应该结束了吗？远远没有，我们还有很多事情可以做！

在上面的这段代码中，对globalcount的操作非常简单，基本上只有++操作，对于这么简单的操作，我们需要使用锁进行保护，每一次操作都需要获取锁、释放锁，是不值得的，毕竟，加锁和解锁操作也需要不小的[开销](http://preshing.com/20111124/always-use-a-lightweight-mutex/)。那么，是否还可以进行优化呢？答案是肯定的。

我们之所以需要使用锁对globalcount进行保护，是因为globalcount++操作不是原子的，如果能够保证globalcount++是原子操作，就可以不用任何锁保护了。幸运的是，我们是有这样的系统调用的：<http://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html>

原子操作的使用方法如下所示：


    #include <stdio.h>

    int main(int argc, char* argv[])
    {
        int a = 0;
        int i = 100;
        for ( i = 0; i < 100; i++)
        {
            __sync_fetch_and_add(&a, 5);
        }
        printf("%d\n", a);
        return 0;
    }

对于前面的全局计数器，只需要将`globalcount++`操作，修改成`__sync_fetch_and_add(&globalcount, 1)`即可。

### 4. 如果竞争很激烈呢？

到目前为止，我们已经有了一个完全可用的，并且效率不错的全局计数器了，但是，在一个高并发的系统中，这还远远不够。例如，在数据库系统中，可能存在很多线程并发的修改同一个计数器。对于这种情况，全局计数器就是竞争的热点，性能的瓶颈。

为了提高数据库系统的扩展性和吞吐率，需要将计数器进行拆分，即，不是使用一个计数器，而是使用一个计数器数组，在需要统计结果的时候，再将计数器数组中的值累加起来。实现方式也很简单呢，可以通过线程的ID（或者其他hash值）对数组个数进行取模，将不同的线程分配到不同的计数器上，这样一个简单的改变，显著地提升了系统的扩展性，几乎完全消除了竞争热点。


    const int N = 7;
    int globalcount[N];

    void fun() {
        //do something
        __sync_fetch_and_add(&globalcount[ thread_id % N ], 1);
    }

### 5. 还能不能继续优化呢？

我们已经使用了两种优化方式对计数器进行了优化，那么，还能不能继续优化呢？答案也是肯定的，我们还可以使用CACHE_LINE_SIZE对齐，减少高速Cache的竞争。这个介绍起来比较麻烦一点，先看一下MySQL数据库是如何实现一个计数器的：


    /**************************************************//**
    @file include/ut0counter.h

    Counter utility class

    Created 2012/04/12 by Sunny Bains
    *******************************************************/

    #ifndef UT0COUNTER_H
    #define UT0COUNTER_H

    #include "univ.i"
    #include <string.h>
    #include "os0thread.h"

    /** CPU cache line size */
    #define CACHE_LINE_SIZE		64

    /** Default number of slots to use in ib_counter_t */
    #define IB_N_SLOTS		64

    /** Get the offset into the counter array. */
    template <typename Type, int N>
    struct generic_indexer_t {
    	/** Default constructor/destructor should be OK. */

            /** @return offset within m_counter */
            size_t offset(size_t index) const UNIV_NOTHROW {
                    return(((index % N) + 1) * (CACHE_LINE_SIZE / sizeof(Type)));
            }
    };


    /** Class for using fuzzy counters. The counter is not protected by any
    mutex and the results are not guaranteed to be 100% accurate but close
    enough. Creates an array of counters and separates each element by the
    CACHE_LINE_SIZE bytes */
    template <
    	typename Type,
    	int N = IB_N_SLOTS,
    	template<typename, int> class Indexer = thread_id_indexer_t>
    class ib_counter_t {
    public:
    	ib_counter_t() { memset(m_counter, 0x0, sizeof(m_counter)); }

    	~ib_counter_t()
    	{
    		ut_ad(validate());
    	}

    	/** If you can't use a good index id. Increment by 1. */
    	void inc() UNIV_NOTHROW { add(1); }

    	/** If you can't use a good index id.
    	* @param n  - is the amount to increment */
    	void add(Type n) UNIV_NOTHROW {
    		size_t	i = m_policy.offset(m_policy.get_rnd_index());

    		ut_ad(i < UT_ARR_SIZE(m_counter));

    		m_counter[i] += n;
    	}

    	/** Use this if you can use a unique indentifier, saves a
    	call to get_rnd_index().
    	@param i - index into a slot
    	@param n - amount to increment */
    	void add(size_t index, Type n) UNIV_NOTHROW {
    		size_t	i = m_policy.offset(index);

    		ut_ad(i < UT_ARR_SIZE(m_counter));

    		m_counter[i] += n;
    	}

    	/** If you can't use a good index id. Decrement by 1. */
    	void dec() UNIV_NOTHROW { sub(1); }

    	/** If you can't use a good index id.
    	* @param - n is the amount to decrement */
    	void sub(Type n) UNIV_NOTHROW {
    		size_t	i = m_policy.offset(m_policy.get_rnd_index());

    		ut_ad(i < UT_ARR_SIZE(m_counter));

    		m_counter[i] -= n;
    	}

    	/** Use this if you can use a unique indentifier, saves a
    	call to get_rnd_index().
    	@param i - index into a slot
    	@param n - amount to decrement */
    	void sub(size_t index, Type n) UNIV_NOTHROW {
    		size_t	i = m_policy.offset(index);

    		ut_ad(i < UT_ARR_SIZE(m_counter));

    		m_counter[i] -= n;
    	}

    	/* @return total value - not 100% accurate, since it is not atomic. */
    	operator Type() const UNIV_NOTHROW {
    		Type	total = 0;

    		for (size_t i = 0; i < N; ++i) {
    			total += m_counter[m_policy.offset(i)];
    		}

    		return(total);
    	}

    private:
    	/** Indexer into the array */
    	Indexer<Type, N>m_policy;

            /** Slot 0 is unused. */
    	Type		m_counter[(N + 1) * (CACHE_LINE_SIZE / sizeof(Type))];
    };

    #endif /* UT0COUNTER_H */


这是一个生产环境的计数器实现，我们来看最难的部分，也就是我们要讲的部分，代码的倒数第4行：


    Type m_counter[(N + 1) * (CACHE_LINE_SIZE / sizeof(Type))];

我们知道，N是计数器数组的元素个数，CACHE_LINE_SIZE是64，那么，对于一个计数器数组，MySQL的计数器类到底为它分配了几个字节呢？

一起来分析一下：假设计数器是int型，那么，sizeof(Type)的值就是4,假设N=7,那么，m_counter就有`（（7+1）×（64/4））= 128`个int元素，所以m_counter共占用`128*4=512`字节。我们明明只有7个计数器，为什么会分配128个int元素的数组呢？这还得从背后的技术说起。

1. 为什么CACHE_LINE_SIZE定义成64？因为，高速缓存Cache的读写单位是64字节
2. 为什么需要N+1个元素？因为第一个元素不用
3. 为什么第一个元素不使用？为了与CACHE_LINE_SIZE对齐
4. 多了一个元素，为什么就能与CACHE_LINE_SIZE对齐？

再来看一下m_counter的内存分配策略，仔细看一下m_counter是一个Type型的数组，也就是说，m_counter中的每一个元素都占用sizeof(Type)个字节，那么m_counter占用的内存公式为(N+1)*(CACHE_LINE_SIZE/sizeof(Type))*sizeof(Type),其中,sizeof(Type)可以约掉，就只剩下(N+1)*CACHE_LINE_SIZE，很明显，每一个计数器不是占用sizeof(Type)个字节，而是占用64个字节，且第一个元素的64个字节不使用。

如图所示：

![img1](/cn/image/cacheline.png)

其中：粉色的4个方块，代表第一个元素，仅仅是pading，不会使用；红色的方块是我们真正的计数器，为了使得每一个计数器位于一个cache line中，白色的方块故意不用。

那么，为什么需要这么做呢？要理解这个问题，需要对Cache的工作原理有较好的理解，虽然网络上也有一些比较好的将cache的材料，我还是推荐《[深入理解计算机系统](http://book.douban.com/subject/5333562/)》，这本书的第九章虚拟存储器，理由有二：1）这本书非常好，给我周围的每个人都推荐；2）看书知识能够形成体系，网络上的碎片知识很难理解透彻。

在解释为什么Cache line对齐，能够提高效率之前，我们再看一个小例子。

在MySQL 5.5中，有一个名为kernel_mutex的锁，该锁是整个系统的竞争热点和性能瓶颈，因此，MySQL对该锁进行了优化，通过pad的方式来减少对象的访问竞争：

    byte* srv_pad1[64];
    mutex_t* kernel_mutex;
    byte* srv_pad2[64];

当然，这只是优化的小技巧，在MySQL 5.6中，对该mutex进行了拆分。

我们关注的重点就是，为什么通过pad的方式能够减少对象的访问竞争。要理解为何通过pad的方式能够减少访问的竞争，首先得理解什么地方存在竞争。对于现代处理器（英特尔酷睿i7，酷睿， Atom和NetBurst， Core Solo和Pentium M处理器），它们的的L1，L2或L3缓存的高速缓存行是64个字节宽，不支持部分填充缓存行，也就是说，Cache每次从内存读、写的数据单位是64字节，当一个处理器试图修改某一个变量时，会将整个缓存行锁定，那么在缓存一致性机制的作用下，会导致其他处理器不能访问自己高速缓存中的内容，所以在多处理器的情况下将会严重影响效率。使用追加到64字节的方式来填满高速缓冲区的缓存行，避免将不同的计数器加载到同一个缓存行，使得各个计数器的修改不会互相锁定。

### 5. 总结

本文首先介绍了一个可用的计数器，然后分别介绍了三种技术对计数器进行优化，分别是：原子操作、计数器拆分和CACHE_LINE_SIZE优化。计数器只是这篇文章的一个载体，主要的技术和知识，可以应用于软件设计的各个方面。
