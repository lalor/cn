---
layout: post
title: "为什么shell脚本中的判断式这么变态"
description: ""
category: 程序语言
tags: [shell, 脚本, test]
---

我们首先来看一下shell中的if语句是怎么写的，下面这个if
语句出现频率还是很高的，就用它来说事吧。

	if grep -q "root" /etc/passwd 2>/dev/null
	then
		echo "there is a user, root"
	else
		echo "there is not a user, root"
	fi

这就是shell中的if语句，非常常见的用法，当然，你也可以将then和if
放在同一行，不过需要在grep命令最后加上分号，这很好理解，grep
是一个单独的命令，加上分号以表示该命令结束了。

	if grep -q "root" /etc/passwd 2>/dev/null; then
		echo "there is a user, root"
	else
		echo "there is not a user, root"
	fi

我们再来看一下在if语句中使用关系表达式时的写法：

	a=5
	b=6
	if [ $a -ne $b ]; then
		echo "$a -eq $b"
	else
		echo "$a -ne $b"
	fi

它也可以写成下面这样：

	a=5
	b=6
	if [ $a -ne $b ]
	then
		echo "$a -eq $b"
	else
		echo "$a -ne $b"
	fi


为什么使用关系表达式时的写法和在if语句中使用一条命令的写法一样呢？为什么写关系表达式的时候后面也要加分号呢？这不科学啊。

这其中的奥秘就是shell中的if只有一种形式，那就是if
后面跟一条命令，根据命令的返回值(echo $?)来判断是真还是假，如下所示：

	if command
	then
		commands
	else
		commands
	fi

可是，如果这样的话，[ $a -ne $b ]怎么解释？你现在想象一下泰囧里王宝强那非常非常贱的表情，然后，真相是，[ ] 是一条命令。

我们知道，linux下有一个名为test的命令，用来完成数字，字符串和文件的比较与判断，如果你还不知道test命令，请`man test` ，真相中的真相就是
[ ]是test的一种形式，一个别名，你用[ ]的时候，就相当于用test命令。所以，上面使用的脚本也可以这样写：

	a=5
	b=6
	if test $a -eq $b
	then
		echo "$a -eq $b"
	else
		echo "$a -ne $b"
	fi

不要怀疑我， [ ]真是一条命令，不信你在终端执行`[ 5 -eq 6 ]` 然后 `echo
$?`试试，你还可以 `man [ ]`呢。

理解了[ ]是一条命令以后，其他都可以解释了，比如，为什么`if [ $a -ne $b ];
then` 这里要分号。为什么 if 中间用&&和||的时候，&&和||不是在[ ] 内，而是在 [ ]外。 

	if [[ -e file ]] && [[ -r file ]]; then

个人觉得shell中的if还是挺烦人的，起码在我刚开始接触shell
的时候，我总不能一次写正确，不过，当我知道[ ]是一条命令以后，我就很少写错了:-)

最后，请记住，shell中的if只有一种形式：

	if command
	then
		commands
	else
		commands
	fi

如果要将then和if放在同一行，就需要在命令后面加上分号。

	if command; then
		commands
	else
		commands
	fi

完。
