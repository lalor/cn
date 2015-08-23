---
layout: post
title: "MySQL源码注释与类图"
description: ""
category: 数据库
tags: [mysq、innodb]
---

在我学习[MySQL internal manual](http://dev.mysql.com/doc/internals/en/index.html)的时候，发现可以通过Doxygen产生MySQL的源代码注释和类图，生成的注释可读性非常高，尤其类图，MySQL内部这么多类，各个类之间的关系，不好好理理还真是不容易弄清楚，而且我最近正在学习MySQL的源码，为了学习源码，画了很多类图，很是花了不少时间。

我刚开始以为只是我不知道这个功能，然后google了一下，网上基本找不到Doxygen生成的MySQL注释的网页版，应该还有很多搞MySQL的同志不知道，所以我就将Doxygen生成的是静态html文件，放在github上作为一个站点，以便自己和别人随时随地查看。


有需要的同学，可以点击[这里](http://mingxinglai.com/mysql56-annotation)查看MySQL Server的注释和[这里](http://mingxinglai.com/innodb-annotation/)查看Innodb的注释。也可以直接输入`mingxinglai.com`，点击页面上的链接。


另外，像下面这种比较复杂的图，如果不是因为Doxygen能够产生Innodb的类图，自己画的话需要不少时间吧。

![img1](http://mingxinglai.com/innodb-annotation/structdict__table__t__coll__graph.png)
