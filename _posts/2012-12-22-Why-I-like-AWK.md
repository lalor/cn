---
layout: post
title: "为什么我特别喜欢Awk"
description: ""
category: 程序语言
tags: [awk]
---

#1. 介绍

Awk是由Aho、Weinberger和Kernighan三位科学家开发的，特别擅长文本处理的linux 工具，该工具是 linux 下最常用的工具之一。Awk不是普通的工具，其实，也是一种编程语言，在编程语言[排行榜][1]上也能排上号的。

Google 的一位工程师（他的博客最近访问不了）写了一篇文章，介绍Awk，标题为《[Why you should learn just a little awk][2]》，该文章简单的介绍了Awk 的用法，在Reddit 和 Ycombinator 上引起了热烈讨论。好玩的是，文章的最后作者还留下邮箱，大意是"如果你是那种对awk感兴趣的人，毫无疑问，你就是那种我希望可以在Google共事的人，如果你感兴趣，可以给我简历（ggrothau@gmail.com），我可以确保它一定会出现在合适的招聘者面前，而不是遗落在我们每天收到的那一大堆简历中"。

我一直关注的一个博客，博主去AT&T实验室实习，总结的[文章][3]里有提到，AT&T里的科学家和工程师每天都在Awk/sed/grep 和管道中折腾。如果现在要让我选择最喜欢的编程语言的话，我会选择Awk ，Awk 不但简单易学，功能强大，而且能够通过管道与Shell脚本很好的交互，这是其他编程语言所不能及的。当然了，我们要用合适的工具和合适的语言做合适的事情，Awk并不是万能的，它的强项就是文本处理以及与Shell 脚本的交互。

我猜想我喜欢Awk 的另一个原因是它的语法与C 语言很像，相对于Python 和其他语言，Awk更符合我的思维方式。Awk和C语言很像，但是省去了初始化，内存分配，严格的语法等烦人的细节。这里还不得不提的是，Awk 三位创始人之一 Kernighan 就是《The C Programming Language》的第一作者，它的语法与C 语言相像也就不足为奇了吧。

#2. Awk

##2.1 Awk 程序的结构

下面根据我的理解简单介绍下Awk 的语法，然后摘录一些小例子演示Awk 的用法，再通过几个程序，演示Awk 的强大之处，最后，给大家推荐本参考书吧。

Awk 程序的结构如下：

	awk 'BEGIN{ print "start" } pattern { commands } END{ print "end" }  file

Awk程序由3部分组成：BEGIN语句块，END语句块，和能够使用模式（正则表达式/关系表达式）匹配的通用语句，这三部分中的任何一部分都可以省略，BEGIN语句块在程序最开始执行，通常完成一些初始化的工作，END语句块在程序的最后执行，通常在最后格式化输出结果。模式部分的工作原理如下：首先获取一行，检查该行是否与提供的样式匹配，如果匹配就执行与该样式对应的{}中的语句。

简单来说，就是首先执行BEGIN 语句块（如果有的话），然后逐行检查file 中的数据是否满足pattern， 如果满足，就执行{}中的语句，如果不满足，就什么也不做，在程序的最后执行END语句块（如果有的话）。

也可以通过管道获取数据，如下所示：

	cat file | awk 'BEGIN{ print "start" } pattern { commands } END{ print "end" }  

##2.2 Hello,world

下面我们来看一下在Awk 中如何打印Hello,world ， 在上面已经提到过，Awk
程序由三部分组成（BEGIN语句块，pattern ，
END语句块），三部分中的任何一部分都可以省略，最简单的方法打印Hello,world 如下：

* 只有BEGIN 语句块

		echo | awk 'BEGIN{ print "Hello, world"}'

* 只有END 语句块

		echo | awk 'END{ print "Hello, world"}'

* 只有pattern 部分

		echo | awk '{ print "Hello, world"}' 

在pattern 部分，因为pattern 为空，只有{}语句块，所以，程序认为每一行数据满足模式，都执行{}语句块中的内容。

##2.3 深入理解三个语句块

**再次强调：**Awk 在程序执行之前执行BEGIN
语句块，然后读入一行数据，检查该行数据是否满足模式，如果满足，则执行该模式对应的{}语句块里的内容，再次读入下一行数据，进行第二次模式检查，直到读完所有的数据，在程序的最后，执行END语句块中的语句。一个标准的Awk 程序如下：

	seq 5 | awk 'BEGIN{ print "begin"} $1 > 2 { print } END{ print "end" }'

输出结果：

	begin
	3
	4
	5
	end

该程序首先执行BEGIN语句块，输出begin,然后检查模式`$1 > 2`（稍后解释）是否满足，满足就执行{}块中的语句，即print ，在Awk 中，print 后面没有任何东西，意思就是打印该行内容，在程序最后执行END语句块，打印end 。

##2.4 Awk中的特殊变量

Awk 之所以处理文本非常方便，是因为它提供了很多内置的功能和变量，让我们可以方便的操作各数据。
 
* NR: 表示记录的数量(number of record)，在执行过程中，相当于当前行号
* NF: 表示当前行的字段数量(number of fields)
* $0 这个标量包含执行过程中当前行的文本内容
* $1 这个标量包含当前行中第一个字段的内容
* $2 当前行第二个字段的内容
* FS 域分割符，相当于sort 命令和cut 命令中的－d选项
* OFS 输出时，域分割符，默认是"\t"

##2.5 Examples

下面来看几个简单的例子，假设你有一个文件emp.data,里面的数据如下：

	Beth	4.00	0
	Dan	3.75	0
	Kathy	4.00	10
	Mark	5.00	20
	Mary	5.50	22
	Susie	4.25	18

其中，第一列是员工的名字，第二列是每小时的工钱，第3列是工作时长，为了得到每个员工的工钱，只需要下面这条语句即可：

	awk '$3 > 0 { print $1, $2 * $3 }' emp.data

由于Awk 已经将数据帮我们分割好了，我们只需要直接引用就好了，当我们使用变量$1 的时候，就指当前行的第一个域，也就是姓名，我们使用变量$3的时候，就是引用第3个域，即工作时长。


你如果想看哪些员工没有工作，即工作时长为0的员工，只需要像下面这样即可：

	awk '$3 == 0 { print }' emp.data

或者这样：

	awk '$3 == 0 { print }' emp.data

上面两条语句用于演示，print 不加任何参数，就是打印当前行中所有的内容，$0
也是代表当前行的所有内容，所以，上面两条语句的作用是一样的，如果，你只想看工作时长为0的员工的名字，则：

	awk '$3 == 0 { print $1 }' emp.data

你还可以把行号打印出来，以方便一眼看出有几个员工没有工作：

	awk '$3 == 0 { print NR, ":\t",  $1 }' emp.data

如果，你只想看Kathy 的工资，则：

	awk '$1 == Kathy { print $1, $2 * $3}' emp.data

或者使用正则表达式：

	awk '$1 ~ /Kathy/ { print $1, $2 * $3}' emp.data

Awk 中也可以用printf 格式化输出，printf 的使用方法和C 语言中printf
的使用方法一样，如：

	awk '$3 > 0{ printf("Total pay for %s is %.2f\n", $1, $2 * $3) }' emp.data

模式是可以结合使用的，如：

	awk '$2 >= 4 || $3 >= 20 {print $0}' emp.data

输出结果如下：

	Beth	4.00	0
	Kathy	4.00	10
	Mark	5.00	20
	Mary	5.50	22
	Susie	4.25	18
	
##2.6 数据验证

模式一般是用来选择需要处理的数据的，例如，上面我们就用$3 > 0
来选择工作时长不为0的员工，还可以向下面这样：

	awk 'NR < 5 {} '         # 只处理前4行，即行号小于5的行
	awk 'NR == 1, NR == 4{}' # 行号在1到4之间
	awk '/linux/{}'          # 包含样式linux 的行
	awk '!/lunux/{}'         # 不包含样式linux 的行

除此之外，模式还可以用来验证数据，如下所示：

	NF != 3   { print $0, "number of fields is not equal to 3" }
	$2 < 3.35 { print $0, "rate is below minimum wage"         }
	$2 > 10   { print $0, "rate exceeds $10 per hour"          }
	$3 < 0    { print $0, "negative hours worked"              }
	$3 > 60   { print $0, "too many hours worked"              }

##2.7 BEGIN语句块的作用

BEGIN语句块通常用于输出头信息，或者预先处理其他信息。例如，我们可以这样打印标题：

	awk 'BEGIN{print "NAME    RATE    HOURS"; print ""} { print $0 }' emp.data

则输出信息如下：

	NAME   RATE	HOURS
	
	Beth	4.00	0
	Dan	3.75	0
	Kathy	4.00	10
	Mark	5.00	20
	Mary	5.50	22
	Susie	4.25	18

更常用的用法是在BEGIN语句块中执行FS,即域分隔符，前面已经说过，域分隔符类似与sort命令和cut
命令中的－d选项，例如，你要在查看每个用于和他的Home目录，使用cut
的话，方法如下：


	cut -d: -f1,6 /etc/passwd

使用Awk也很方便

	awk 'BEGIN{ FS=":" }{ print $1, $6}' /etc/passwd

我们也可以将awk写入文件之中

	#print.awk - print user and it's home dir
	BEGIN{FS = ":"}
	{
		print $1, $6
	}

执行的时候只需要输入下如命令即可：

	awk -f print.awk /etc/passwd

或者
	
	cat /etc/passwd | awk -f print.awk

##2.8 END语句块

END语句块主要用来输出一些汇总信息，例如，我们想知道到底有多少员工：

	awk 'END { print NR, "employees"}' emp.data

我们也可以轻易得到总的工资和每个员工的平均工资：

	#count.awk - compute the average pay 
	{
		pay = pay + $2 * $3
		}
	END{ print NR, employees
		 print "total pay is", pay
		 print "average pay is", pay/NR
	}

	awk -f count.awk emp.data

输出结果如下：

	6 employees
	total pay is 337.5
	average pay is 56.25

##2.9 Awk中的控制语句

Awk 中的控制语句和C用法和C语言一样，不过没有Switch。
	
	#reverse- print input in reverse order by line
	{ line[NR] = $0 } #remember each input line
	END{ i = NR
		while ( i > 0 ){
			print line[i]
			i--
		}
	}


	#reverse - print input in reverse order by line
	{ line[NR] = $0 } #remember each input line
	END{
		for ( i = NR; i > 0; i--)
			print line[i]
		}

除此之外，for 循环略有差别, Awk 中的for 循环有两种方式：

1. `for ( i = 0; i < 10; i++) {print $i;}`
2. `for ( i in array){ print array[$i]; }`

##2.10 关联数组

所谓关联数组，就是既可以用数字也可以用字符串做下标的数组。为了演示关联数组的用法，考虑下面几个[实例][4]。假设有文件file,里面的数据如下：

	Item1,200
	Item2,500
	Item3,900
	Item2,800
	Item1,600

在file 文件中，有些Item 出现了多次，假设要对相同的Item 求和：

	awk -F, '{ a[$1] += $2} END{ for ( i in a ) print i "," a[i]}' file


输出结果如下：

	Item1, 800
	Item2, 1300
	Item3, 900

这里出现了两个新知识点，一个使用了关联数组，我们不需要初始化数组，也不用知道数组中有多少元素，因为我们可以在用第二种for循环的形式来输出数组中的内容；第二个知识点是print 语句，print 语句中没有使用逗号分隔，而是直接将各字段连接起来，在Awk中连接字符串的方法如下：

		str = "hello" "world" "!" $2

如果我们只想打印每项中数字最大的那一项，而不是把它们加起来，则可以像下面这样：
	
	awk -F, '{if (a[$1] < $2) a[$1] = $2} END{ for( i in a ){ print i, a[i]}}' 
	OFS=, file

输出结果如下：

	Item1,600
	Item2,800
	Item3,900

统计每项出现的次数：

	awk -F, '{ a[$1]++}END{ for ( i in a) print i, a[i]}' file

输出结果如下：

	Item1 2
	Item2 2
	Item3 1

打印每一项的第一次出现：

	awk -F, '!a[$1]++' file

输出结果如下：

	Item1,200
	Item2,500
	Item3,900
	
在这里，只有模式，没有{}语句块，则默认原样输出。


##2.11 Awk 中的内建函数

Awk 提供了很多函数来帮助用户提高工作效率

<a href="http://imgur.com/5Z4Sp"><img src="http://i.imgur.com/5Z4Sp.png" alt="" title="Hosted by imgur.com" /></a>
<a href="http://imgur.com/wE8TT"><img src="http://i.imgur.com/wE8TT.png" title="Hosted by imgur.com" alt="" /></a>

#3. [A handful of Userful "one-liners"][5]

* print the total number of input lines
	
		END { print NR }

* print the tenth input line:

		NR == 10

* Print the last field of every input line

		{ print $NF}

* print  the last field of the last input line

		{field = $NF}
		END { print field}

* print every input line with more than four fields

		NF > 4

* print every input line in which the last field is more than 4

		$NF > 4

* print the total number of fields in all input lines:

		{ nf = nf + NF }
		END { print nf }

* print the total number of lines that contain Beth

		/Beth/{nlines += 1}
		END { print nlines}

* print the largest first field and the line that contains it (assumes some $1
  is positive)

		$1 > max { max = $1; maxline = $0 }
		END { print max, maxline }

* print every line that has at least one field

		NF > 0

* print every line longer than 80 characters

		length($0) > 80

* print the number of fields in every line followed by the line itself:

		{print NF, $0}

* print the first two fields in opposite order, of every line
	
		{ print $2, $1}

* Exchange the first two fields of every ine and then print the line
		
		{ temp = $1; $1 = $2; $2 = temp; print}

* print every line with the first field replaced by the line number

		{$1 = NR; print}

* print every line after erasing the second filed

		{$2 = ""; print}

* print in reverse order the fields of every line

		{ for ( i = 1; i <= NF; ++i)printf ("%s ", $i)
			print ""
		}

* print the sums of the fields of every line
	
		{ 
			sum = 0
			for ( i = 1; i <= NF; ++i) sum += $i
			print sum
		}

* Add up all fields in all lines and print the sum

		{ for ( i = 1; i <= NF; ++i ) sum += $i
		END { print sum}

* print every line after replacing each field by its absolute value

		{
			for ( i = 1; i <= NF; ++i) if ( $i < 0 ) $i = -$i
			print
		}

#4. Awk 程序的应用

##4.1 统计单词出现次数

我们分析的第一个程序是一个统计单词出现次数的程序，由于Awk 提供关联数组，而且默认将变量初始化为0
，所以Awk解决这个问题可谓是得心应手，我在之前的文章中讨论过这个问题，在那篇文章中试图用c,c++,shell
来解决。

但是这个问题用Awk 来解决最简单，还能轻易的处理标点符号的问题，程序如下：

	#wordfreq- print number of occurences of each word
	#input: text
	#output: number-word pairs sorted by number
	
	{ gsub(/[.=,:;!?(){}]/, "") #remove punctuation
		for ( i = 1; i <= NF; i++)
			count[$i]++
			}
	END {
		for ( w in count)
			print count[w], w | "sort -rn"
	}

##4.2 数据处理

虽然Awk 不是全能的，但是也能解决很多问题，但是，它最擅长的还是数据处理，假设有如下数据：

	USSR	8649	275	Asia
	Canada	3852	25	North America
	China	3705	1032	Asia
	USA	3615	237	North America
	Brazil	3286	134	South America
	India	1267	746	Asia
	Mexico	762	78	North America
	France	211	55	Europe
	Japan	144	120	Asia
	Germany	96	61	Europe
	England	94	56	Europe

第一列是国家名，第二列是该国所占的面积，第三列是人口数量，最后一列是所属的洲，如果，我们希望得按洲的名称升序排序，再按每个国家人口密集度降序排序的表格，我们应该怎么做呢？

我不知道Excel 怎么做，应该是可以，学习Awk 的好处就在于，你可以通过Awk
解决很多Excel 解决的问题，而不用去研究傻瓜式的Excel ， 很明显，Awk 更灵活。

下面来解决这个问题，Awk 解决这类问题的基本思路是，准备数据－－排序（或其他处理）－－格式化输出。

##4.3 有穷自动机

##4.4 Markov 链算法

Markov 链算法的定义见[这篇文章][6]，我在那篇文章中讨论了各种语言实现Markov 链算法的实现，这简直就是演示Awk 优点的问题，具体程序如下，不再作详细讨论。

	# Copyright (C) 1999 Lucent Technologies
	# Excerpted from 'The Practice of Programming'
	# by Brian W. Kernighan and Rob Pike
	
	# markov.awk: markov chain algorithm for 2-word prefixes
	BEGIN {	MAXGEN = 10000; NONWORD = "\n"; w1 = w2 = NONWORD }
	
	{	for (i = 1; i <= NF; i++) { 	# read all words
			statetab[w1,w2,++nsuffix[w1,w2]] = $i
			w1 = w2
			w2 = $i
		}
	}
	
	END {
		statetab[w1,w2,++nsuffix[w1,w2]] = NONWORD	# add tail
		w1 = w2 = NONWORD
		for (i = 0; i < MAXGEN; i++) {	# generate
			r = int(rand()*nsuffix[w1,w2]) + 1  # nsuffix >= 1
			p = statetab[w1,w2,r]
			if (p == NONWORD)
				exit
			print p
			w1 = w2			# advance chain
			w2 = p
		}
	}	


##4.5 本人遇到的一个小问题

#5. 注意事项



#6. 学习资料

最好的学习资料非《The Awk Programming Language》不可，这本书是Awk 的创始人，也《The C Programming Language》的作者，老头子文笔非常之好，非常简单的语言，把问题解释的非常清楚，要让我选诺贝尔文学奖获得者，我一定会第一给他投票，老头子实在是太NB了，他的其他书《The Practice of Programming》和《Unix 编程环境》也很不错，强烈推荐。除此之外，还要想看更多Awk例子的，可以看看《Effective Awk Programming》， 千万别看什么《Sed && Awk》，全是废话，垃圾书。

如果不想正儿八经的看书，网上也有很多资料，强烈推荐[theunixschool.com][4],需要翻墙，里面有很多Shell脚本学习的资料。


[1]: http://developer.51cto.com/art/201202/315529_1.htm
[2]: http://www.kuqin.com/linux/20111027/313907.html
[3]: http://yihui.name/cn/2012/08/quo-vadis/
[4]: http://www.theunixschool.com/p/awk-sed.html
[5]: http://book.douban.com/subject/1876898/
[6]: http://mingxinglai.com/cn/2012/12/markov/
