---
layout: post
title: "推荐几本学习MySQL的好书"
description: ""
category: 数据库
tags: [MySQL, 数据库]
---

虽然我自己目前还不是大牛，不过正走在成为大牛的路上。我个人比较幸运，实习的时候在登博(何登成）手下实习，从登博那里学到不少东西，毕业以后，有幸投奔另一位MySQL大牛姜sir（姜承尧），目前还在姜sir手下工作，对目前的工作状态比较满意(我们组持续招人中)，这两位都是技术实力强，却又非常好相处，非常平易近人的大牛，让我打内心欣赏、崇拜，真希望自己能够达早日到他们的水平。相信不是所有人都有我这么幸运。对于想在数据库层面深入的朋友，我这里推荐几本MySQL的好书，应该能够有效避免学习MySQL的弯路，并且达到一个不错的水平。

我这里推荐的书或材料分为两个部分，分别是MySQL的使用和MySQL的源码学习。在介绍的过程中，我会穿插简单的评语或感想。

## 1.MySQL的使用

#### 1.1 MySQL技术内幕:InnoDB存储引擎

学习MySQL的使用，首推姜承尧的《MySQL技术内幕:InnoDB存储引擎》，当然不是因为姜sir是我的经理才推荐这本书。这本书确实做到了由渐入深、深入浅出，是中国人写的最赞的MySQL技术书籍，符合国人的思维方式和阅读习惯，而且，这本书简直就是面试宝典，对于近期有求职MySQL相关岗位的朋友，可以认真阅读，对找工作有很大的帮助。当然，也有人说这本书入门难度较大，这个就自己取舍了，个人建议就以这本书入门即可，有不懂的地方可以求助官方手册和google。

![MySQL技术内幕](/cn/image/mysql-book-1.jpg)

#### 1.2 MySQL的官方手册

我刚开始学习MySQL的时候误区就是，没有好好阅读MySQL的官方手册。例如，我刚开始很难理解InnoDB的锁，尤其是各个情况下如何加锁，这个问题在我师弟进入百度做DBA时，也困扰了他一阵子，我们两还讨论来讨论去，其实，MySQL官方手册已经写得清清楚楚，什么样的SQL语句加什么样的锁，当然，MySQL的官方手册非常庞大，一时半会很难看完，建议先看InnoDB相关的部分。

<http://dev.mysql.com/doc/refman/5.7/en/innodb-storage-engine.html>

#### 1.3 MySQL排错指南

《[MySQL排错指南][1]》是2015年夏天引入中国的书籍，这本书可以说是DBA速成指南，介绍的内容其实比较简单，但是也非常实用，对于DBA这个讲究经验的工种，这本书就是传授经验的，可能对有较多工作经验的DBA来说，这本书基本没有什么用，但是，对于刚入职场的新人，或学校里的学生，这本书会有较大的帮助，非常推荐。

![MySQL排错指南](/cn/image/mysql-book-2.jpg)

#### 1.4 高性能MySQL

《[高性能MySQL][2]》是MySQL领域的经典之作，拥有广泛的影响力，学习MySQL的朋友都应该有所耳闻，所以我就不作过多介绍，唯一的建议就是仔细看、认真看、多看几遍，我每次看都会有不小的收获。这就是一本虽然书很厚，但是需要一页一页、一行一行都认真看的书。

![高性能MySQL](/cn/image/mysql-book-3.jpg)

#### 1.5 数据库索引设计与优化

如果认真学习完前面几本书，基本上都已经对MySQL掌握得不错了，但是，如果不了解如何设计一个好的索引，仍然不能成为牛逼的DBA，牛逼的DBA和不牛逼的DBA，一半就是看对索引的掌握情况，《[数据库索引设计与优化][3]》就是从普通DBA走向牛逼DBA的捷径，这本书在淘宝内部非常推崇，但是在中国名气却不是很大，很多人不了解。这本书也是今年夏天刚有中文版本的，非常值得入手以后跟着练习，虽然知道的人不多，豆瓣上也几乎没有什么评价，但是，强烈推荐、吐血推荐！

![数据库索引设计与优化](/cn/image/mysql-book-4.jpg)

#### 1.6 Effective MySQL系列

《[Effective MySQL系列][4]》是指:

* Effective MySQL Replication Techniques in Depth
* Effective MySQL之SQL语句最优化
* Effective MySQL之备份与恢复

![effective](/cn/image/mysql-book-5.jpg)

这一系列并不如前面推荐的好，其中，我只看了前两本，这几本书只能算是小册子，如果有时间可以看看，对某一个"模块"进入深入了解。

## 2.MySQL的源码

关于MySQL源码的书非常少，还好现在市面上有两本不错的书，而且刚好一本讲server层，一本讲innodb存储引擎层，对于学习MySQL源码会很有帮助，至少能够更加快速地了解MySQL的原理和宏观结构，然后再深入细节。此外，还有一些博客或PPT将得也很不错，这里推荐最好的几份材料。

#### 2.1 InnoDB - A journey to the core

《[InnoDB - A journey to the core][5]》 是MySQL大牛Jeremy Cole写的PPT，介绍InnoDB的存储模块，即表空间、区、段、页的格式、记录的格式、槽等等。是学习Innodb存储的最好的材料。感谢Jeremy Cole!

#### 2.2 深入MySQL源码

登博的分享《[深入MySQL源码][6]》，相信很多想了解MySQL源码的朋友已经知道这份PPT，就不过多介绍，不过，要多说一句，登博的参考资料里列出的几个博客，都要关注一下，干货满满，是学习MySQL必须关注的博客。

#### 2.3 深入理解MySQL核心技术

《[深入理解MySQL核心技术][7]》是第一本关于MySQL源码的书，着重介绍了MySQL的Server层，重点介绍了宏观架构，对于刚开始学习MySQL源码的人，相信会有很大的帮助，我在学习MySQL源码的过程中，反复的翻阅了几遍，这本书刚开始看的时候会很痛苦，但是，对于研究MySQL源码，非常有帮助，就看你是否需要，如果没有研究MySQL源码的决心，这本书应该会被唾弃。

![深入理解MySQL核心技术](/cn/image/mysql-book-6.jpg)

#### 2.4 MySQL内核：InnoDB存储引擎

我们组的同事写的《[MySQL内核：InnoDB存储引擎][8]》，可能宇宙范围内这本书就数我学得最认真了，虽然书中有很多编辑错误，但是，平心而论，还是写得非常好的，相对于《深入理解MySQL核心技术》，可读性更强一些，建议研究Innodb存储引擎的朋友，可以了解一下，先对Innodb有一个宏观的概念，对大致原理有一个整体的了解，然后再深入细节，肯定会比自己从头开始研究会快很多，这本书可以帮助你事半功倍。

![MySQL内核](/cn/image/mysql-book-7.jpg)

#### 2.5 MySQL Internals Manual

《[MySQL Internals Manual][9]》相对于MySQL Manual来说，写的太粗糙，谁让人家是官方文档呢，研究MySQL源码的时候可以简单地参考一下，但是，还是不要指望文档能够回答你的问题，还需要看代码才行。

<http://dev.mysql.com/doc/internals/en/>

#### 2.6 MariaDB原理与实现

评论里提到的《[MariaDB原理与实现][10]》我也买了一本，还不错，MariaDB讲的并不多，重点讲了Group Commit、线程池和复制的实现，都是MySQL Server层的知识，对MySQL Server层感兴趣的可以参考一下。

![MariaDB](/cn/image/mysql-book-8.jpg)

## 3. 后记

希望这里推荐的材料对学习MySQL的同学、朋友有所帮助，也欢迎推荐靠谱的学习材料，大家共同进步。

[1]: http://book.douban.com/subject/26591051/
[2]: http://book.douban.com/subject/23008813/
[3]: http://book.douban.com/subject/26419771/
[4]: http://book.douban.com/subject/11653424/
[5]: https://www.percona.com/live/mysql-conference-2013/sites/default/files/slides/InnoDB%20-%20A%20journey%20to%20the%20core%20-%20PLMCE%202013.pdf
[6]: http://hotpu-meeting.b0.upaiyun.com/2014dtcc/post_pdf/hedengcheng.pdf
[7]: http://book.douban.com/subject/4022870/
[8]: http://img4.douban.com/lpic/s27266366.jpg
[9]: http://dev.mysql.com/doc/internals/en/
[10]: https://book.douban.com/subject/26340413/
