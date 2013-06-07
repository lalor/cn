---
layout: post
title: find 学习笔记
category: 工具
tags: [find, shell]
---

#文件查找与文件列表

`find`是Unix/Linux命令行工具箱最棒的工具之一，这个命令对编写shell脚本很有帮助，但是多数人由于对它缺乏认识，并不能有效的使用它。:-),包括我自己，刚开始极少使用`find`,在知道`find`的很多功能以后时常需要用到，但是由于find的参数实在太多，每次都需要翻书查阅，特地在此记录，方便下次查阅，这部分例子主要来自《[Linux Shell脚本攻略](http://book.douban.com/subject/6889456/)》，略有补充,我在此向看到这篇博客的童鞋强烈推荐此书。

要列出当前目录及子目录下的文件和文件夹，可以采用下面的写法：

    find base_path
    find . -print
    find .. -print0


bash_path可以是任意位置，如(/home/slynx)，`find`会从该位置开始向下查找。.指定当前目录，..指定父目录，这是Unix文件系统的约定用法。`-print`指明打印出匹配文件的文件名，当使用`-print`时，'\n'作为用于分隔文件的定界符，`-print0`指明使用作为'\0'用于分隔文件的定界符。

作为一个强大的命令行工具，`find`命令包含了诸多值得留意的选项，接下来让我们来看一下`find`命令的一些其他选项。

###(1). 根据文件名或者正则表达式匹配搜索

选项`-name`的参数指定了文件名所必须匹配的字符串。我们可以将同配符作为参数使用。如下所示：

    find . -name "*.txt"
    find . -iname "example*"


`find`命令还有一个选项`-iname`(忽略大小写），该选项的作用和`-name`类似，只不过在匹配名字的时候会忽略大小写。

我们还可以使用OR条件来一次匹配多个：

    find .  -name "*.txt" -o -name "*.pdf"  -print


上面的代码会打印出所有的txt和pdf文件。选项-path可以使用通配符来匹配文件路径或者文件。`-name`总是用给定的文件名进行匹配。`-path`则将文件路径作为一个整体进行匹配。如下所示：

    find ~ -path "*git*" -print


选项`-regex`和`-path`相似，不过`-regex`是基于正则表达式来匹配文件路径的。下面的命令匹配.py或者.sh文件。

    find -regex ".*\(\.py\|\.sh\)$"

类似于`-name`,`-iregex`用于忽略正则表达式的大小写。


在当前目录及子目录中查找不是The开头的pdf文件：

	find . -name "The*" -prune -o -name "*.pdf" -print  

-prune 选项是否定的意思，与接下来要介绍的"!"的区别么，我也还不知道。

###(2). 否定参数

`find`可以用"!"否定参数的含义，例如：

    find . ! -name "*.txt" -print
	find . ! \( -name "*.txt" -o  -name "*.pdf" \) -print


###(3). 基于深度的搜索

`find`命令在使用时会遍历所有的子目录。我们可以采用一些深度参数来限制`find`命令遍历的深度。`-maxdepth`和`-mindepth`就是这类参数。如：

    find . -maxdepth 1 -type f 


该命令只列出当前目录下的所有普通文件，即使有子目录，也不会被打印或遍历，与此类似，`-maxdepth`
2 最多向下遍历两级子目录。

`-maxdepth`和`-mindepth`应该作为`find`的第3个参数出现，如果作为第4个参数或者之后的参数，就可能会影响到`find`的效率，因为它不得不进行一些不必要的检查。

###(4). 根据文件类型搜索

`-type`可以对文件搜索进行过滤。借助这个选项，我们可以为`find`命令指明特定的文件匹配类型。如下所示：

只列出链接：

    find . -type l


只列出目录：

    find . -type d


<table cellspacing="0">
<thead>
<tr align="first">
<td align="left">文件类型</td>
<td align="left">类型参数</td>
</tr>
</thead>
<tbody>
<tr align="center">
<td align="left">普通文件</td>
<td align="left">f</td>
</tr>
<tr align="center">
<td align="left">符号链接</td>
<td align="left">l</td>
</tr>
<tr align="center">
<td align="left">目录</td>
<td align="left">d</td>
</tr>
<tr align="center">
<td align="left">字符设备</td>
<td align="left">c</td>
</tr>
<tr align="center">
<td align="left">块设备</td>
<td align="left">b</td>
</tr>
<tr align="center">
<td align="left">套接字</td>
<td align="left">s</td>
</tr>
<tr align="center">
<td align="left">Fifo</td>
<td align="left">p</td>
</tr>

</tbody>
</table>

###(5). 根据文件时间进行搜索

`-atime`(访问时间)，`-mtime`(修改时间)`-ctime`(变化时间)可以作为`find`的时间参数。它们用整数值给出，单位是天数，这些整数还可以带有正负号，如下所示：

打印出最近7天内被访问过的所有文件：

    find . -type f -atime -7 -print


打印恰好7天前被访问的所有文件：

    find . -type f -atime 7 -print


打印出访问时间超过7天的所有文件

    find . -type f -atime +7 -print


类似的，我们可以根据修改时间，用`-mtime`进行搜索，也可以根据变化时间`-ctime`进行搜索。与此同时，我们还可以使用`-amin,-mmin,-cmin`做为参数，即以分钟作为计量的单位。举例如下：

    find . -type f -amin +7 -print


上面这条语句会打印出所有访问时间超过7分钟的文件。

`find` 另一个漂亮的参数是`-newer` ,使用`-newer`
，我们可以指定一个用于比较时间戳(修改时间)的参考文件，然后找出比该文件更新的所有文件，例如，找出比file.txt 修改时间更长的所有文件：

    find . -type f -newer file.txt -print


###(6). 基于文件大小进行搜索

根据文件的大小，可以这样搜索：

大于2KB的文件

    find . -type f -size +2k


小于2KB的文件

    find . -type f -size -2k


等于2KB的文件

    find . -type f -size 2k

这个参数非常强大，例如，大于一个G的日志文件：

	find . -type f -name "*.Log" -size 1G -delete

除了k之外，我们还可以使用其他文件大小单元

<table>
<tr align="center">
<td align="left">b</td>
<td align="left">块</td>
</tr>

<tr align="center">
<td align="left">c</td>
<td align="left">字节</td>
</tr>

<tr align="center">
<td align="left">w</td>
<td align="left">字</td>
</tr>

<tr align="center">
<td align="left">k</td>
<td align="left">千字节</td>
</tr>

<tr align="center">
<td align="left">M</td>
<td align="left">兆字节</td>
</tr>

<tr align="center">
<td align="left">G</td>
<td align="left">吉</td>
</tr>

</table>

###(7). 删除匹配的选项

假入你的home 目录下有一个code子目录,用于存放你平时做的一些算法练习,而且你也熟悉git,那么你很可能会会写出下面的脚本

    #!/bin/bash
    cd ~/code
    git add .
    git commit -am "Commit - @ $(date)"
    git push
    

然后使用cron 让计算机每晚9点帮你备份数据,但是很可能你的~/code 下还有很多a.out
文件,很明显这是无用的文件,我们要珍惜 github 上宝贵的300M 空间,所以要把所有的a.out 文件删除,该怎么做呢？

一种方法是我们可以通过find 提供的-exec 参数来对找到的文件执行操作,格式如下：

    find . -name a.out -exec rm -rf {} \;


还有更简单的办法就是使用find 提供的-delete 参数,

    find . -type f -name "a.out" -delete


当然了,方法总是多多的,我们还可以将找到的文件名通过xargs
传给下一个命令,不过需要注意的是,正确的用法应该是

    find . -type f -name "filename" -print0 | xargs -0 rm -f


上面的-print0 就是明确说明用\0 作为输入的定界,而不是默认的空格或回车,按照下面的方法也是正确的,但是当遇到文件名包含空格的时候就可能会出错。如你当前文件夹下面有三个文件,分别是empty、file 和  empty file. 你现在要删除empty file .

    find . -name "empty file" | xargs rm -rf 


结果不是我们期望的那样的,这条命令会删除empty 和 file 文件,保留empty file 文件,所以为了以防万一,只要我们把find 的输出作为xargs 的输入,就必须将 -print0 与find 结合使用,以字符null 来分割输出。

更常用的，我们可以方便的删除0字节的文件

	find . -type f -size 0 -delete


###(8). 基于文件权限和所有权的匹配

文件匹配可以根据文件权限进行。列出具有特定权限的所有文件：

    find . -type f -perm 644 -print


以Appache Web服务器为例。Web服务器上的PHP文件需要具有合适的执行权限。我们可以用下面的方法找出那些没有设置好权限的PHP文件：

    find . -type f -name "*.php" ! -perm 644 -print


用选项`-user USER`就能够找出某个特定用户所拥有的文件，参数USER可以是用户名也可以是UID

###(9). 结合find执行命令或动作

`find`命令可以借助选项`-exec`与其他命令进行结合。`-exec`算得上是`find`最强大的特性之一。下面看看应该如何使用`-exec`选项。

下面，我们将某位用户(比如说是root)的全部文件的所有权更改成另一位用户，那么我们就可以用`-user`找出root拥有的所有文件，然后用`-exec`更改所有权。

    find . -type f -user root -exec chown YourName {} \;


在这个命令中，{}是一个特殊的字符串，与`-exec`选项结合使用。对于每一个匹配的文件，{}，会被替换成相应的文件名。例如：`find`命令找到两个文件test1.txt和test2.txt,其所有者均为root,那么`find`将会执行`chown YourName {}` 它会被解析为c`hown YourName test1.txt` 和 `chown YourName test2.txt`.

另一个例子是将指定目录中的所有C程序文件拼接起来，写入带个文件all_c_files.txt,我们可以用find
找到所有的C文件，然后结合`-exec` 使用cat命令：

    find . -type f -name "*.c" -exec cat {} \; >all_files.txt


`-exec`之后可以接任何命令。{}表示一个匹配。对于任何一个匹配的文件名，{}会被该文件名所替代。

我们使用>操作符将来自`find`的数据重定向到all_c_files.txt文件，没有使用>>的原因是因为`find`命令全部处处只是一个单数据流，而只有当多个数据流被追加到单个文件中的时候才有必要使用>>。

例如，用下列命令将10天前的.txt文件复制到OLD目录中：

    find . -type f -mtime +10 -name "*.txt" -exec cp {} OLD \;


`find`命令同样可以采用类似的方法与其他命令结合起来。

有一点值得注意的是，我们无法在`-exec`参数中直接使用多个命令。它只能够接受单个命令，不过我们可以耍一个小花招。把多个命令写到一个shell脚本中，例如(commands.sh)然后在`-exec`中使用这个脚本：

    -exec ./commands.sh {} \;


`-exec`能够同`printf`结合来生成有用的输出信息。例如：


    find . -type f -name "*.txt" -exec
    print "Text file: %s\n" {} \;

在结合find执行命令的时候，我们一般都使用-exec，其实还有一个有用的参数-execdir，
在执行命令的时候，会跳到子目录中去执行相应的命令，例如，请把当前目录及所有子目录下的图片重命名为小写,
正确的方式就是像下面这样：

	find linziyu -type f -iname "*.jpg" -execdir rename 'y/a-z/A-Z/' {} \;   

###(10). 让find跳过特定目录

在搜索目录并执行某些操作的时候，有时为了提高效率，需要跳过一些子目录。例如：

    find . \( -name ".git" -prune \) -o
    \( -type f -name "*.html" \)

以上命令打印出不包括在`.git`目录中的所有文件的名称。
还有一种方法可以跳过特定目录，例如，当前文件夹包含很多文件和子文件夹，我们现在需要把除了某一文件以外的其他文件都删除，应该怎么做？(这是一个我经常碰到的问题，比如我从github 上下载了[git 的教程](https://github.com/blynn/gitmagic)，但是该教程下面有各种语言版本，显然我只需要保留简体中文版，所以我需要删除除了zh_cn 外的所有文件和文件夹)

    find . -maxdepth 1 ! -name "zh_cn" -exec rm -rf {} \;

首先我们指定搜索深度为1,这样才不会把zh_cn子目录下的文件给找出来，然后我们通过！来对文件名取反，再删除之。
