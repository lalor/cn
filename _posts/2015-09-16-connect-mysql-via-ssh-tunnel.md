---
layout: post
title: "使用SSH隧道连接MYSQL"
description: ""
category: 工具
tags: [ssh]
---

# 1. 概述

本文介绍了如何使用SSH隧道连接MySQL,网络隔离导致无法直接访问数据库的问题. 第2节对需求进行了简单的描述；第3节介绍了linux环境下,如何通过SSH隧道连接MySQL；第4节介绍了windows环境下,如何通过SSH隧道连接MySQL.

# 2. 需求描述

![ssh_mysql](/cn/image/ssh_mysql_01.png)

图1 需求示意图

图1描述了用户的具体需求,即用户在localhost这台机器上,数据库实例在B这台机器上,由于网络隔离的原因,用户无法直接访问B这台机器.但是,用户可用SSH到A这台机器,而A这台机器是可以访问B这台机器上的数据库实例的.因此,可以考虑将A作为跳板机,使用SSH隧道技术,以便用户在localhost上可以直接访问B这台机器上的数据库.

**额外说明：**接下来将以图1中的IP和端口为例,演示linux环境下和windows环境下用户通过SSH隧道访问MySQL的具体方法,在图1中,用户可以通过SSH登录到A这台机器,A的IP为106.2.32.177,A的SSH端口是1046,数据库实例所在的机器IP为10.164.172.204,数据库的端口为3306,假设现在存在一个数据库用户为test,密码为123456.

# 3. linux环境

假设localhost是一台linux,则可以在该机器上执行如下命令,建立一个SSH隧道：

    ssh -fCPN –L 3307:10.164.172.204:3306 -p1046 rds-user@106.2.32.177

说明如下：执行上面的shell语句,会在本地打开一个ssh的守护进程,该进程会监听本地的3307端口,这个端口为隧道的入口,当访问本地的3307端口时,数据包会通过ssh进程,经由106.2.32.177（A）发送到最终的10.164.172.204（B）的3306端口,以此达到localhost机器直接访问B上的数据库实例的目的.

    # 参数解释
    # -C    使用压缩功能,是可选的,加快速度.
    # -P    用一个非特权端口进行出去的连接.
    # -f    一旦SSH完成认证并建立port forwarding,则转入后台运行.
    # -N    不执行远程命令.该参数在只打开转发端口时很有用（V2版本SSH支持）

执行成功以后,可以通过如下命令查看SSH进程：

![ssh_mysql](/cn/image/ssh_mysql_02.png)

也可以通过如下命令查看打开的端口号：

![ssh_mysql](/cn/image/ssh_mysql_03.png)

设置完成以后,直接访问本地的3307端口,就可以登录远程的MySQL了,如下所示：

![ssh_mysql](/cn/image/ssh_mysql_04.png)

可以看到,只需要一条命令,就能够实现通过SSH隧道连接MySQL.只要保证localhost能够通过SSH登录A机器,A机器能够访问B机器上的MySQL实例,设置几乎没有任何困难.

# 4. windows环境

windows下通过SSH隧道连接MySQL与linux几乎一样,首先设置SSH隧道,然后连接本地的端口,下面以XShell与Navicat为例,说明如何在windows下实现通过SSH隧道的方式访问MySQL.

第一步,设置ssh连接,这与普通的SSH连接一模一样.

![ssh_mysql](/cn/image/ssh_mysql_06.png)

第二步,设置SSH隧道,如下所示：

![ssh_mysql](/cn/image/ssh_mysql_07.png)

第三步,设置完成以后,点击确定,并且使用SSH登录到A机器,以便打开SSH隧道.
第四步,打开Navicat,建立一个新的连接,连接的地址为localhost或127.0.0.1,连接的端口为3307（在本例中）,如下所示：

![ssh_mysql](/cn/image/ssh_mysql_08.png)

点击Test Connection,将看到连接成功的提示,经过以上简单的几步,我们就实现了,访问localhost的本地端口,但是实际却连接的是我们原本无法访问的机器B上的数据库实例.
