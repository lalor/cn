---
layout: post
title: "使用Polysh交互式的批量修改用户密码"
description: ""
category: 工具
tags: [polysh]
---

由于公司的一个测试环境被外部人员攻击了，所以公司要求所有的机器，都不能以密码登陆。
为此，我写了一个脚本来禁止root登陆、禁止密码登陆，即修改/etc/ssh/sshd_config
文件中的PermitRootLogin、PasswordAuthentication、RSAAuthentication和
PubkeyAuthentication。

虽然这么做了，但是还是可以通过私钥登陆以后`su root`到root用户，因此，
即使不能通过密码登陆了，也还是不允许使用简单密码，所以，需要一个批量修改密码的工具。


由于修改密码是交互式的，脚本操作起来非常麻烦。当然也不是不行，如果搜一下
"批量修改用户密码"，一般都会搜到expect相关的文章，个人感觉expect还是一个比较麻烦，
需要处理起来很小心，然后我脑洞大开，想起来一个叫[Polysh][1]的东西，
这个工具可以一次登录多台机器，并在各台机器上同时执行相同的操作。用来修改所有虚拟机都
拥有相同用户和密码的机器实在是太合适了。

安装方法如下：

    wget wget http://guichaz.free.fr/polysh/files/polysh-0.4.tar.gz
    tar -zxvf polysh-0.4.tar.gz
    cd polysh-0.4
    sudo python setup.py install

使用方法如下：

    polysh --ssh='exec ssh -p 1047 -i ~/.ssh/rds.private' --user=rds-user --hosts-file=ip.list

其中`ip.list`中是各个服务器的IP，每行一个IP地址，不需要任何分隔符，如下所示：

    10.180.49.77
    10.180.49.21
    10.180.49.40
    10.180.49.32
    10.180.49.44
    10.180.49.49
    10.180.49.37
    10.180.49.38

效果见本文最后的截图。

此外，还有一个类似的叫做[csshx][2]的工具，这个工具可以开很多窗口，然后把输入广播到所有
窗口。

总结：批量修改用户密码应该还是比较少见的需求，不过需要同时登陆多台服务器，然后执行相同
的操作，这种需求还是比较常见，这个时候借助Polysh和shell脚本，就能够大大提高工作效率。
其实这些并没有什么难的地方，关键就在于你是否知道这个工具的存在，然后充分利用这些工具
来提高自己的工作效率，从琐事的工作当中解放出来。

![polysh](/cn/image/polysh.png)

[1]: http://guichaz.free.fr/polysh/
[2]: http://askdba.org/weblog/2012/01/cluster-ssh-tool-utility/
