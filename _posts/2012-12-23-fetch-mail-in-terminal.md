---
layout: post
title: "在终端检查gmail新邮件"
description: ""
category: 工具
tags: [awk, gmail]
---

如何在终端下面检查gmail 新邮件？这个需求很变态？别说，就有人有这么变态的需求，我在就ChinaUnix看到有人因为这个问题而头疼，正在寻求[帮助][1]。

下面这个脚本是我写的用于在linux终端获取检查gmail新邮件的脚本，用于帮助那个可怜的娃。其实，我并没有特别善良，或者特别乐于助人，只是晚上刚写完一篇关于Awk 的文章，就看到这个问题，实在是忍不住练练手，也证明自己Awk 掌握得还不错，哈哈。下面就来解析一下这个脚本。

	curl -u Email:Password --silent "https://mail.google.com/mail/feed/atom" |
	awk 'BEGIN{flag=0}
	/<entry>/{flag=1;}
	flag==1{print}
	/<\/entry>/{flag=0;print ""
	}' |
	awk 'BEGIN{RS=""; FS="\n"}
	{
		print  "邮件：" NR; 
		print "主题：" $2; 
		print "发件人："$9; 
		print "发件人邮箱："$10; 
		print ""
	}' |
	sed 's/<\/.*>//g' | sed 's/<.*>//g' 


首先，要实现这个功能，你得知道gmail 提供了新邮件查询功能，就是访问<https://mail.google.com/mail/feed/atom> ，新邮件会以xml 格式显示出来，如下所示：


	<feed xmlns="http://purl.org/atom/ns#" version="0.3">
	<title>Gmail - Inbox for mingxinglai@gmail.com</title>
	<tagline>New messages in your Gmail Inbox</tagline>
	<fullcount>2</fullcount>
	<link rel="alternate" href="http://mail.google.com/mail" type="text/html"/>
	<modified>2012-12-23T14:03:40Z</modified>
	<entry>
	<title>test subject</title>
	<summary>test context 来自网易手机邮箱了解更多</summary>
	<link rel="alternate" href="" type="text/html"/>
	<modified>2012-12-23T14:01:22Z</modified>
	<issued>2012-12-23T14:01:22Z</issued>
	<id>tag:gmail.google.com,2004:1422153516998520296</id>
	<author>
	<name>赖明星</name>
	<email>18050565577@163.com</email>
	</author>
	</entry>
	<entry>
	<title>[lalor.github.com] Page build successful</title>
	<summary>
	Your page has been built. If this is the first time you've pushed
	</summary>
	<link rel="alternate" href="" type="text/html"/>
	<modified>2012-12-23T11:57:28Z</modified>
	<issued>2012-12-23T11:57:28Z</issued>
	<id>tag:gmail.google.com,2004:1422145720861872668</id>
	<author>
	<name>GitHub</name>
	<email>noreply@github.com</email>
	</author>
	</entry>
	</feed>

知道怎么获取新邮件以后，剩下的就是解析文本了，一个<entry>就是一封新邮件，所以，首先，将一封封邮件分开：

	awk 'BEGIN{flag=0}
	/<entry>/{flag=1;}
	flag==1{print}
	/<\/entry>/{flag=0;print ""}' 

遇到`<entry>`就将flag 置1,然后输出接下来的内容，遇到`</entry>`就将flag
置0,停止输出，并且输出一个空行，以方便接下来的处理。

接下来就是输出邮件主题和发件信息了，这涉及Awk 中多行处理的情况，在Awk 中，令`RS=""` ，将多行视为1条记录，每条记录之间有空行分隔（这也就是我们前面要在每封邮件后面输出一个空行的原因），令`FS="\n"`则每行就是一个域，我们输出相应的域即可。

	awk 'BEGIN{RS=""; FS="\n"}
	{
		print "邮件：" NR; 
		print "主题：" $2; 
		print "发件人："$9;
		print "发件人邮箱："$10; 
		print ""
	}'

输出相应的域以后，在去掉标签`<name>`，`<\name>`，`<email>`等，这就交给后面的sed  吧，就这样，一个在终端检测新邮件的脚本就搞定了。

在我手头的《[Linux Shell 脚本攻略][2]》也有一个类似的脚本，它完全用sed来解析上面的xml文件，着实让人看了头大，相信我，用Awk绝对要少死很多脑细胞。所以，学点Awk吧，我这里还有[教程][3]呢。

[1]: http://bbs.chinaunix.net/thread-4060260-1-1.html
[2]: http://book.douban.com/subject/6889456/
[3]: http://mingxinglai.com/cn/2012/12/Why-I-like-AWK/

