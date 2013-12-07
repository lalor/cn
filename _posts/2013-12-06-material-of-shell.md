---
layout: post
title: "shell脚本学习材料"
description: ""
category: 程序语言
tags: [shell]
---

本着对linux和shell脚本的极大兴趣，我看了不少shell脚本相关的书和文章，也有了很多学习心得，
回首自己的linux和shell脚本学习历程，不禁发现自己竟也走了不少的弯路，完全是靠着自己的满腔
热情走到今天。为了帮助有缘的朋友少走一点弯路，这里向大家推荐一些学习的资料，
资料包括书，开源免费书，博客和相关网站。这里推荐的材料都属于进阶类型，特别适合已经掌握
了一些shell脚本的基础知识，并希望深入学习shell脚本的朋友。

### 1. 书

1. linux命令行与shell脚本编程大全<sup>[1][url1]</sup>

    ![img1][img1]
    这本书是入门级的读物，作为入门材料非常合适。可惜我入门的时侯没有遇到它。虽然很大
    部头，其实一天也就能看完了。讲的东西比较全面，也比较简单。三颗星推荐。


2. linux shell脚本攻略<sup>[2][url2]</sup>

    ![img2][img2]
    这是我强烈推荐的一本书，我的shell脚本从新手到老手全靠这本书。这本书很薄，也精华。
    与上面的那本书不一样，这本书追求的不是全，而是精，是各种各样的奇技淫巧，是老手
    天天用却不会告诉你或者他们也没有意识到的技巧。五颗星推荐。

    此外，这本书的作者是一个印度人，二十岁的时侯写下了这本书，那时他已经是Ubuntu
    和Fedora的源码贡献者了。


3. The AWK Programming Language<sup>[3][url3]</sup>

    ![img3][img3]

    这本书的作者就是AWK程序语言的设计者，所以，其权威性毋庸置疑。值得注意的是，这本书的第二作者
    Brian W. Kernighan是神一样的科学家，他是《The C Programming Language》的作者。
    Brian W. Kernighan 的《程序设计实践》也是一本好书。

    AWK的学习资料网上一搜一大片，其实，只要静下心来把这本书中的代码敲一遍，就可以无视其他所有材料了。
    这本书网上有电子版。很奇怪的是，这么好的一本书，国内的出版社怎么没有引入，可能是AWK太老太小众了吧。
    总之，学习AWK这是必读的一本书，五颗星推荐。


### 2. 免费电子书

1. 高级shell脚本编程指南<sup>[4][url4]</sup>
    ![img4][img4]

    高级shell脚本编程指南，也就是传说中的ABS，你如果到论坛里去问有什么好的学习shell脚本
    的进阶书籍，大多数人都会推荐这本书。正如这个世界上最珍贵的东西（如：空气，阳光，
    亲情与爱）都是免费的一样，这本书也是免费的，是网上开源的免费书籍，任何人都可以获得。
    已经有热心的朋友将它翻译成了中文版，不过还是推荐看英文版，专业书籍使用的英文单词都不难，
    大家应该尽快习惯看英文版的书籍。四颗星推荐。


2. sed and awk 101 hacks<sup>[5][url5]</sup>
    ![img5][img5]

    这是一本印度人写的开源免费书籍（印度人好像比较喜欢也有能力干这事），应该算是入门书籍，
    不过作者表达能力太强，交代的东西也很多，所以无论老手还是新手，看这本书都会有一定的收获。

    此外，该作者还写了另外两本同样有名的开源书籍，即《vim 101 hacks》和《linux 101 hacks》。
    这三本书都四颗星推荐。


### 3. 博客 && 网站

1. One-Liners Explained<sup>[6][url6]</sup>

    除了上面推荐的比较好的书以外，网络上也有一些比较好的博文。比如，大名鼎鼎的One-liners
    Explained 系列。这个系列共有4篇文章，都是进阶级的，需要较强的功底才能看懂。

    这4篇文章是：

    * Bash One-Liners Explained
    * sed One-Liners Explained
    * awk One-Liners Explained
    * perl One-Liners Explained

2. commandlinefu.com<sup>[7][url7]</sup>

    这是一个很有名的网站，专门收集一些很酷很简洁的命令。

    **例如:**当你使用vim编辑一个文档，保存的时侯才提示你没有权限，这个时侯怎么办呢？
    如果你知道`:w !sudo tee %`命令就不用烦恼了，这个牛逼的命令我就是在commandlinefu上学会的，
    也是我至今觉得最酷的一个命令。

    * Top Ten One-Liners from CommandLineFu Explained
    One-liners Explained 系列的作者还根据CommandLineFu 写了另外一篇文章，即
    [Top Ten One-Liners from CommandLineFu Explained](http://www.catonmat.net/blog/top-ten-one-liners-from-commandlinefu-explained/)

3. the unix school<sup>[8][url8]</sup>

    the unix school是一个印度哥们维护的博客，这个博客专注于linux与shell，最大的特色
    是尝试使用不同的方法解决同一个问题，对于开阔视野，拓展思路非常有用。

    the unix school 是我最喜欢的博客之一，里面的每一篇博文我都认真看过，强烈推荐。

4. bash Pitfalls<sup>[9][url9]</sup>

    Bash Pitfalls文章介绍了40多条日常Bash编程中，老手和新手都容易忽略的错误编程习惯。
    每条作者在给出错误的范例上，详细分析与解释错误的原因，同时给出正确的改写建议。

    已经有朋友(网名：团子)将这篇文章翻译成了[中文版](http://kodango.com/bash-pitfalls-part-1)。

5. 简洁bash编程<sup>[10][url10]</sup>

    团子写的简洁bash编程也值得一看。

    里面虽然有很多知识你可能在其他地方已经见过，但是这么多编程技巧里面，只要有一条你不知道，
    并在知道以后长期使用就值得你看了。

    我第一次写一个较复杂的脚本的时侯，用到了多进程，需要在主进程里面等待子进程结束。当时我的做法
    是记录下子进程的pid(sleep 10 &; pid= $!)，然后使用while
    循环判断进程是否结束(kill -s 0 $pid)，进程少还行，子进程多了一后就没法这么干了。
    其实有一个很好也很方便的做法，使用`wait`命令即可。这个命令我就是在简洁bash编程里学到的。



### 4. 后记

认真学习完这里推荐的资料，就算得上是shell脚本老手了，不过，凡事都是凭兴趣，我相信这篇文章对
shell脚本感兴趣的新手会有很大的帮助。不过，还是要切记，多用多动手才是学习linux和shell的最佳方法。

我真的很佩服印度人，他们非常有才华，也善于表达，还具有开源精神。《linux shell脚本攻略》的
作者一直致力于开源软件，年纪轻轻就向Ubuntu和Fedora贡献源码（我也想，能力不够），还维护了自己的
linux发行版。二十岁时写下的这本《linux shell脚本攻略》，现在已是我必备书籍。

101 hack系列的是一个印度人写的，the unix school是我最喜欢的博客之一，博主也是一个印度人，
大名鼎鼎的《a byte of python》与《a byte of vim》(与这篇博客无关)的作者也是印度人，
我就发现这些印度人确实很厉害，他们能用简单易懂的方式传递知识，而且他们也很愿意无偿的贡献自己劳动成果。

有感于印度人的才华与表达能力，并且网络上已经有很多优秀的shell脚本学习资料。所以我打消了写shell
相关博客的想法。仅用此文收集一些优秀的书籍和博文，以后发现好的资料还会再补充。也欢迎补充优秀
的学习资料。




[url1]: http://book.douban.com/subject/11589828/
[url2]: http://book.douban.com/subject/6889456/
[url3]: http://book.douban.com/subject/1876898/
[url4]: http://www.tldp.org/LDP/abs/html/
[url5]: http://www.thegeekstuff.com/sed-awk-101-hacks-ebook/
[url6]: http://www.catonmat.net/series/sed-one-liners-explained
[url7]: http://www.commandlinefu.com/commands/browse
[url8]: http://www.theunixschool.com/
[url9]: http://mywiki.wooledge.org/BashPitfalls
[url10]:http://kodango.com/simple-bash-programming-skills

[img1]: /cn/image/material_of_shell01.jpg
[img2]: /cn/image/material_of_shell02.jpg
[img3]: /cn/image/material_of_shell03.jpg
[img4]: /cn/image/material_of_shell04.jpg
[img5]: /cn/image/material_of_shell05.png
