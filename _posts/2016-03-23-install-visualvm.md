---
layout: post
title: "visualvm的安装笔记"
description: ""
category: 工具
tags: [java, visualvm]
---

**前言：**我要定位一个疑似java内存泄露的问题，所以了解了一下相关的工具，通过[这篇文章][1]，我选择了visualvm。这篇博客记录了安装和使用过程中踩过的一些坑，以备不时之需，以及，给遇到相同问题的朋友一点帮助。

------------

### 1. 工具的安装

不知道为什么，网络上将该工具统称为visualvm，其实，该工具在jdk的bin目录下实际的名字是`jvisualvm.exe`。visualvm包含在jdk内，不需要额外的安装，找到这个工具双击即可打开。

    C:\Program Files\Java\jdk1.8.0_66\bin\jvisualvm.exe

### 2. 连接到java进程

jvisualvm有两种不同的方式可以连接到远程的java进程:

1. jstatd: 需要在远程开启一个jstatd进程，并且不能发挥jvisualvm的所有能力
1. jmx: 需要重启远程的java进程，对于部分应用可能不能接受

#### 2.1 使用jstatd连接到远程

使用jstatd特别简单，参考[这里][2]就可以安装成功了，基本思路就是创建一个策略文件，启动jstatd守护进程，打开1098端口，然后使用visualvm连接该端口。

如下所示：

    cat jstatd.all.policy

    grant codebase "file:${java.home}/../lib/tools.jar" {
    permission java.security.AllPermission;
    }

    jstatd -J-Djava.security.policy=jstatd.all.policy

上面的命令执行完成以后，就会开启一个1098端口，通过visualvm连接该端口即可。

#### 2.2 使用jmx连接到远程

使用jmx踩到了多个坑，也是写这篇博客的原因。

首先，为了使得visualvm能够连接上远程的java进程，需要修改tomcat的启动参数，增加以下几行：

    -Dcom.sun.management.jmxremote \
    -Dcom.sun.management.jmxremote.local.only=false \
    -Dcom.sun.management.jmxremote.ssl=false \
    -Dcom.sun.management.jmxremote.authenticate=false \
    -Djava.rmi.server.hostname=10.180.148.56 \
    -Dcom.sun.management.jmxremote.port=1024 \

在我的安装过程中，增加了`com.sun.management.jmxremote.port=1024`这一行以后tomcat就起不来了，并不是因为port已经被占用了，tomcat的日志如下：

    Error: Exception thrown by the agent : java.net.MalformedURLException: Local host name unknow: java.net.UnknownHostException: qa-control-helper-test2-manager: qa-control-helper-test2-manager

这里可以看到，提示里有UnknowHostException，并且，有提到hostname，因此，修改了`/etc/hosts`文件，增加一行

    root@qa-control-helper-test2-manager:~# cat /etc/hosts
    127.0.0.1   localhost
    127.0.1.1   localhost.localdomain   localhost
    127.0.0.1   qa-control-helper-test2-manager

修改完成以后启动tomcat，就可以看到启动了1024端口，这时候就可以通过visualvm连接远程的java进程了，但是，连接的时候一直报错。可以通过在终端打开日志的方式启动visualvm，以查看错误信息：

    jvisualvm -J-Dnetbeans.logger.console=true

错误信息如下：

    java.io.EOFException: SSL peer shut down incorrectly
    at sun.security.ssl.InputRecord.read(InputRecord.java:352)
    at sun.security.ssl.SSLSocketImpl.readRecord(SSLSocketImpl.java:927)
    Caused: javax.net.ssl.SSLHandshakeException: Remote host closed connection during handshake
    at sun.security.ssl.SSLSocketImpl.readRecord(SSLSocketImpl.java:946)
    at sun.security.ssl.SSLSocketImpl.performInitialHandshake(SSLSocketImpl.java:1328)
    at sun.security.ssl.SSLSocketImpl.writeRecord(SSLSocketImpl.java:702)
    at sun.security.ssl.AppOutputStream.write(AppOutputStream.java:122)
    at java.io.BufferedOutputStream.flushBuffer(BufferedOutputStream.java:82)
    at java.io.BufferedOutputStream.flush(BufferedOutputStream.java:140)
    at java.io.DataOutputStream.flush(DataOutputStream.java:123)
    at sun.rmi.transport.tcp.TCPChannel.createConnection(TCPChannel.java:229)
    Caused: java.rmi.ConnectIOException: error during JRMP connection establishment; nested exception is:

这个问题是因为SSl连接的时候出错，但是，我在建立jmx连接的时候已经选择了**不使用SSL**，这个问题google了好久都没有找到解决办法，遇到同类问题的朋友也不少，我也不知道怎么被我猜对了。就是`java.rmi.server.hostname`这个选项的值不是jvisual所在的主机的ip地址，而是tomcat所在的那台机器的ip地址，是visualvm连接的这个ip地址，太违反直觉了。

    +-------------------+                                    +-------------------+
    |       A           |                                    |       B           |
    +-------------------+                                    +-------------------+
    +-------------------+                                    +-------------------+
    |                   |                                    |                   |
    |                   +----------------------------------> |                   |
    |     jvisualvm     |                                    |      java         |
    |                   |                                    |                   |
    |                   |                                    |                   |
    +-------------------+                                    +-------------------+

### 3. visualvm的使用

visualvm的使用非常简单，都是一些图形化的东西，大家随便点点研究研究就学会了。这里简单提一下，我们可以在"监视"选项中选择`dump 堆`，dump堆以后对内存中的对象进行分析。这个功能很好用，与jmap和jhat类似。

jmap支持dump java进程的内存堆，如下所示：

    ./jmap -dump:format=b,file=dump.bin 25842

dump完成以后可以使用jhat分析堆：

    ./jhat dump.bin

jhat会启动servlet进程，并打开7000端口，我们可以在浏览器上查看jhat的分析结果。

### 4. 总结

本文简单地介绍了jvisualvm的使用过程中踩过的坑，jvisualvm还算使用比较广泛，安装起来居然这么不容易，差评！

[1]: http://www.cnblogs.com/amosli/p/3901794.html
[2]: https://theholyjava.wordpress.com/2012/09/21/visualvm-monitoring-remote-jvm-over-ssh-jmx-or-not/
