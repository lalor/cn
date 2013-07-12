---
layout: post
title: "Linux Performance Analysis and Tools(Linux性能分析和工具)"
description: ""
category: 工具
tags: [iostat, vmstat, mpstat, netstat, tcpdump, dstat]
---

首先来看一张图：
<a href="http://imgur.com/pKopgkk"><img src="http://i.imgur.com/pKopgkk.png" title="Hosted by imgur.com"/></a>

上面这张神一样的图出自国外一个Lead Performance Engineer(Brendan Gregg)的一次[分享][1]，几乎涵盖了一个系统的方方面面，任何人，如果没有完善的计算系统知识，网络知识和操作系统的知识，这张图中列出的工具，是不可能全部掌握的。

出于本人对linux系统的极大兴趣，以及对底层知识的强烈渴望，并作为检验自己基础只是的一个指标，我决定将这里的所有工具学习一遍（时间不限），这篇文章将作为我学习这些工具的学习笔记。尽量组织得方便自己日后查阅，并希望对别人也有一定帮助。

这里所有的工具，都可以通过man 获得它的帮助文档，这里只介绍一些常规用法。

**注意：**文中所有的interval都是指收集数据的间隔时间，times是指采样次数。例如

	vmstat interval times

将命令中的interval和times替换成数字：

	vmstat 3 5

上面语句的意思就是每3秒中输出一次内存统计数据，共输出5次。

Tools: Basic & Intermediate

1. <a href="#vmstat" > vmstat  </a>
2. <a href="#iostat" > iostat  </a>
3. <a href="#dstat"  > dstat   </a>
4. <a href="#mpstat" > mpstat  </a>
5. <a href="#netstat"> netstat </a>
6. <a href="#strace" > strace  </a>
7. <a href="#uptime" > uptime  </a>

7. <a href="#top" > uptime  </a>
7. <a href="#free" > uptime  </a>
7. <a href="#ping" > uptime  </a>
7. <a href="#nicstat" > uptime  </a>
7. <a href="#sar" > uptime  </a>
7. <a href="#pidsta" > uptime  </a>
7. <a href="#tcpdump" > uptime  </a>
7. <a href="#blktrace" > uptime  </a>
7. <a href="#iotop" > uptime  </a>
7. <a href="#slabtop" > uptime  </a>
7. <a href="#sysctl" > uptime  </a>

Tools: Advanced

1. <a href="#perf" > perf  </a>
2. <a href="#DTrace" > Dtrace  </a>
3. <a href="#SystemTap" > SystemTap </a>

<a name="vmstat"></a>
#vmstat

我们先看一个vmstat 的例子。用下面的命令让它每5秒中打印一个报告。
<a href="http://imgur.com/cWzr7lv"><img src="http://i.imgur.com/cWzr7lv.png" title="Hosted by imgur.com"/></a>
可以用ctrl+c停止vmstat。vmstat的常规用法是`vmstat interval times`，即每个interval秒采
样一次，共采样times次，如果省略times，则一直采集数据到用户手动停止。

第一行的值是显示了自系统启动以来的平均值，第二行开始展示现在正在发生的情况，接下来的行会显示每5秒的间隔内发生了什么。每一列的含义在头部，如下所示：

* procs

r这一列显示了多少进程正在等待cpu，b列显示多少进程正在不可中断的休眠（通常意味着它们在等待IO
，例如磁盘，网络，用户输入，等等）。

* memory

swapd列显示了多少块被换出到了磁盘（页面交换）。剩下的三个列显示了多少块是空闲的（未被使用），多少块正在被用作缓冲区，以及多少正在被用作操作系统的缓存。

* swap

这些列显示页面交换活动：每秒有多少块正在被换入（从磁盘）和换出（到磁盘）。它们比监控swpd列重要多了。<font color=#FF0000>大部分时间我们希望看到si 和so 列是0,并且我们很明确不希望看到每秒超过10个块</font>。

* io

这些列显示了多少块从块设备读取(bi)和写出(bo)。这通常反映了硬盘I/O。

* system

这些列显示了每秒中断(in)和上下文切换(cs)的数量。<font color=#FF0000>除非上下文切换超过100 000次或更多，一般不用担心上下文切换。</font>

* cpu

这些列显示所有的CPU时间花费在各类操作的百分比，包括执行用户代码（非内核），执行系统代码（内核），空闲以及等待IO。如果正在使用虚拟化，第5列可能是st，显示了从虚拟机中"偷走"的百分比。


<a name="iostat"></a>
#iostat

现在让我们转移到iostat 。默认情况下，它显示了与vmstat 相同的CPU
使用信息。我们通常只对I/O统计感兴趣，所以使用下面的命令只展示扩展的设备统计。
<a href="http://imgur.com/L6Ko2Mp"><img src="http://i.imgur.com/L6Ko2Mp.png" title="Hosted by imgur.com"/></a>

与vmstat
一样，第一行报告显示的是自系统启动以来的平均值，（通常删掉它节省空间），然后接下来的报告显示了增量的平均值，每个设备一行。

有多种选项显示和隐藏列。官方文档有点难以理解，因此我们必须从源码中挖掘真正显示的内容是什么。说明的列信息如下：

为了看懂Linux的磁盘IO指标，先了解一些常见的缩写习惯：rq是request，r是read，w是write，qu是queue，sz是size的，a是average，tm是time，svc是service。

* rrqm/s 和 wrqm/s

	每秒合并的读和写请求。"合并的"意味着操作系统从队列中拿出多个逻辑请求合并为一个请求到实际磁盘。

* r/s 和 w/s

	每秒发送到设备的读和写请求。

* rsec/s 和 wsec/s

	每秒读和写的扇区数。有些系统也输出为rKB/s 和 wKB/s
	，意味每秒读写的千字节数。

* avgrq-sz

	请求的扇区数。

* avgqu-sz

	在设备队列中等待的请求数。

* await

	磁盘排队上花费的毫秒数。很不幸，iostat
	没有独立统计读和写的请求，它们实际上不应该一起平均。

* svctm

	服务请求花费的毫秒数，不包括排队时间。

* %util

	至少有一个活跃请求所占时间的百分比。

这里有一篇关于iostat的，讲得非常详细且深刻的[文章][3]。

##cpu 密集型机器

cpu 密集型服务器的vmstat 输出通常在us
列会有一个很高的值，报告了花费在非内核代码上的cpu 时钟；也可能在sy
列有很高的值，表示系统cpu 利用率，超过20%
就足以令人不安了。在大部分情况下，也会有进程队列排队时间（在r列报告的）。下面是一个列子：
<a href="http://imgur.com/T0oRNWf"><img src="http://i.imgur.com/T0oRNWf.png" title="Hosted by imgur.com"/></a>

如果我们在同一台及其上观察iostat
的输出（再次剔除显示启动以来平均值的第一行），可以发现磁盘利用率低于50%：
<a href="http://imgur.com/ZiY7WpY"><img src="http://i.imgur.com/ZiY7WpY.png" title="Hosted by imgur.com"/></a>

这台机器不是IO密集型的，但是依然有相当数量的IO发生，在数据库服务器中这种情况很少见。另一方面，传统的Web 服务器会消耗掉大量的CPU 资源，但是很少发生IO,所以Web 服务器的输出不会像这个例子。

##IO 密集型机器

在IO密集型工作负载下，CPU花费大量时间在等待IO请求完成。这意味着vmstat
会显示很多处理器在非中断休眠(b列)状态，并且在wa 这一列的值很高，下面是个例子：
<a href="http://imgur.com/K1MG6Mf"><img src="http://i.imgur.com/K1MG6Mf.png" title="Hosted by imgur.com"/></a>
这台机器的iostat 输出显示硬盘一直很忙：
<a href="http://imgur.com/1dXANQZ"><img src="http://i.imgur.com/1dXANQZ.png" title="Hosted by imgur.com"/></a>

%util的值可能因为四舍五入的错误超过100%。什么迹象意味着机器是IO密集的呢？只要有足够的缓冲来服务写请求，即使机器正在做大量的写操作，也可能可以满足，但是却通常意味着硬盘可能会无法满足读请求。这听起来好像违反直觉，但是如果思考读和写的本质，就不会这么认为了：

* 写请求能够缓冲或同步操作。
* 读请求就其本质而言都是同步的。当然，程序可以猜测到可能需要某些数据，并异步地提前读取（预读）。无论如何，通常程序在继续工作前必须得到它们需要的数据。这就强制读请求为同步操作：程序必须阻塞直到请求完成。

想想这种方式：你可以发出一个写请求到缓冲区的某个地方，然后过一会完成。甚至可以每秒发出很多这样的请求。如果缓冲区正确工作，并且有足够的空间，每个请求都可以很快完成，并且实际上写到物理硬盘是被重新排序后更有效地批量操作的。然而，没有办法对读操作这么做————不管多小或多少的请求，都不可能让硬盘响应说："这是你的数据，我等一会读它"。这就是为什么读需要IO等待是可以理解的原因。

##发生内存交换的机器

一台正在发生内存交换的机器可能在swpd
列有一个很高的值，也可能不高。但是可以看到si 和 so
列有很高的值，这是我们不希望看到的。下面是一台内存交换严重的机器的vmstat 输出：
<a href="http://imgur.com/qLzTzsY"><img src="http://i.imgur.com/qLzTzsY.png" title="Hosted by imgur.com"/></a>

##空闲的机器

下面是一台空闲机器上的vmstat输出。可以看到idle列显示CPU是100%的空闲。
<a href="http://imgur.com/RZPIWHJ"><img src="http://i.imgur.com/RZPIWHJ.png" title="Hosted by imgur.com"/></a>


<a name="dstat"></a>
#dstat

dstat 显示了cpu使用情况，磁盘io 情况，网络发包情况和换页情况。

个人觉得，iostat 和vmstat 的输出虽然详细，但是不够直观，不如dstat
好用。而且，dstat输出是彩色的，可读性更强。现在dstat作为我首选的，查看系统状态的工具。

dstat使用时，直接输入命令即可，不用任何参数。也可以通过指定参数来显示更加详细的信息。

	dstat -cdlmnpsy

![dstat](http://coolshell.cn//wp-content/uploads/2012/07/dstat_screenshot.png)
	

<a name="mpstat"></a>
#mpstat

mpstat 用来统计多核处理器中，每一个处理器的使用情况。mpstat使用方法很简单，常见用法如下：

	mpstat -P ALL interval times

不出意外，读者执行上面的命令，看不出个所以然来，试试在运行下面这条命令的同时，查看mpstat的输出。

	sysbench --test=cpu --cpu-max-prime=20000 run

**注意：**sysbench是一个基准测试工具，可以用来测试cpu,io,mutex,oltp等。在Debain系统下，
通过下面的命令安装：

	sudo apt-get install sysbench 


<a name="netstat"></a>
#netstat

netstat用于显示与IP、TCP、UDP和ICMP协议相关的统计数据，一般用于检验本机各端口的网络连接情况。

就我自己而言，经常使用的用法如下：

	netstat -npl

上面这条命令可以查看你要打开的端口是否已经打开，以此来作为程序是否已经启动的依据。

netstat 还有很多其他的用法，在《TCP/IP详解》一书中，作者喜欢用netstat命令来打印路由表，使用方法如下：

	netstat -rn

其中，flag域的解释如下：

* U 该路由可用
* G 该路由是到一个到网关（路由器）。如果没有设置该标志，说明目的地址是直接相连的
* H 该路由是到一个主机
* D 该路由是由重定向报文创建
* M 该路由已被重定向报文修改

netstat 也可以提供系统上的接口信息：

	netstat -in

这个命令打印每个接口的MTU,输入分组数，输入错误，输出分组数，输出错误，冲突以及当前的输出队列的长度。


<a name="strace"></a>
#strace

用我自己的理解来介绍strace:  

我们写程序，会调用很多库函数，而这些库函数，只是对系统调用的封装，它们最后都会去调用操
作系统提供的系统调用，通过系统调用去访问硬件设备。strace
的作用就是显示出这些调用关系，让程序员知道调用一个函数，它到底做了些什么事情。当然，strace
远比上面的介绍灵活多变。

下面来看一个例子：

查看mysqld 在linux上加载哪种配置文件，可以通过运行下面的命令行：

	strace -e stat64 mysqld --print-defaults > /dev/null

strace太强大，内容也太多了，我找到一篇比较好的，介绍strace的文章，请点击[这里][2]。

<a name="uptime"></a>
#uptime

uptime是最最最最简单的工具了，但是也有值得讨论的地方，那就是它最后输出的三个数字是怎么得来的，代表什么意思？

这三个数字的意思是1分钟、5分钟、15分钟内系统的平均负荷，关于这些数字的意义和原理，推荐看阮一峰的文章《[理解linux系统负荷这里][4]》。

我想说的是，这三个数字我天天看，时时看，但是我从来不相信它。

此话怎讲呢？我之所以天天看，时时看，是因为我将这三个数字显示到了[tmux][5]
的[状态栏][6]上，所以，我任何时候都可以看到,而不用专门输入`uptime`这个命令。

为什么我不相信它呢，因为这几个数字只能说明有多少线程在等待cpu,如果我们的一个任务有很多线程，系统负载不是特别高，但是这几个数字会出奇的高，也就是不能完全相信uptime的原因。如果不信，可以执行下面这条语句，然后再看看uptime的输出结果。

    sysbench --test=mutex --num-threads=1600 --mutex-num=2048 \
                    --mutex-locks=1000000 --mutex-loops=5000 run

运行了5分钟以后，我的电脑上输出如下所示。需要强调的是，这个时候电脑一点不卡，看uptime来判断系统负载，跟听cpu风扇声音判断系统负载一样。哈。

     20:32:39 up 10:21,  4 users,  load average: 386.53, 965.37, 418.57

《Linux Performance Analysis and Tools》里面也说了，This is only useful as a
clue. Use other tools to investigate!

[1]:http://www.slideshare.net/brendangregg/linux-performance-analysis-and-tools
[2]:http://www.dbabeta.com/2009/strace.html
[3]:http://bhavin.directi.com/iostat-and-disk-utilization-monitoring-nirvana/
[4]:http://www.ruanyifeng.com/blog/2011/07/linux_load_average_explained.html
[5]:http://mingxinglai.com/cn/2012/09/tmux/
[6]:https://github.com/lalor/.git-dotfile/blob/master/tmux.conf
