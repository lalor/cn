---
layout: post
title: "MySQL的整体架构"
description: ""
category: 数据库
tags: [MySQL, 源码, 架构]
---


我最近正在学习MySQL源码，已经对MySQL的整体架构和工作原理有了一个较全面的、完整的了解，正在准备深入细节。

在我学习的过程中，发现脑中牢记MySQL的整体架构对我理解各个点具有莫大的帮助，MySQL非常复杂，研究的人也很多，相关的示意图、流程图和架构图也很丰富，在我看过比较丰富的学习材料里面，接下来要介绍的两张图对我学习MySQL帮助最大，如果有研究MySQL的朋友，可以参考一下。


**MySQL 的整体架构**

下图是MySQL模块的高层次视图，来自《[Understanding MySQL Internals][0]》的图1-1,我对该图进行了批注，主要是将书中提到的函数以注释的方式放在了图中，方便随时查看和思索，大家可以结合这张图和[这里][1]的介绍，了解一条SQL语句的执行过程。

由于MySQL的Server层代码都位于源代码的sql目录下，模块化并不如InnoDB规范，因此，作者根据函数的作用（而不是以文件）进行了模块划分。在《[Understanding MySQL Internals][0]》这本书的图1-1后面，详细的说明了每个模块包含哪些函数和每个模块的作用。

![img1](/cn/image/MySQL-architecture.png)


**InnoDB的整体架构**

由于InnoDB一开始和MySQL是一个独立的项目，两者的关系比较独立，因此，InnoDB的架构图可以单独拧出来看。InnoDB的模块划分则比MySQL Server层好很多，但是，由于InnoDB足够复杂，因此，刚开始研究的时候，还是很难搞清楚storage/innobase目录下的各个子目录的作用，以及各个模块之间的调用关系。脑海中牢记下面这种图，对学习InnoDB源码有莫大的帮助。

下面这张图来自《[InnoDB Concrete Architecture][2]》，对于图的详细说明，可以查看[这里][2]，或者看[MySQL官方文档][3]对每个目录的解释。

![img2](/cn/image/Innodb-architecture.png)


**参考资料：**

* Sasha Pachev. [Understanding MySQL Internals][0].
* Ryan Bannon, Alvin Chin, Faryaaz Kassam and Andrew Roszko. [InnoDB Concrete Architecture][2].
* Appendix B [InnoDB Source Code Distribution][3].
* [The Skeleton of the Server Code][1].

[0]: http://book.douban.com/subject/1924288/
[1]: http://dev.mysql.com/doc/internals/en/guided-tour-skeleton.html
[2]: http://www.swen.uwaterloo.ca/~mrbannon/cs798/assignment_02/innodb.pdf
[3]: http://dev.mysql.com/doc/internals/en/files-in-innodb-sources.html
