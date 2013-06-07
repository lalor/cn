---
layout: post
title: "一堆shell脚本技巧"
description: ""
category: 程序语言
tags: [shell, 脚本, 技巧, tee, stat]
---

本文收集了一堆的shell脚本技巧，我说过，我写博客主要是作一些学习笔记，方便自己查阅，所以，我会搞出这么一篇文章，也没有什么不可理解的。关于这些技巧的出处，诶，我也忘了，可能来自[theunixschool][1]、 
[commandlinefu][2]、[酷勤网][3]和[igigo.net][4]，当然了，也有部分是我自己的经验心得，管他呢，进了我的脑子就是我的了。

###0. shell 调试

1. sh -x somefile.sh
2. 在somefile.sh 文件里加上set+x set-x

###1. 用 && || 简化if else

	gzip -t a.tar.gz
	if [[ 0 == $? ]]; then
	    echo "good zip"
	else
	    echo "bad zip"
	fi

可以简化为：

	gzip  -t a.tar.gz && echo "good zip" || echo "bad zip"

###2. 判断文件非空

	if [[ -s $file ]]; then
	    echo "not empty"
	fi


###3. 获取文件大小

	stat -c %s $file
	stat --printf='%s\n' $file
	wc -c $file

###4. 字符串替换

	${string//pattern/replacement}
	a='a,b,c'
	echo ${a//,/ /}

###5. Contains 子字符串？

	string="My string"
	if [[ $string == *My* ]]; then
	    echo "It's there!"
	fi

###6. 备份

	rsync -r -t -v /source_folder /destination_folder
	rsync -r -t -v /source_folder [user@host:/destination_folder

###7. 批量重命名文件

* 为所有txt文件加上.bak 后缀：

		rename '.txt' '.txt.bak' *.txt

* 去掉所有的bak后缀

		rename '*.bak' '' *.bak

* 把所有的空格改成下划线

		find path -type f -exec rename 's/ /_/g' {} \;

* 把文件名都改成大写

		find path -type f -exec rename 'y/a-z/A-Z/' {} \;


###8. for/while 循环

	for ((i=0; i < 10; i++)); do echo $i; done 
	for line in $(cat a.txt); do echo $line; done 
	for f in *.txt; do echo $f; done 
	while read line ; do echo $line; done < a.txt 
	cat a.txt | while read line; do echo $line; done

###9. 删除空行

	cat a.txt | sed -e '/^$/d'
	(echo "abc"; echo ""; echo "ddd";) | awk '{if (0 != NF) print $0;}'

###10. 比较文件的修改时间

	[[ file1.txt -nt file2.txt ]] && echo true || echo false
	[[ file1.txt -ot file2.txt ]] && echo true || echo false

###11. 实现Dictionary结构

    hput() { 
            eval "hkey_$1"="$2" 
    }
    hget() { 
            eval echo '${'"hkey_$1"'}' 
            }
    $ hput k1 aaa
    $ hget k1
    aaa

###12. 去掉第二列

	$echo 'a b c d e f' | cut -d ' ' -f1,3-
	$a c d e f

###13. 把stderr输出保存到变量

	$ a=$( (echo 'out'; echo 'error' 1>&2) 2>&1 1>/dev/null) 
	$ echo $a
	error

###14. 删除前3行

	$cat a.txt | sed 1,3d


###15. 读取多个域到变量

	read a b c <<< "xxx yyy zzz"


###16. 遍历数组

	array=( one two three ) 
	for i in ${array[@]} 
	        do 
	echo $i 
	done

###17. 查看目录大小

	$ du –sh ~/apps

###18. 查看CPU信息

	$ cat /proc/cpuinfo

###19. date

	$ date +%Y-%m-%d
	2012-12-24
	$ date +%Y-%m-%d –date ‘-1 day’
	2012-12-23
	$ date +%Y-m-%d –date ‘Dec 25’
	2011-12-25
	$ date +%Y-m-%d –date ‘Dec 25 – 10 days’
	2011-12-15

###20. 获取路径名和文件名

	$ dirname ‘/home/lalor/a.txt’
	/home/lalor
	$ basename ‘/home/lalor/a.txt’
	a.txt

###21. 并集和交集

comm 可以用来求并集，交集，差集,假设现在有两个文件a和b,它们的内容如下：

	$cat a
	1
	3
	5
	
	$cat b
	3
	4
	5
	6
	7
	
	$comm a b 
	1
			3
		4
			5
		6
		7
	
	$comm -1 -2 a b #交集
	3
	5
	
	$comm a b | sed 's/\t//g' #并集
	1
	2
	3
	4
	5
	6
	7
	
	$comm -1 -3 a b | sed 's/\t//g' #b-a
	4
	6
	7
	
###22. awk复杂分隔符

* 多字符作分隔符

		$ echo "a||b||c||d" | awk -F '[|][|]' '{print $3}'
		c

* 多种分隔符1

		$echo "a||b,#c d" | awk -F '[| ,#]+' '{print $4}'
		d
* 多种分隔符2

		$echo "a||b##c|#d" | awk -F '([|][|])|([#][#])' '{print $NF}'
		c|#d

###23. 产生一个随机数

	echo $RANDOM

###24. 按照模式split 文件

	csplit server.log /PATTERN/ -n 2 -s {*} -f server_result -b "%02d.log" -z
	/PATTERN/ 用来匹配某一行，分割过程由此开始
	{*} 根据匹配，重复执行分割
	-s 静默模式
	-n 分割后文件名后缀中，数字的个数
	-f 分割后的文件名前缀
	-b 指定后缀格式

###25. 获取文件名或者扩展名

	var=hack.fun.book.txt
	echo ${var%.*}
	hack.fun.book
	echo ${var%%.*}
	hack
	echo ${var#.*}
	fun.book.txt
	echo ${var##.*}
	txt


###26. 以 root 帐户执行上一条命令。

	$sudo !!

其中：
* !! 是指上一条命令
* !$ 上一条命令的最后一个参数
* !\* 上一条命令的所有参数
* !:3 上一条命令的第3个参数

例如：

	$ls /tmp/somedir
	 ls: cannot access /tmp/somedir: No such file or directory
	$mkdir !$
	 madir /tmp/somedir


###27. 利用 Python 搭建一个简单的 Web 服务器，可通过 http://$HOSTNAME:8000 访问。

	python -m SimpleHTTPServer


###28. 在 Vim 中无需权限保存编辑的文件。

	:w !sudo tee %


###29. 将上一条命令中的 foo 替换为 bar，并执行。

	^foo^bar

###30. 快速备份或复制文件。

	cp filename{,.bak}


###31. 将 ssh keys 复制到 user@host 以启用无密码 SSH 登录。

	$ssh-copy-id user@host


###32. 把 Linux 桌面录制为视频。

	ffmpeg -f x11grab -s wxga -r 25 -i :0.0 -sameq /tmp/out.mpg


###33. man 妙用

	man ascii
	man test


###34. 在 vim 中编辑上一条命令

	fc 


###35. 删除0 字节文件或垃圾文件

	find . -type f -size 0 -delete
	find . -type f -exec rm -rf {} \;
	find . -type f -name "a.out" -exec rm -rf {} \;
	find . type f -name "a.out" -delete
	find . type f -name "*.txt" -print0 | xargs -0 rm -f


###36. 在编写SHELL 时显示多行信息

	cat << EOF
	+--------------------------------------------------------------+
	| === Welcome to Tunoff services === |
	+--------------------------------------------------------------+
	EOF

注意，在指定结束符时，它必须是该行的唯一内容，并且该行必须以这个字符开头。


###37. 如何给mysql建软链接

	cd /usr/local/mysql/bin
	for i in *
	do ln /usr/local/mysql/bin/$i /usr/bin/$i
	done


###38. 获取IP地址：

	ifconfig eth0 |grep "inet addr:" |awk '{print $2}'|cut -c 6-


###39. 打开文件数目

	lsof


###40. 清除僵尸进程

	ps -eal | awk '{ if ($2 == "Z"){ print $4}}' | kill -9


###41. 打印唯一行

	awk '!a[$0]++' file


###42. 打印奇数行

	awk 'i=!i' file
	awk 'NR%2' file


###43. 打印匹配行后的某一行

	seq 10 | awk '/4/{f=4};--f==0{print;exit}'


###44. 打印某行后后面的10行

	cat file | grep -A100 string
	cat file | grep -B100 string #前面
	cat file | grep -C100 string #前后

	sed -n '/string/,+100p'

	awk '/string/{f=100}--f>=0'

###45. 获取命令行最后一个参数

	echo ${!#}
	echo ${$#} #错误的尝试

###46. 输出重定向

如果你愿你，可以将STDERR 和 STDOUT 的输出重定向到一个输出文件，为此，bash 提供了特殊的重定向符号 &>

	ls file nofile &> /dev/null

我们如何在脚本里面重定向呢？没有什么特别之处，和普通重定向一样。

	#!/bin/bash
	#redirecting output to different locations
	echo "now redirecting all output to another location" &>/dev/null

问题就来了，如果我们要将所有的输出都重定向到某个文件呢？我们都不希望每次输出的时候都重定向一下吧，正所谓，山人自有妙计。我们可以用exec 来永久重定向，如下所示：

	#!/bin/bash
	#redirecting output to different locations
	exec 2>testerror
	echo "This is the start of the script"
	echo "now redirecting all output to another location"
	
	exec 1>testout
	echo "This output should go to testout file"
	echo "but this should go the the testerror file" >& 2

输出结果如下所示：

	This is the start of the script
	now redirecting all output to another location
	lalor@lalor:~/temp$ cat testout 
	This output should go to testout file
	lalor@lalor:~/temp$ cat testerror 
	but this should go the the testerror file
	lalor@lalor:~/temp$ 

以追加的方式重定向：

	exec 3 >> testout

取消重定向：

	exec 3> -


###47. 函数

* 任何地方定义的变量都是全局变量，如果要定义局部变量，需加local 关键字

* shell中的函数也可以用递归

		#!/bin/bash
		
		function factorial {
			if [[ $1 -eq 1 ]]; then
				echo 1
			else
				local temp=$[ $1 - 1 ]
				local result=`factorial $temp`
				echo $[ $result * $1 ]
			fi
		}
		
		result=`factorial 5`
		echo $result

* 创建函数库

	将函数定一个在另一个文件，然后通过source 命令加载到当前文件

* 在命令行使用函数

	将函数定义在~/.bashrc 中即可

* 向函数传递数组

		#!/bin/bash
		#adding values in an array
		
		function addarray {
			local sum=0
			local newarray
			newarray=(`echo "$@"`)
			for value in ${newarray[*]}
			do
				sum=$[ $sum+$value ]
			done
			echo $sum
		}
		
		myarray=(1 2 3 4 5)
		echo "The original array is: ${myarray[*]}"
		arg1=`echo ${myarray[*]}`
		result=`addarray $arg1`
		echo "The result is $result"


##正则表达式

匹配中文字符的正则表达式：`[u4e00-u9fa5]`      
评注：匹配中文还真是个头疼的事，有了这个表达式就好办了    
    
匹配双字节字符(包括汉字在内)：`[^x00-xff]`      
评注：可以用来计算字符串的长度（一个双字节字符长度计2，ASCII字符计1）    
    
匹配空白行的正则表达式：`^ *$`      
评注：可以用来删除空白行    
    
匹配HTML标记的正则表达式：`<(S*?)[^>]*>.*?</1>|<.*? />`      
评注：网上流传的版本太糟糕，上面这个也仅仅能匹配部分，对于复杂的嵌套标记依旧无能为力    
    
匹配首尾空白字符的正则表达式：`^s*|s*$`    
评注：可以用来删除行首行尾的空白字符(包括空格、制表符、换页符等等)，非常有用的表达式    
    
匹配Email地址的正则表达式：`w+([-+.]w+)*@w+([-.]w+)*.w+([-.]w+)*`    
评注：表单验证时很实用    
    
匹配网址URL的正则表达式：`[a-zA-z]+://[^s]*`    
评注：网上流传的版本功能很有限，上面这个基本可以满足需求    
    
匹配帐号是否合法(字母开头，允许5-16字节，允许字母数字下划线)：`^[a-zA-Z][a-zA-Z0-9_]{4,15}$`    
评注：表单验证时很实用    
    
匹配国内电话号码：`d{3}-d{8}|d{4}-d{7}`    
评注：匹配形式如0511-4405222或021-87888822    
    
匹配腾讯QQ号：`[1-9][0-9]{4,}`    
评注：腾讯QQ号从10000开始    
    
匹配中国邮政编码：`[1-9]d{5}(?!d)`    
评注：中国邮政编码为6位数字    
    
匹配身份证：`d{15}|d{18}`    
评注：中国的身份证为15位或18位    
    
匹配ip地址：`d+.d+.d+.d+`   
评注：提取ip地址时有用    
    
匹配特定数字：    
`^[1-9]d*$`　　//匹配正整数    
`^-[1-9]d*$`　//匹配负整数    
`^-?[1-9]d*$`　　//匹配整数    
`^[1-9]d*|0$`　//匹配非负整数（正整数+ 0）    
`^-[1-9]d*|0$`　　//匹配非正整数（负整数+ 0）    
`^[1-9]d*.d*|0.d*[1-9]d*$`　　//匹配正浮点数    
`^-([1-9]d*.d*|0.d*[1-9]d*)$`　//匹配负浮点数    
`^-?([1-9]d*.d*|0.d*[1-9]d*|0?.0+|0)$`　//匹配浮点数    
`^[1-9]d*.d*|0.d*[1-9]d*|0?.0+|0$`　　//匹配非负浮点数（正浮点数+ 0）    
`^(-([1-9]d*.d*|0.d*[1-9]d*))|0?.0+|0$`　　//匹配非正浮点数（负浮点数+ 0）    
评注：处理大量数据时有用，具体应用时注意修正    
    
匹配特定字符串：    
`^[A-Za-z]+$`　　//匹配由26个英文字母组成的字符串    
`^[A-Z]+$`　　//匹配由26个英文字母的大写组成的字符串    
`^[a-z]+$`　　//匹配由26个英文字母的小写组成的字符串    
`^[A-Za-z0-9]+$`　　//匹配由数字和26个英文字母组成的字符串    
`^w+$`　　//匹配由数字、26个英文字母或者下划线组成的字符串    

[1]: http://theunixschool.com
[2]: http://www.commandlinefu.com/commands/browse
[3]: http://www.kuqin.com/linux/20121018/332591.html
[4]: http://www.igigo.net/cmd
