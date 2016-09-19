---
layout: post
title: "Python并发工具"
description: ""
category: 编程语言
tags: [python, 并发]
---

相信每一个接触Python并发的同学,刚开始都会有一段困惑时间:Python的并发看起来杂乱无章.
大家使用最多的是threading,但是,Python中由于GIL的存在,又没有实现真正的并发.因此,
在寻求其他并发工具的时候,Python往往由于资料太多,而大家又没有系统性的学习并发,导致
丈二的和尚摸不着头脑.

本文将分别讲解Python中常用的几个并发工具：

1. 线程
2. 进程
3. 线程池/进程池
4. 并发执行框架
5. Python 3的asyncio
6. 开源的gevent

![snake](/cn/a-yellow-snake-pv.png)

### 1. Python的threading库

Python通过threading库提供了并发支持,有两种方式可以使用多线程:

* 新建Thread对象
* 继承Thread,然后实现run方法

Python中的多线程使用示例:

    #!/usr/bin/python
    #-*- coding: UTF-8 -*-
    import os
    from threading import Thread

    def get_data(num):
        print('sum to {0} with pid {1}'.format(num, os.getpid()))
        return sum([i for i in range(num)])

    def main():
        data = [ i for i in range(10) ]
        threads = []
        for num in data:
            thread = Thread(target=get_data, args=(num,))
            thread.start()
            threads.append(thread)

        for thread in threads:
            thread.join()

    if __name__ == '__main__':
        main()

### 2. Python的multiprocessing库

> multiprocessing is a package that supports spawning processes using an API
> similar to the threading module.

可以看到,为了让用户方便的使用多进程,减少用户代码的修改,multiprocessing的API与
threading的API是保持一致的.

例如,对于上面这个并发的例子,只需要将导入语句修改为下面的语句,就能够正常使用多进程:

    from multiprocessing import Process as Thread

当然,实际使用中不推荐大家`import Process as Thread`,这里只是为了说明,这两个包的接口
是一样的,很方便大家进行切换.

### 3. 线程池/进程池

我们知道,Python里面有一个内置的map函数,该函数是函数式编程语言里面的概念,也是
map-reduce中的map,它的作用就是对一个序列中的每一个元素进行某种操作,例如,求整数的平方:

    map(lambda x: x*x, [ i for i in range(5))

内置的map是单线程运行的,如果涉及到网络请求或者大量的cpu计算,则速度相对会慢很多,因此,
出现了并发的map,如下所示：

    import requests
    from multiprocessing import Pool

    def get_website_data(url):
        r = requests.get(url)
        return r.url

    def main():
        urls = ['http://mingxinglai.com',
                'http://www.baidu.com',
                'http://163.com']
        pool = Pool(2)
        print pool.map(get_website_data, urls)

    main()

为了与线程兼容,该模块还提供了multiprocessing.dummy,用以提供线程实现,如下所示：

    from multiprocessing.dummy import Pool

### 4. 并发执行框架

用过java的同学应该知道,java中有ThreadPoolExecutor框架,这类框架特别适合执行一些大批量
的异步任务.

Python3的concurrent.futures模块,提供了Executor框架,类似于java中的ThreadPoolExecutor,
不过,Python中支持两种Executor,分别是：

* ThreadPoolExecutor
* ProcessPoolExecutor

示例如下:

    import concurrent.futures
    import math

    PRIMES = [
        112272535095293,
        112582705942171,
        112272535095293,
        115280095190773,
        115797848077099,
        1099726899285419]

    def is_prime(n):
        if n % 2 == 0:
            return False

        sqrt_n = int(math.floor(math.sqrt(n)))
        for i in range(3, sqrt_n + 1, 2):
            if n % i == 0:
                return False
        return True

    def main():
        with concurrent.futures.ProcessPoolExecutor() as executor:
            for number, prime in zip(PRIMES, executor.map(is_prime, PRIMES)):
                print('%d is prime: %s' % (number, prime))

    if __name__ == '__main__':
        main()

如果要使用ThreadPoolExecutor,只需要修改一行代码即可：

    with ThreadPoolExecutor(max_workers=1) as executor:

### 5. asyncio

Python 3.4 中asyncio被纳入了标准库,它提供了使用协程编写单线程并发代码,通过IO多路复用
技术访问套接字和其他资源,Python 3.5中添加了async和awit这两个关键字.自此,协程成为新的
语法,而不再是一种生成器类型.


    import asyncio

    async def slow_operation(n):
        await asyncio.sleep(1)
        print('Slow operation {} complete'.format(n))

    async def main():
        await asyncio.wait([
            slow_operation(1),
            slow_operation(2),
            slow_operation(3),
            ])

    loop = asyncio.get_event_loop()
    loop.run_until_complete(main())

### 6. gevent

gevent是一个基于微线程库greenlet的并发框架,其基本思想是：当一个greenlet遇到IO操作时,
比如访问网络,就自动切换到其他的greenlet,等到IO操作完成,再在适当的时候切换回来继续执行.
由于IO操作非常耗时,经常使程序处于等待状态,有了gevent为我们自动切换协程,就保证总有
greenlet在运行,而不是等待IO.

虽然与直接使用greenlet、eventlet相比性能略低,但是,它提供了和线程模型编程相仿的接口,
而且提供了Monkey Patch方法,可以在运行时动态修改标准库里大部分的阻塞式系统调用,
如socket, threading, select等模块.

gevent的好处是Python 2和Python 3都可以使用,不像asyncio只能在python
3中使用.Gevent的使用示例如下:

    import gevent

    def foo():
        print('Running in foo')
        gevent.sleep(0)
        print('Explicit context switch to foo again')

    def bar():
        print('Explicit context to bar')
        gevent.sleep(0)
        print('Implicit context switch back to bar')

    gevent.joinall([
        gevent.spawn(foo),
        gevent.spawn(bar),
    ])

**再强调一遍:** 当一个greenlet遇到IO操作时,比如访问网络,就自动切换到其他的greenlet,
等到IO操作完成,再在适当的时候切换回来继续执行.由于IO操作非常耗时,经常使程序处于等待
状态,有了gevent为我们自动切换协程,就保证总有greenlet在运行,而不是等待IO.

所以上段代码的执行流程如下：

![flow](/cn/python_concurrent_flow.gif)

### 总结

本文概要的介绍了Python中的6个并发工具,大家可以通过本文,快速的了解各个工具的使用,选择
适合的工作.
