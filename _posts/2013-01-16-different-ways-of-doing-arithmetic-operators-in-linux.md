---
layout: post
title: "shell脚本中的算术运算"
description: ""
category: 程序语言
tags: [shell, 脚本, 算术运算]
---

本文将介绍6种shell脚本中算术运算的方法，各有优缺点，当然了，最常用的就那两种，我会特别指出。

####6种算术运算方法是：

1. let operation
2. expr operation
3. $[ operation ]
4. $(( operation ))
5. 用awk 做算术运算
6. echo "operation" | bc


##1. let 

	let no1++
	let no1--
	let no+=6
	let no=no-6

let 使用方法比较简单，尤其是用来做自增自减运算，看起来更直观，引用变量的时候也不用在变量名前加"$"符号。


##2. expr

	result=`expr 3 + 4`
	result=$( expr $no + 5)

我最不喜欢这种方式了，怎么看怎么别扭，每次都要写expr 这
几个字符，而且都要用反引号引起来，怎么会有这种方式呢？这一定是历史遗留问题！


##3. $[ operation ]

这是比较常见的一种方式了，而且用起来也比较方便，在引用变量的时候，可以在变量名前面加"$"符号，也可以不加。

	result=$[ no1 + no2 ]
	result=$[ $no1 + 5 ]

个人觉得这是比较正常也比较方便的方式，不过，它也不是没有缺点，dash
就不支持它，好在现在大部分linux
发行版都有bash,所以，在写脚本的时候，最好在前面加上'#!/bin/bash'
，你也可能加的是'#!/bin/sh' ,这种方式也可能调用的是dash,因为有可能/bin/sh 只是/bin/dash 的一个符号链接(见参考书1)。

##4. $(( operation ))

这也是一种很常见的方式，除了比上一种方式要多敲两次键盘，其他用法都一样，就我个人而言，还是更喜欢上面那种方法，怎么看怎么正常，因为shell 里面if 语句的判断式是 [ command or expression ] ，用上面那种方式进行算术运算，可能看起来更一致。

上面就是4种shell
脚本进行算术运算的方法，**它们都不支持浮点数**。所以，聪明的我们可以用其他方法来进行算数运算，这其中，awk 就是一个很好的方法。

##5. awk 

awk类似于c语言，而且提供了多种函数，需要浮点数运算或更高级数学函数的时候，用awk
才是聪明之举。

	echo -e "$i\t$j" | awk '{ print $1 + $2}'

##6. bc 

awk 已经可以帮助我们完成很多运算了，不过你可能更喜欢用bc
来计算，毕竟，人家bc 才是专业的，而且看起来比awk
更直观，最重要的，有些事情用awk
无法解决，比如，你要计算两个10进制的数，然后以2进制的方式输出结果。

	echo "obase=2;$i+$j" | bc

bc 还有一个非常非常好的好处，如下所示：

	variable=`bc << EOF
	options
	statements
	expressions
	EOF
	`

具体实例：

	#!/bin/bash
	var1=10.46
	var2=43.67
	var3=33.2
	var4=71
	var5=`bc << EOF
	scale=4
	a1=($var1 * $var2)
	b1=($var3 * $var4)
	a1+b1
	EOF
	`
	echo The final answer for this mess is $var5


应该，或许，可能还有其他方法，不过，这些都是linux系统自带的工具和方法，所以，兼容各种平台，而且，这几种运算已经能够满足我的需求了，方法再多，反而会变成累赘。

####参考资料：

* 《[Linux命令行与shell脚本编程大全][1]》
* 《[Linux Shell脚本攻略][2]》

[1]: http://book.douban.com/subject/11589828/
[2]: http://book.douban.com/subject/6889456/
