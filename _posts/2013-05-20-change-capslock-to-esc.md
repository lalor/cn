---
layout: post
title: "将Caps Lock转换成Esc(windows and linux)"
description: ""
category: 工具
tags: [vim, esc]
---

##1. linux 下将Caps Lock 转换成Esc

作为一个vimer，Caps Lock对我（还有其他很多人）来说根本就是多余的，而且在键盘中的位置根本不合理，一不小心就按到了Caps Lock，然后就出错了。Esc又是vim下使用最为频繁的按键，将Caps Lock替换成Esc真是再好不过了，而且不用担心按ESC的时候按到了该死的F1。

可能有人会问，这样对于连续的大写不是就不方便了吗？我们可以先用小写，然后选中该段文中，按U，就变成大写了。对于在vim中的大写，这里还有一个小技巧推荐给大家：

	inoremap <C-u> <esc>gUiwea  

将上面这条语句是放在.vimrc文件里的，它的作用就是在编辑状态下，按ctrl+u，将你刚刚输入的那个单词变成大写。谁用谁知道。对一个用vim 作为主要编辑器的人，有了上面两条技巧以后， Caps Lock 就是多余的了。

在linux[将Caps Lock转换成Esc][1]非常简单，只要在.profile文件加入下面这条语句即可。

	xmodmap -e 'clear Lock' -e 'keycode 0x42 = Escape'  

当你不需要的时候，只要将这条语句删除，重启即可。


##2. windows下将Caps Lock 转换成Esc

在linux下把Caps Lock当Esc习惯以后，到windows下自然也希望将Caps
Lock转换成Esc，如果用搜索引擎搜"windows Esc Caps lock"，很轻松的就找到了下面的答案：

1. 新建文本文档，重命名为capslock2esc.reg
2. 输入如下内容

		[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Keyboard Layout]
		"Scancode Map"=hex:00,00,00,00,00,00,00,00,03,00,00,00,3a,00,01,00,01,00,3a,00,00,00,00,00

3. 然后保存，并且双击写入注册表，重新登录计算机

这个答案有个问题就是交换了Caps Lock与Esc
，当我向我们组的同事请教问题的时候，他需要在我的电脑上操作，按Esc确是Caps Lock
显然让他很不习惯，所以，我又搜了一下，如何将Caps Lock转换成Esc
，而不是交换它们。我找到了[这篇文章][2]，讲得不错，可惜有错误。

为了完整，把这篇文章转过来了，并修正了错误。方法还是和上面的一样，不过多了解析。

下面分析"Scancode Map"后面一长串数字， 为方便分析，拆分为每8位一行，并加上行号，结果如下：

	1. 00,00,00,00,
	2. 00,00,00,00,
	3. 03,00,00,00,
	4. 3a,00,01,00,
	5. 01,00,3a,00,
	6. 00,00,00,00

头两行和最后一行，是固定的，都是8个0。  
第3行，表示共更改了多少个按键，本例为3个，怎么算的？从第4行开始，到最末尾行（虽然严格来说末行不算），每行算1个，4、5、6行，刚好3个  
第4行，前后各4位，分别代表某个按键。本例为将0100的按键映射为3a00。3a00代表CapsLock键，0100代表Esc键  
第5行，同理，3a00的按键映射为0100，即映CapsLock射为Esc  
第6行，已经讲了，末行，固定8个0。如果需要增加更多的映射，可以在此行之上，不断加类似4、5行格式的。当然，最后的结果，还是要将各行合为一行的。  
每行的4个数字，两个数字一组，代表一个按键，在映射时，他们的关系类似与c语言的`char *strcpy(char *strDest, const char *strSource)`函数，把后面一组的意义赋给前一组。 


综上所述，将Caps Lock映射成Esc，而不是交换Esc与Caps Lock的代码如下：

		[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Keyboard Layout]
		"Scancode Map"=hex:00,00,00,00,00,00,00,00,02,00,00,00,01,00,3a,00,00,00,00,00


叫上面代码保存到capslock2esc.reg 文件中，双击运行，重启电脑即可。


还原按键时，只需要在注册表的路径(`[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Keyboard Layout]`)下，删除Scancode Map键即可。


###附录


部分按键映射对照表。

	Escape                01    00
	Tab				      0F    00
	Caps Lock             3A    00
	Left Alt              38    00
	Left Ctrl	          1D    00
	Left Shift            2A    00
	Left Windows          5B    E0
	Right Alt             38    E0
	Right Ctrl            1D    E0
	Right Shift           36    00
	Right Windows         5C    E0
	Backspace             0E    00
	Delete                53    E0
	Enter                 1C    00
	Space                 39    00
	Insert                52    E0
	HOME                  47    E0
	End                   4F    E0
	Num    Lock           45    00
	Page   Down           51    E0
	Page   Up             49    E0
	Scroll Lock           46    00


还有一个特殊的值 —— 00 00，表示啥也不干，如果要将某个按键禁用，只需映射为00 00。


[1]: http://stackoverflow.com/questions/2176532/how-to-map-caps-lock-key-in-vim
[2]: http://xyztony1985.blog.163.com/blog/static/3611782011752420104/
