---
layout: post
title: "那些你希望N年前就掌握的命令"
description: ""
category: 工具
tags: [tool, linux]
---

*这篇文章转载自[黑客志][0],短短的一篇文章我找到了3个对我非常有用的技巧,在信息爆炸的今天,简直就跟捡宝似的,希望这些命令对你也有帮助。*

有人在Reddit上发帖询问：[有没有哪条命令是你希望自己在几年前就掌握的][1],下面的回复中提到的一些命令,看看你知道几个,当然,也欢迎补充你的。

#1.units

单位转换程序：

	You have: cm^3
	You want: gallons
      * 0.00026417205
       / 3785.4118

	You have: 1|2 inch
	You want: cm
	      * 1.27
        / 0.78740157


#2.后台运行命令

如果你希望让某条命令在退出SSH后依然保持运行状态,ZSH用户可以在命令后加上&|或是&!,也可以设置Shell的nohup属性,这样所有后台任务都将忽略hangup信号。


#3.取得运行中进程的输出

	~$ touch log
	~$ gdb -p 22499
	(gdb) p close(1) # This closes the stdout of the
	running process
	(gdb) p open("/home/.../log", 1) # This will open the
	log-file with O_WRITE (gdb) c

#4.fc 

fc命令会根据$EDITOR变量打开一个编辑器,让你编辑上一条命令,你还可以使用`Ctrl-x,Ctrl-e`来编辑当前行的命令。


#5.bindkey

绑定键盘快捷键,执行下面这条命令后,你就可以使用Ctrl-j回到上一条命令：

	bindkey "^j" history-search-backward


#6.lsof

关于lsof,可以查看我们之前的一篇介绍文章"[Unix调试的瑞士军刀：lsof][2]"。


#7.man ascii

以8进制,10进制以及16进制显示ASCII码表


#8.^find^replace

	$ /etc/init.d/foo stop
	$ ^stop^start


下面是一条等价命令：

	!:s/foo/bar  # 替换第一个
	!:gs/foo/bar # 替换全部


#9.cd -

看个例子：

	cd /some/complicated/directory
	cd ~/foo cp some_file `cd -`

更简单的：

	cp some_file ~-

#10.:w ! sudo tee %

当你在vi中修改了半天配置文件,然后发现没有写权限,没有比这更令人感到挫败了,此时你需要这条命令。

[0]: http://heikezhi.com/2011/11/28/one-command-you-wish-you-knew-years/
[1]: http://www.reddit.com/r/linux/comments/mi80x/give_me_that_one_command_you_wish_you_knew_years/ 
[2]: http://heikezhi.com/2011/06/19/swiss-army-knife-of-unix-debugging-lsof/

