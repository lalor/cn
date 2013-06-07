---
layout: post
title: "HTML5幻灯片"
description: ""
category: 程序语言
tags: [HTML5, PPT, keydown]
---

偶然间在一博客里看到HTML5做的[幻灯片][1],非常漂亮,于是心动了,琢磨着自己也搞一个。作为一个前端白痴,没有工具的辅助,是万不可能自己搞定的,原作者就提示自己使用的是keydown, Ruby Gem,而我当时连Ruby Gem是什么都不知道,也不知道如何安装keydown,最后通过与作者邮件交流,算是搞定了,但是非常难看,各种折腾,最后自己搞定。

首先展示一下我做的[幻灯片][2],源代码在[这里][3],非常的简单,制作幻灯片的具体步骤已经在[幻灯片][2]中讲了,这里需要友情提示的是,如果你按照我的方法,做出来的幻灯片应该是[这样][4]的,这是keydown的版本问题,也是我搞了很久没有搞定的原因,你一定很纳闷为什么keydown升级以后比升级以前丑那么多,那为什么还要升级呢？我也很纳闷。

你如果希望做出来的幻灯片跟我的一样,而不是默认这样,只需要使用keydown 0.7.0版本,默认安装的是最新版,现在最新版是0.9.2,安装旧版的方法：

    sudo gem install keydown -v 0.7.0

安装完成以后,使用方法真是让人蛋疼的厉害:

    keydown _0.7.0_ slides slides.md

要是每次都让我输入`_0.7.0_`,我一定想杀人,所以我在`.zshrc`里加入了下面这句话
    
    alias keydown='keydown _0.7.0_'

这样,用HTML5制作幻灯片的过程就交代清楚了,至于写slides.md文件,就照葫芦画瓢吧,相信你能看懂的。

作为一个热心的人,我还是再废话两句。幻灯片都是以`!SLIDE`开始,`！SLIDE`后面跟的内容是css格式,如middle代表居中,dark 代表当前幻灯片背景是暗色,所以字体用白色显示,bulleted 说明当前幻灯片中的内容是项目列表,幻灯片最后用`}}} images/background.jpg`来指定背图案。

完。


[1]: http://yihui.name/slides/2011-r-dev-lessons.html#slide1
[2]: http://mingxinglai.com/cn/slides/instruction-of-keydown.html#slide1
[3]: http://mingxinglai.com/cn/slides/instruction-of-keydown.md
[4]: http://infews.github.com/keydown/
