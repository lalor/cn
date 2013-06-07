---
layout: post
title: "傻瓜式配置mutt"
description: ""
category: 工具
tags: [mutt, msmtp, fedora, gmail]
---

#介绍


<!--用了一年的Ubuntu，终于换成fedora了，换掉Ubuntu的理由居然是......前几天写了个脚本帮我在淘宝顶红包，我发现Ubuntu没有usleep这个系统调用，我终于相信Ubuntu是linux的阉割版了，果断弃之，你们尽管唾弃我吧。 -->

mutt是什么？用一句话介绍就是：它是linux终端有名的邮件客户端，它运行在终端，受到很多linux狂热分子和无限最求效率的人的热捧，我当然没有那么geek，也没有追求效率到什么事都要在命令行搞定，但是mutt有时候真的很方便。

比如我经常要向另一台电脑发送数据，或者只是通过邮件发送几句话，几个小文件，这时就是发挥mutt优势的时候了。例如，我可以像下面这样给自己发送文件：

    echo -e "Hi\n\tthis email come from terminal" |
    mutt -s "just for fun" -a *.txt -- someone@gmail.com

在命令行使用邮件还有一个比较方便的地方就是便于写脚本，比如你要让电脑帮你做事情，末了自动把结果发到指定邮箱，这时你就需要mutt了。再比如，你写了一封情书，一时激动，发到你前女友那去了，这时候该怎么办呢？如果，万一，要是，这种事情真的发生了，你可以向她邮箱发送一百封标题跟刚才一样的邮件，她打开前几封邮件发现都是一样的，就不会往下看了，全选，删除之，你的目的就达到了。如果想指定邮箱发送一百封内容一样的邮件，我想，没有比shell脚本更方便的东西了。


#mutt配置

## 一、安装mutt和msmtp

    sudo yum install mutt msmtp

## 二、配置mutt

###### 创建~/.muttrc

    vim ~/.muttrc

###### 设置你自己邮件头的姓名和邮件地址

    set from = "username@gmail.com"
    set realname = "username"

###### 设置mutt登陆gmail邮箱和密码

    set imap_user = "username@gmail.com"
    set imap_pass = "yourpassword"

###### 设置邮件服务器上的文件夹

    set folder = "imaps://imap.gmail.com:993"
    set spoolfile = "+INBOX"
    set postponed = "+[Gmail]/Drafts"

### 设置本地计算机上的缓存

###### 创建本地文件夹

    mkdir -p ~/.mutt/cache

###### 在.muttrc中加入一下信息设置本地缓存

    set header_cache=~/.mutt/cache/headers
    set message_cachedir=~/.mutt/cache/bodies

###### 设置TLS证书

gmail登陆需要TLS证书验证，不同发行版路径好像不太一样，下面路径是fedora的，ubuntu的类似。

    set certificate_file=/etc/ssl/certs/ca-certificates.crt

###### 设置smtp服务来发送邮件

    set smtp_url="smtp://username@smtp.gmail.com:587/"
    set smtp_pass="yourpassword"

###### 最后保存.muttrc，并设置好权限

    chmod 700 .muttrc

## 三、配置msmtp

###### 创建或修改~/.msmtprc

    vim ~/.msmtprc

###### 写入以下配置信息

    default
    tls on
    tls_trust_file /etc/ssl/certs/ca-certificates.crt
    logfile ~/.mutt/msmtp.log
    
    #gmail
    account gmail
    host smtp.gmail.com
    port 587
    from username@gmail.com
    auth on
    user username@gmail.com
    password yourpassword

###### 保存.msmtprc，并设置好权限

    chmod 600 .msmtprc

##其他注意事项##

我在公司的服务器上配置mutt ，遇到的错误是：

	No authenticators available
	Could not send the message.

解决方法是安装一个叫libsas12-modules 的库。

	sudo aptitude install libsas12-modules



## 参考

- [https://blog.lowstz.org](https://blog.lowstz.org/posts/2011/08/08/mutt-msmtp-gmail-imap/)
- [http://stevelosh.com](http://stevelosh.com/blog/2012/10/the-homely-mutt/?utm_source=feedburner&utm_medium=feed&utm_campaign=Feed%3A+stevelosh+%28Steve+Losh%29)
- [http://blog.youxu.info](http://blog.youxu.info/2008/03/20/some-handy-scripts/)
- [http://velt.de](http://velt.de/blog/sven/mutt-quot-no-authenticators-available-quot)


