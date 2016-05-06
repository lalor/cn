---
layout: post
title: "同学,你的java线程挂了!"
description: ""
category: 程序语言
tags: [java, executor, thread]
---

今天我要讨论一个工作中遇到的bug,该bug导致我们系统的一个java线程挂掉,使得相应的模块没有正常工作,当然,也给公司造成了一定的损失.作为程序员,我们重点应该关注的是java线程挂掉的原因,以免在以后的工作中犯同样的错误,给公司造成更大的损失.此外,我觉得这个问题的定位过程也很有意思,所以拿出来与大家分享.

在接下来的章节中,我将首先描述问题现场(见第1节);然后详细分析问题定位的过程(见第2节),并在分析过程中给出定位问题的一些指导原则;我将在第3节中给出java线程挂掉的原因.最后,在第4节中对问题进行了简单总结.

那么,就跟我一起来解决问题吧！

## 1. 问题现场描述

有同事反映,我们RDS系统的线上实例不会进行扣费.由于当天该模块的开发者请假,所以就由我来跟这个问题.对问题进行了简单的了解,有以下几点信息:

1.  RDS系统的扣费的实现是,由RDS系统将实例的信息按照指定的格式打印到日志文件,由计费平台统一拉取日志进行扣费
2.  之所有没有进行扣费,是因为计费相关的日志没有打印,计费平台获取不到相应的日志,自然就不会进行扣费
3.  该问题在4个月前出现过一次,相应的开发人员进行了定位,没有找出问题原因,重启系统问题立马就没有了,QA在线下也没有复现成功,所以问题一直没能解决

## 2. 问题定位过程

接下来,将详细描述问题定位的每一步,并且穿插给出分析、定位问题的指导原则.

### 2.1 调整心态

定位问题的第一步,就是调整心态.为什么这么说呢,首先,该模块不是我开发的,对该模块几乎一无所知.需要定位该模块的问题,第一反应是要花好多时间,关键我手里还有不少活啊！所以我的心情是这样的:

![宝宝心里苦](/cn/image/bb_ku.png)

但是,这是线上问题啊,时间就是金钱,必须马上解决！我当然可以把服务重启一下,然后等着相应的开发回来再分析定位.但是,那时候可能现场就不在了,可能就更难分析定位了,而且,我也不是逃避责任的人啊.

![这就是命](/cn/image/bb_ming.png)

分析定位问题指导原则1: **不要逃避问题,要勇敢面对困难,要相信,只要时间充裕,没有解决不了的问题**

### 2.2 空杯精神

按理说只是打印计费相关的日志,并没有进行什么实际的操作,出现问题的可能性不大啊.为啥就不打日志了呢.所以就显示胡乱猜测:

1. 是不是磁盘满了? 这个很好验证,事实上并没有
2. log4j配置不对? 检查了log4j的配置,并没有什么问题
3. 计费日志是单独的线程吗,是不是线程挂了?赶紧看看计费日志的实现,确实是使用单独的线程

有没有可能是因为计费这个线程挂了呢,可以猜测,但是需要进行验证.看看计费相关的代码:

        public class ChargeCheckTask extends Thread {
          Logger managerLog = Logger.getLogger("managerLog");
          @Override
          public void run() {
            List<DBInstance> allDb = Cache.cache.listDBs();
            managerLog.info("ChargeCheckTask start running! ");
            for (DBInstance dbInstance : allDb) {
            ......
            }
          }
        }


可以看到,计费线程每次运行的时候,都会打印"ChargeCheckTask start running!",每次退出还会打印"ChargeCheckTask exit!".那么,直接去日志里面搜这几个关键字就可以了.搜当天的日志,没有！搜前一天的日志,没有！搜再前一天的日志,也没有！

![java crash1](/cn/image/java_thread_crash1.png)

![java crash2](/cn/image/java_thread_crash2.png)


最后,还是通过日志发现,该线程最后一次运行时间是2016-04-24 23:43:38,那么,是不是从这个点开始就没有计费了呢?赶紧找了一个线上实例看了一下,确认了最后一次计费日期是2016-04-24.自此,可以断定,是由于计费线程挂了导致计费停止.


分析定位问题指导原则2: **保持空杯精神,进行各种猜测验证**

相应的开发没有定位出问题,而由我定位到了.我在想,可能是因为他对自己开发的功能太熟悉了,计费日志只是简单的打印打印日志,没有什么实际的操作,怎么可能导致计费线程挂掉呢.而我由于对这个模块不了解,更容易保持空杯精神,进行各种猜测和验证,所以发现了一些蛛丝马迹.

### 2.3 接受质疑

在我坚信是由于计费线程挂掉,导致计费停止以后,QA提出了质疑,怀疑是缓存问题.因为,QA说,上一次遇到这个问题的时候,他们发现老实例不计费,新建的实例依然还会计费.这个结论与我发现的事实就矛盾了,如果真的是计费线程挂了,应该所有实例都不计费才对！

既然大家都无法说服对方,就进行验证好了.找了一个2016-04-24以后建的实例,查看该实例的扣费信息,依然没有扣费.所以,可以确定,他们上一次没有定位出问题,是因为思路错了,甚至他们获取到的信息也是错的！

分析定位问题指导原则3: **以事实为依据,不要详细自己没有验证的结果,以免误导自己**

### 2.4 只打印日志也会有问题?

我们虽然确定了是计费线程挂了导致计费停止,但是,我们还不知道到底发生了什么,导致计费线程挂掉.这时候怎么办呢？当然是看计费线程挂掉之前到底在干什么!

通过日志可以看到,计费线程在最后打印了一行错误日志,如下所示:

![java crash3](/cn/image/java_thread_crash3.png)

这里有一个宝贵的信息,就是日志对应到了具体的代码行,可以看到CommandUtil.java:2921,到这一行代码,发现这行代码也没有干什么,只是判断该实例是否一个只读实例,这么简单的逻辑为什么会出错呢,难道是空指针？

这时候就开始猜测,有没有可能是因为在计费线程中获取了实例列表,当操作实例A的时候,A被删除了,所以A变成了一个空指针？当然,我们需要依据事实依据,因此,我们通过日志查看计费线程在挂掉之前正在打印哪个实例的计费日志,找到实例以后,我们查看该实例的操作日志,果不其然:

![java crash4](/cn/image/java_thread_crash4.png)

有人在相同的时间删除了这个实例,因此,触发了空指针异常.


## 3. java线程挂掉的真正原因

到这里,我们已经清楚了事情的来龙去脉了.计费线程每次打印计费日志的时候,会获取所有实例的实例列表.线程准备去操作实例A的时候,有人删除了实例A,因此,A这个对象就变成了一个空指针.由于计费线程没有对这种情况进行考虑,所以在打印实例A的计费日志的时候出错了.


我们来看一下计费线程的部分代码:

      try {
        product.getProductLock().writeLock().lock();
        ......
        Master.chargeSystemClient.dbInstanceCharge(dbInstance);
      } catch (Exception e) {
        managerLog.error("ChargeStatusTask: exception when print charge log" + e.getMessage());
        throw new RDSError(RDSError.UNKNOWN_ERROR, "ChargeStatusTask: exception when print charge log", RDSError.INNER_ERROR);
      } finally {
        product.getProductLock().writeLock().unlock();
      }

从代码上看,计费线程在执行操作的时候有去捕获异常,那么,是不是存在没有捕获到异常呢？例如,有两个异常ExceptionA和ExceptionB,如果用户捕获了ExceptionA,没有捕获ExceptionB,则依然会继续往上抛.但是,如果ExceptionA是ExceptionB的父类,则`catch ExceptionA`能够捕获ExceptionA和ExceptionB.有没有可能是这种情况呢？可以看一下java的异常类层次结构:

![exception](/cn/image/ExceptionHierarchy.png)

明显,Exception异常是空指针异常的父类,因此,上面的代码是能够捕获空指针异常的,那么,为什么线程会挂呢？

分析定位问题指导原则4: **一定要刨根问题,不要放弃学习的机会,才能成为大牛**

我们仔细观察一下上面这段代码,这段代码捕获了异常,然后打印日志,然后抛出新的异常.我们知道,如果没有任何人捕获异常,日志里面相应的异常应该是这样的:

        at com.netease.backend.rds.task.CleanHandleThread.run(CleanHandleThread.java:65)
        at java.util.concurrent.Executors$RunnableAdapter.call(Executors.java:439)
        at java.util.concurrent.FutureTask$Sync.innerRunAndReset(FutureTask.java:317)
        at java.util.concurrent.FutureTask.runAndReset(FutureTask.java:150)
        at java.util.concurrent.ScheduledThreadPoolExecutor$ScheduledFutureTask.access$101(ScheduledThreadPoolExecutor.java:98)
        at java.util.concurrent.ScheduledThreadPoolExecutor$ScheduledFutureTask.runPeriodic(ScheduledThreadPoolExecutor.java:180)
        at java.util.concurrent.ScheduledThreadPoolExecutor$ScheduledFutureTask.run(ScheduledThreadPoolExecutor.java:204)
        at java.util.concurrent.ThreadPoolExecutor$Worker.runTask(ThreadPoolExecutor.java:895)
        at java.util.concurrent.ThreadPoolExecutor$Worker.run(ThreadPoolExecutor.java:918)
        at java.lang.Thread.run(Thread.java:662)

奇怪的是,这里抛出了一个异常以后,并没有打印异常的堆栈？这是为什么呢？

为了回答这个问题,我们还得看看是谁调用了这个线程,或者说,这个线程是怎么运行的.

      Thread chargeCheckThread = new ChargeCheckTask();
      taskExecutor.scheduleWithFixedDelay(chargeCheckThread, a, b, TimeUnit.SECONDS);

可以看到,线程是由java的`ScheduledExecutorService`执行框架执行的,那么,有没有可能与`ScheduledExecutorService`处理异常的方式有关呢？

这时候用google搜一下"scheduledexecutorservice exception handling"就能够得到想要的结果了,排名第一的答案是[这个][1].

那么,java线程挂掉的主要原因是:**Any thrown exception or error reaching the executor causes the executor(ScheduledExecutorService) to halt**. No more invocations on the Runnable, no more work done. This work stoppage happens silently, you'll not be informed.

也就是说,如果使用者抛出异常,ScheduledExecutorService 将会停止线程的运行,而且不会报错,没有任何提示信息.也是一个坑,可以看到有不少人被坑过,[这个外国哥们][2]被坑以后就很激动,忍不住的脏话.

感兴趣的同学可以进行简单的[测试][2]:

![executor](/cn/image/executor.png)

## 4. 总结

本文详细的分析了一个线上bug的定位过程,穿插的介绍了一些分析定位问题的指导原则,原则汇总如下:

* 分析定位问题指导原则1: 不要逃避问题,要勇敢面对困难,要相信,只要时间充裕,没有解决不了的问题.
* 分析定位问题指导原则2: 保持空杯精神,进行各种猜测验证
* 分析定位问题指导原则3: 以事实为依据,不要详细自己没有验证的结果,以免误导自己
* 分析定位问题指导原则4: 一定要刨根问题,不要放弃学习的机会,才能成为大牛

其实，这个bug还是有很多机会避免的:

1. 该系统中的其他线程,都存在获取了实例列表以后,在具体使用时,再次判断实例是否null,以避免线程运行过程中实例被删除的情况,但是计费线程并没有这么做
2. 在对实例进行操作过程中,没有对实例加锁,正确的做法应该对实例加锁,以免实例被删除
3. 如果底层代码抛出了异常,捕获异常以后,就new一个新的UNKNOW 异常,其实没有什么必要,反而让异常显得不够清晰,正确的做法要么捕获异常以后进行处理,要么就直接不捕获异常
4. 需要对java的Executors进行深入的了解,才能最终避免出现类似问题

总之，大家应该充分地掌握自己所用到的知识，在出现问题以后，大胆猜测，仔细推敲，就没有解决不了的问题。更重要的，要敢于承担责任！


[1]: http://stackoverflow.com/questions/6894595/scheduledexecutorservice-exception-handling
[2]: http://code.nomad-labs.com/2011/12/09/mother-fk-the-scheduledexecutorservice/
