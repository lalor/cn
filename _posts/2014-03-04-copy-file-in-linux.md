---
layout: post
title: "Linux下的常用文件传输方式介绍与比较"
description: "copy file"
category: 工具
tags: [ftp, nc, rsync, ssh, scp]
---

本文介绍了linux之间传输文件的几种方式，并通过具体实验测试了几种文件传输方式之间的传输速度。这篇文章是我一次作业的实验报告，我经常查看这个文档，所以贴出来方便自己查略。

###0. 实验环境以及实验数据

实验环境: 两台装有Ubuntu的电脑，两台电脑位于同一个局域网中，传输速度约4.1MB/s。

实验数据: 使用MySQL的日志文件(ib_logfile0)进行测试，日志文件压缩前1.1G，压缩后159M，具体应用中，压缩比例可能没有这么高，但是不影响我们的讨论。

###1. scp

scp是secure copy的缩写，scp是linux系统下基于ssh登陆进行安全的远程文件拷贝命令，主要用于linux服务器之间复制文件和目录。scp使用ssh安全协议传输数据，具有和ssh一样的验证机制，从而可以实现安全的远程拷贝文件。

下面介绍SCP三种不同用法的效率。**注意：**使用SCP前请配制好SSH。

##### 1.1 scp 不使用压缩

将本地文件拷贝到远程服务器：

    scp -P port ufile user@host:~/ufile

将远程服务器中的文件拷贝到本地的用法：

    scp -P port user@host:~/ufile ufile

经测试，SCP不启用压缩功能的情况下，传输ib_logfile0文件需要4:12s。

##### 1.2 压缩后传输

SCP传输ib_logfile0文件之所以需要那么多时间，是因为它没有对传输的数据进行压缩，可以先
将文件压缩，然后再进行远程拷贝。如下所示：

    tar -zcf ufile.tar.gz ufile
    scp -P port ufile.tar.gz user@host:~/ufile.tar.gz
    ssh -p port user@host 'tar -zxf ufile.tar.gz'

经测试，先手动压缩，然后再传输，4次测试结果的平均值为00:39s。

##### 1.3 scp启用压缩

相对于第一种方法，第二种方法极大地减少了传输时间，但是需要执行三条语句，较为麻烦。更简单的方法如下所示：

    scp -P port -C ufile user@host:~/ufile

`-C`选项启用了SSH的压缩功能，通过`man ssh`可以看到，在-C选项的解释部分有这么一句话："the compression algorithm is the same used by gzip"。SSH与gzip使用的是同一种压缩算法，即第二种方法与第三种方法几乎一样，但是，第三种方法更为简单方便，所以，推荐使用第三种方法传输数据。

第三种方法传输ib_logfile0花费了00:52s，比第二种方法多花了10秒，这10秒主要用于数据的压缩和解压。

此外，值得注意的是，修改压缩算法的压缩比对总的数据传输时间影响不大，这是因为更高的压缩比例，需要的压缩时间也更多。

### 2. sftp

sftp是Secure File Transfer Protocol的缩写，安全文件传送协议，可以为传输文件提供一种安全的加密方法。sftp与ftp有着几乎一样的语法和功能，不过SFTP是SSH的一部分，它使用加密传输认证信息和传输的数据，所以，使用SFTP是非常安全的。但是，由于这种传输方式使用了加密、解密技术，所以传输效率比普通的FTP要低一些。

上传数据到服务器上：

    echo progress; echo "put ufile"; echo quit) | sftp -o Compression=yes -o Port=port user@host -b

从服务器上下载数据：

    echo progress; echo "get ufile"; echo quit) | sftp -o Compression=yes -o Port=port user@host -b

在我的实验中，使用sftp传输ib_logfile0文件花费了1:05s。比SCP略慢。

### 3. 直截使用ssh

将本地的数据传输到远程服务器的用法：

    gzip -c ufile | ssh -p port user@host 'gunzip >ufile'

将远程服务器传输到本地的用法

    ssh -p port user@host "gzip -c ufile" | gunzip -c > ufile

使用这种方式传输ib_logfile0需要00:55s，相对于`scp -C`慢了几秒，不过相差不大。但是，scp用法更简单一些，只需要加一个`-C`参数，而不用手动调用gzip压缩程序。

### 4. nc

netcat(简称nc）是网络工具中的瑞士军刀，它能通过TCP和UDP在网络中读写数据。通过与其他工具结合和重定向，你可以在脚本中以多种方式使用它。使用netcat命令所能完成的事情令人惊讶。

netcat所做的就是在两台电脑之间建立链接并返回两个数据流，在这之后所能做的事就看你的想像力了。你能建立一个服务器，传输文件，与朋友聊天，传输流媒体或者用它作为其它协议的独立客户端。我们这里只讨论nc的用于数据传输的情况。

在服务器端：

    sudo nc -l -p port | tar -zxf - #l 参数用于监听
    sudo nc -l -p port > ufile

在客户端：

    tar -zcf - ufile | sudo nc host port
    sudo nc host port < ufile

使用nc传输ib_logfile0文件需要00:49s，比`SCP -C`略快，这是因为SCP需要对数据进行加密，而nc只是简单的传输数据。

可以看到，nc不需要任何配置操作，使用也非常简单，不过nc需要root权限。

### 5. rsync

rsync(remote sync)是类unix系统下的数据镜像备份工具，从软件的命名上就可以看出来，它主要用于数据的同步备份。

rsync有两种用法，一种是通过SSH通道传输数据，另一种是通过与服务器的rsync守护者(daemon)进程建立连接来传输数据。下面对这两种情况进行测试。

##### 5.1 rsync with ssh

rsync使用SSH通道传输数据时，配置比较简单，只要配置好了SSH，就直接可用，不需要额外的操作，正是这个原因，很多人更喜欢使用这种方式来使用rsync。

使用方法如下：

    rsync -zav --rsh='ssh -p port' ufile user@host:path

rsync 使用SSH 通道传输ib_logfile0花费了00:55s，与直接使用SSH传输数据时间一样。

##### 5.1 rsync with daemon

不同于SCP和SFTP，rsync是一套独立的软件，除了通过SSH通道传输数据以外，还可以通过rsync的守护者进程进行数据传输。

这里只是对rsync的数据传输的性能进行简单的测试，关于rsync的配置，可以参考其他资料。

rsync 的使用方法如下：

    rsync -avz ufile user@host::module_name

在实验中，rsync传输ib_logfile0共花费了00:51s，与`scp -C`差不多。此外，值得注意的是，rsync使用zlib压缩算法压缩数据。ssh与gzip使用的是LZ77算法。

rsync的参数特别多，详细的使用方法可以参考[这里](http://www.howtocn.org/rsync:use_rsync)。


### 6. ftp

FTP命令使用文件传输协议（File Transfer Protocol， FTP）在本地主机和远程主机之间或者在两个远程主机之间进行文件传输。

ftp的使用方法就不再介绍了，在我们的实验中，使用FTP传输ib_logfile0共花费了4:25s，这是因为，FTP没有数据压缩的功能。从前面的实验环境中可以看到，数据压缩能够显著减少传输的数据量，数据不能压缩，再好的传输工具也会受限于网络传输速率。

### 7. 结论

单纯从数据传输来看，我个人比较喜欢SCP，也强烈推荐SCP，不过，千万要记住，使用SCP时一定要使用`-C`选项，即启用压缩功能。从前面的实验中也可以看到，启用压缩与不启用压缩传输的数据量相差很多。

此外，以上几种数据传输方式也有各自的应用场景，其中，nc使用最简单，配置也最方便，但是需要root权限，在有root权限没有配置SSH的情况下，推荐使用nc。

rsync配置相对比较复杂，它的主要功能是进行增量备份而不是数据传输，在需要增量备份的情况下，毫不犹豫地选择rsync。

FTP配置也比较麻烦，传输速度也很一般，而且，FTP没有自带压缩功能，需要手动压缩，但是，FTP是最常用的一种数据传输方式，大多数普通用户都使用过FTP，对于用户来说，学习成本较低，在需要将文件传送给多个用户时，推荐使用FTP。

SFTP是建立在SSH上的FTP，传输速度也很快，与SCP不分伯仲，但是SFTP可以交互式的使用，在某些情况下可能会比较有用，而且，如果已经配置好了SSH，SFTP的配置成本为零，即无需配置，直接可用。

总的来说，每种传输方式都各有千秋，我们应当根据自己的实际需要，选择适合的文件传输方法。
