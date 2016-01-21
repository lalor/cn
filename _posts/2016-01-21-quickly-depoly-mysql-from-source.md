---
layout: post
title: "快速构建MySQL学习环境"
description: ""
category: 数据库
tags: [MySQL, 数据库]
---

**前言：**我开始学习MySQL的时候，写了两篇文章，[如何在windows下源码编译MySQL][1]和[如何在linux下源码编译MySQL][2]。但是，我现在已经不这么干了！因为，还是太麻烦了。对于新手，最快速的安装MySQL的方法可能就是`apt-get `了，这种方式最为简单，不需要太多的配置就能够构建一个MySQL环境，可是，如果需要部署不同的数据库版本呢(例如MySQL 5.7, MariaDB，Facebook的MySQL, twitter的MySQL或者是Percona)，进一步讲，如果想要编译一个debug版本的MySQL以便学习呢？

------------

本文介绍两个工具，用以快速构建MySQL的学习环境，让大家从安装MySQL的痛苦之中解脱出来。也让编译debug版本的数据库变得更加容易，希望根据我的学习经验，能够帮助各位学习MySQL道路上轻松愉悦一点。

### 用Docker快速部署MySQL

Docker大家应该听说过，可能你用过，也可能你没有用过，不管怎么样，赶紧用起来，赶紧用起来，赶紧用起来（重要的事情说了三遍）。

从Docker构建一个MySQL的学习环境实在是太容易了，并且，Docker可以构建任何数据库或者其他软件的学习环境，学会使用Docker，有助于学习任何其他代码的积极性，不会再因为安装太麻烦而放弃尝试新事物。

Docker的安装就不介绍了，可以参考[这里][3]。下面介绍如何通过Docker安装一个MySQL 5.7的数据库。

直接在google中搜`MySQL docker`就能够看到Docker官方提供的[MySQL镜像][4]，可以看到MySQL的版本和相应的环境变量，下面就是见证奇迹的时刻，只需要三条语句就能够初始化一个MySQL 5.7的实例，如果你的网速够快，整个过程一分钟就够了。

    ~# docker pull mysql:5.7
    ~# docker images
    REPOSITORY          TAG                 IMAGE ID            CREATED
    VIRTUAL SIZE
    mysql               5.7                 7fb43e0ace3a        2 days ago
    360.3 MB
    ~# docker run --name mysql57 -e MYSQL_ROOT_PASSWORD=root -p 7788:3306 -d 7fb43e0ace3a


上面三条命令的意思分别是：

1. 从官方的镜像库中获取MySQL 5.7的镜像
2. 查看本地的镜像，获取镜像ID
3. 启动镜像，由于这是一个MySQL的镜像，镜像启动以后会自动启动MySQL进程

上面三条语句顺利执行以后，我们就可以直接登录数据库了，如下所示：

![docker-mysql](/cn/image/docker-mysql.png)


### 用mysql_sandbox源码编译MySQL

接下来要介绍的是mysql_sandbox，这是一个专门为MySQL提供的工具，目的是快速部署MySQL的环境，支持部署主从、部署多个MySQL进程，支持从二进制包部署，也支持从MySQL源码部署。

这一部分将首先介绍mysql_sandbox的安装，然后介绍如何用mysql_sandbox从源码生成一个MySQL实例。

注意了，mysql_sandbox支持从源码部署MySQL，也就是说，如果想要debug MySQL，或者想要对MySQL进行修修改改立马看到效果，使用mysql_sandbox有助于提高学习热情和工作效率。

#### mysql_sandbox的安装

简单介绍一下mysql_sandbox的安装，语句如下：

    git clone https://github.com/datacharmer/mysql-sandbox.git
    cd mysql-sandbox
    perl Makefile.PL
    make
    make test
    make install

安装完成以后，会在home目录下生成一个sandboxes目录，不要删除，之后通过mysql_sandbox安装的MySQL就存放在该目录下。

mysql_sandbox安装完成以后，会生成下列工具：

    make_multiple_custom_sandbox  make_replication_sandbox make_sandbox_from_installed
    make_multiple_sandbox  make_sandbox make_sandbox_from_source  

通过名字已经能够大概猜到各个工具的作用，具体的使用方法，通过`--help` 获取帮助信息即可。

#### 从源码部署mysql_sandbox

例如，我在自己的电脑里，获取了MySQL 5.6.24的源码，进入源码所在的目录，执行cmake和make命令，
然后跳出源码目录，执行`make_sandbox_from_source`，如下所示：

    cmake . -DCURSES_LIBRARY=/usr/lib/x86_64-linux-gnu/libncurses.so -DCURSES_INCLUDE_PATH=/usr/include -DCMAKE_BUILD_TYPE=Debug
    make
    make_sandbox_from_source mysql-5.6.24 single

效果如下，可以看到，mysql_sandbox已经初始化了一个用户，并且自动选择了一个端口号，直接使用该账号登陆即可：

![docker-mysql](/cn/image/mysql-sandbox-mysql.png)

进入mysql_sandbox目录，可以看到，mysql_sandbox目录下有很多脚本，这些脚本用以控制mysql_sandbox启动的数据库实例。

    rds-user@rdshzlaimingxing-no-delete:~/sandboxes$ ls
    clear_all  msb_5_6_24  plugin.conf  restart_all  sandbox_action  send_kill_all
    start_all  status_all  stop_all  test_replication  use_all

从上面的操作可以看到，如果mysql_sandbox已经准备就绪，只需要三条语句，就能够完成MySQL的安装。

### 总结

Docker和mysql_sandbox都是学习MySQL的好工具，充分利用能够有效提高大家的学习效率，希望对大家有所帮助。

[1]: http://mingxinglai.com/cn/2013/06/compile-mysql-in-win7/
[2]: http://mingxinglai.com/cn/2013/06/compile-mysql-in-linux/
[3]: http://dockerpool.com/static/books/docker_practice/introduction/what.html
[4]: https://hub.docker.com/_/mysql/
[5]: https://github.com/datacharmer/mysql-sandbox
