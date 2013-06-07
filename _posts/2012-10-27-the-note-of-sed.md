---
layout: post
title: "sed 学习笔记"
description: "sed 学习笔记"
category: 工具
tags: [sed, shell]
---

##1. 介绍

之前转载了一篇关于sed的[文章][1]，刚开始觉得很不错，慢慢的，越来越觉得没什么用，因为我写博客，主要是作学习笔记，方便自己日后查阅，之前转载的文章只是简单的罗列参数，并没有详细的实例，又没有sed巧妙的用法，让我查阅起来很是不便，于是，我决定重写这篇文章。


##2. 基本替换

sed可以替换给定文本中的字符串，最常见的使用方式是利用正则表达式进行匹配：

	sed 's/pattern/replace_string/' file

或者
	
	cat file | sed 's/pattern/replace_string/' 

sed
并不会改动原始文件，而是将编辑后的结果输出到控制台，很多时候，我们是希望将sed
的替换结果应用到原文件的，很多用户在进行替换以后，借助重定向来保存文件：

	sed 's/text/replace/' file > newfile
	mv newfile file

其实只增加一个参数就可以了，如下所示：

	sed -i 's/text/replace/' file 

上面的所有替换命令都只会替换每一行的第一处符合样式的地方，如果要替换所有符合样式内容，就在末尾加上g:

	sed  's/text/replace/g' file 

我们知道，上面这条语句是用replace 去替换问中的text ，所以，我们要删除文中的text
只需要像下面这样就行了：

	sed  's/text//g' file 



##3. 定界符

字符'/' 在 sed 中作为定界符使用，我们可以像下面一样使用任意的定界符号：

	sed 's:text:replace:g'
	sed 's|text|replace|g'

有时候修改定界符可以让程序看起来很简单明了，例如，我之前就遇到一个问题，需要将一个文件中的所有'$libdir' 替换成 '/usr/local/pgsql'

	sed 's/\$libdir/\/usr\/local\/pgsql/g' file

上面的程序太不优美，转义符太多，看起来很不直观，这时候我们换一个定界符就会好很多，好看不是主要的，主要的是我的目标字符串'/usr/local/pgsql'是存在一个变量里的，如果不用其他定界符，这个问题，以我对shell脚本的熟悉程度，实在是没辄了。先一步一步来，下面是修改了定界符的版本：

	sed "s#$libdir#/usr/local/pgsql#g" file

进一步的，我们将目标字符串放在一个变量中，增加程序的灵活性：

	str="/usr/local/pgsql"
	sed "s#\$libdir#$str#g" file

注意事项：

1. $libdir在写的时候$需要转义 sed "s/\$libdir/someThingElse/g" file
2. sed里面有变量的话，需要用双引号，而不是单引号
3. 用其他符号来代替/ ，可以不用那么多的转义，显得更清晰

##4. 几个小例子


###4.1 移除空白行

用sed 移除空白行不过是小菜一碟。空白行可以用正则表达式\^$进行匹配：

	sed '/^$/d' file

/pattern/d 会移除匹配样式的行，在空白行中，行尾标记紧随着行首标记。


###4.2 已匹配字符串标记 &

在sed 中，用&标记匹配样式的字符串(表示匹配的字符串)，就能够在替换字符串时使用已匹配的内容。例如：

	$echo "China is a developing country" | sed 's/developing country/"&"/g'
	China is a "developing country"    


###4.3 字串匹配标记 \1

在上例中，&用于表示匹配的整个字符串，那么，我们能否获取匹配的子串呢？答案是肯定的，下面的例子演示了如何引用匹配的子串：

	$echo seven EIGHT | sed 's#\([a-z]\+\) \([A-Z]\+\)#\2 \1#' 	
	EIGHT seven

###4.4 组合多个表达式

利用管道，连续使用多个sed命令：

	sed 'expression' | sed 'expression'

它等价于：

	sed 'expression; expression'


###4.5 sed 中的删除

	sed '2d' example      # 删除example文件的第二行
	sed '2,$d' example    # 删除example文件的第二行到末尾所有行
	sed '$d' example      # 删除example文件的最后一行
	sed '/test/'d example # 删除example文件所有包含test的行

###4.6 sed 中的替换

	sed 's/test/mytest/g' example            # 在整行范围内把test替换为mytest。如果没有g标记，则只有每行第一个匹配的test被替换成mytest


	sed -n 's/^test/mytest/p' example        # n选项和p标志一起使用表示只打印那些发生替换的行。也就是说，如果某一行开头的test被替换成mytest，就打印它

	sed 's/^192.168.0.1/&localhost/' example # 符号表示替换换字符串中被找到的部份。所有以192.168.0.1开头的行都会被替换成它自已加localhost，变成192.168.0.1localhost


	sed -n 's/\(love\)able/\1rs/p' example   # love被标记为1，所有loveable会被替换成lovers，而且替换的行会被打印出来。


###4.7 sed 中选定行的范围：逗号


	sed -n '/test/,/check/p' example          # 所有在模板test和check所确定的范围内的行都被打印。

	sed -n '5,/^test/p' example               # 打印从第五行开始到第一个包含以test开始的行之间的所有行。

	sed '/test/,/check/s/$/sed test/' example # 对于模板test和west之间的行，每行的末尾用字符串sed test替换。


###4.8 多点编辑：e命令

	sed -e '1,5d' -e 's/test/check/' example                        # e)选项允许在同一行里执行多条命令。如例子所示，第一条命令删除1至5行，第二条命令用check替换test。命令的执行顺序对结果有影响。如果两个命令都是替换命令，那么第一个替换命令将影响第二个替换命令的结果。

	sed --expression='s/test/check/' --expression='/love/d' example # 一个比-e更好的命令是--expression。它能给sed表达式赋值。


###4.9 从文件读入：r命令 

	sed '/test/r file' example #file里的内容被读进来，显示在与test匹配的行后面，如果匹配多行，则file的内容将显示在所有匹配行的下面。


###4.10 写入文件：w命令

	sed -n '/test/w file' example #在example中所有包含test的行都被写入file里。


###4.11 追加命令：a命令

	sed '/^test/a\ this is a example' example < #this is a example'被追加到以test开头的行后面，sed要求命令a后面有一个反斜杠。

	sed '/test/{ n; s/aa/bb/; }' example            #如果test被匹配，则移动到匹配行的下一行，替换这一行的aa，变为bb，并打印该行，然后继续。


###4.12 变形：y命令 

	sed '1,10y/abcde/ABCDE/' example                #把1--10行内所有abcde转变为大写，注意，正则表达式元字符不能使用这个命令。


###4.13 替换匹配行的一部分

	这是我亲自遇到的一个问题，我有如下一行代码，需要将该代码替换成SOCKET(/usr)变量。

##5. 删除C++源文件中的所有注释，

如何删除源文件的注释，这面这个脚本是我从互联网上找到的，可以实现该功能，用到了很多转义符，我实在是没有什么心情解释这个脚本，不过，这个脚本的关键的地方（对于新手，也是唯一的难点）就是如何删除使用`/* */` 注释的代码，并且注释有跨行的情况，这种情况会在下面解释。

	#!/bin/sh -f
	
	function del_comment_file()
	{
	#C++模式注释的删除。
	#Delete the line begins with //
	#首先删除//在行首的行        
		sed -i '/^[ \t]*\/\//d' $file
	
	#Delete the line ends with //
	#注意已经除去了
			//在行首的情况,下面匹配除去URL的//部分,因为代码中有一部分中有
	#URL，形如fun（"ftp://"）
			sed -i 's/\/\/[^"]*//' $file
	
	#删除以C语言中注释只在一行的行
			sed -i 's/\/\*.*\*\///' $file
	
	#Delete the lines between /* and */
	#删除C语言中注释跨行的情况
			sed -i '/^[ \t]*\/\*/,/.*\*\//d' $file
	}
	
	
	function del_comment()


##6. 删除html 标签

我们将分析一个html文件，通过删除文件中的所有标签，获取数据，在实现这个脚本之前，让我们先看一个有趣的问题（别说，我就遇到过）, 假设有下面的文件：

	<!doctype html>
	<html>
		<head>
			<meta content="text/html;charset=gb2312">
			<title>Here is the page title</title>
		</head>
	<body>
		This is the <b> first </b> lin in the web page. This should provide
		some <i> userful</i> information for us to use in our shell script.
	</body>
	</html>
	<!--708867ba7d22710c-->

如何删除`<html> ... </html>`之间的内容？

	sed '/<html>/,/<\/html>/d' file.html    

如何截取`<html> ... </html>`之间的内容？

	sed '/<html>/,/<\/html>/!d' file.html    

这里需要解释一下，因为`<html>`和`<\html>`不在同一行，属于跨行删除，所以要用到`\,`
只需要知道`\,`用于跨行模式匹配，就这么简单，知道了`\,`用于跨行模式匹配，上面那个删除c++
注释的脚本也就那么一回事了。

现在的问题是如何删除html文件中的所有标签，千万不要怀疑需求变态，我也[遇到过][4]。

遇到这个问题，你可能会和我一样，直接写出下面的代码：

	sed 's/<.*>//g' file.html

输出结果如下：

	
	
	
	This is the  lin in the web page. This should provide
	some  information for us to use in our shell script.
	
	

是不是不该删的东西也被删了，我之前也发现这个问题了，所以，我先删除结束标签，再删除开始标签：

	cat file.html sed 's/<\/.*>//g' | sed 's/<.*>//g'

虽然问题解决了，不过不太优美，优美的解决方案是让sed
忽略掉任何嵌入到原始标签中的大于号。要这么做，可以创建一个字符组来排除大于号（可能比较拗口，看代码更直接），代码如下：

	sed 's/<[^>]*>//g' file.html

这样就搞定了，最后，让我们再把工作做得好一点，用一条删除命令来删除多余的空白行：

	sed 's/<[^>]*>//g; /^$/d' file.html

删除空白行以后，看起来就清爽多了。


##7. 给代码添加行号

这个问题也是我最近遇到的，在写论文的时候，我们需要给源文件加上行号，如下所示：

     1.	#include <iostream>
     2.	using namespace std;
     3.	
     4.	int main(int argc, char* argv[])
     5.	{
     6.	    cout << "This is an example" << endl;
     7.	    return 0;
     8.	}


对于第2个问题，我一开始就想到用vim的宏录制，其实这个问题用sed更简单，就是一行代码的事：

    cat -n temp.cpp | sed 's/^ *[0-9]*/&./g'  

##8. next 命令

下面考虑一个极其变态的问题，有如下文件，如何只删除第2行的空行而不删除第4行的空行？

	This is the header line.
	
	This is a data line.
	
	This is the last line.


我们之前删除空行都是用`sed /^$/d' file` 这样的脚本，但是，这样会把所有的空行删掉，由于要删除的行是空行，你没有任何可以查找的文本来唯一识别该行。解决办法是用n命令，所谓n命令，就是先用模式定位，然后遇到n命令，跳到下一行，所有的操作都是对已经匹配模式的下一行进行的操作。如下所示：

	sed '/header/{n ; d}' file	
	
n 会首先找到匹配的模式，然后将跳到下一行执行相关操作，而N
则是首先找到匹配的模式，然后将下一行读入与当前行合并，再执行相关操作，这会用得到吗？我们考虑下面的问题，将如下文件中的System Administrators 替换成 Desktop Users ，但是，很不幸，它们不在同一行，模式匹配失败。

	The first meeting of the linux System
	Administrator's group will be held on Tuesday.
	All System Administrator should attend this meeting.
	Thank you for you attendence.

这时候就要用到N了，替换命令如下：

	sed 'N; s/System.Administrator/Desktop User/' data  

至于为什么N和n的使用方法（位置）不一样，这个我也不理解，就像java中数组有length property,
Strings有length method , 而Lists 使用size method 一样，设计得很糟糕，不是吗？

完。

[1]: http://www.tsnc.edu.cn/default/tsnc_wgrj/doc/sed.htm
[2]: http://bbs.chinaunix.net/forum.php?mod=viewthread&tid=3775356&page=1#pid22322848
[3]: http://book.douban.com/subject/1236944/
[4]: http://mingxinglai.com/cn/2012/12/fetch-mail-in-terminal/
