---
layout: post
title: "几款写python的vim插件"
description: ""
category: 数据库
tags: [vim, linux]
---

本文推荐几款写python的vim插件，这些插件能够显著地提升工作效率，保证第一次接触的人会感到特别惊喜。

我个人之前用vim比较多，但是写python比较少，刚开始用vim写python的时候，感觉很不方便，于是就用了图形界面的[Pycharm][1](如果是windows用户，这个IDE应该是标配)，用了一段时间也没有觉得有什么不好，不过，脑子里总是会想起《[程序员修炼之道][2]》中，那句"最好是精通一种编辑器，并将其用于所有编辑任务。如果不坚持使用一种编辑器，可能会面临现代的巴别特大混乱。"

所以，*我又决定用vim来写python*。

在知乎上有很多[vim写python的插件][3]推荐，不过我感觉很多人回答都不动脑子，这些回答主要有两个问题：

1. 回答不全面，很多人都只是推荐一款插件，然后贴个链接就完了，如果要想很爽的用vim写python，可以肯定的是，一款插件是不够的
2. 贴上自己的配置，说自己的配置有多爽，然后就不管了。如果是一个vim新手，是用不着这么多配置的，如果是一个vim老手，也不会放弃自己原有的配置，使用别人自定义、认为很爽的个性化配置

所以，这些回答都不是很靠谱。本文争取推荐一些靠谱的东西，那么，就开始吧！

# 1. 一键执行

这个不是插件，而是一个自定义的vim配置。很多时候我们写python，都是写一些较为简单的脚本，那么，这个一键执行的功能就非常实用，我个人感觉最实用的是在写单元测试的时候，写完一个单元测试，都不用退出vim，立即执行就能看到结果。

将下面的配置放到.vimrc文件即可：


        """"""""""""""""""""""
        "Quickly Run
        """"""""""""""""""""""
        map <F5> :call CompileRunGcc()<CR>
        func! CompileRunGcc()
            exec "w"
            if &filetype == 'c'
                exec "!g++ % -o %<"
                exec "!time ./%<"
            elseif &filetype == 'cpp'
                exec "!g++ % -o %<"
                exec "!time ./%<"
            elseif &filetype == 'java'
                exec "!javac %"
                exec "!time java %<"
            elseif &filetype == 'sh'
                :!time bash %
            elseif &filetype == 'python'
                exec "!time python2.7 %"
            elseif &filetype == 'html'
                exec "!firefox % &"
            elseif &filetype == 'go'
        "        exec "!go build %<"
                exec "!time go run %"
            elseif &filetype == 'mkd'
                exec "!~/.vim/markdown.pl % > %.html &"
                exec "!firefox %.html &"
            endif
        endfunc



# 2. 代码补全(snipMate)

代码补全能够显著地减少你敲键的次数，并且将你从琐碎的语法中解放出来。说白了，就是帮你写代码！

如果使用snipMate插件，那么，当你输入`ifmain`以后按tab键，将会自动为你生成下面的代码：


        if __name__ == '__main__':
            main()

输入`for`，再按tab键，生成如下代码：


        for needle in haystack:
            # code...

更直观的演示如下所示(xptemplate)：

![image](/cn/image/py-class-def.gif "xptemplate")

代码补全有两款插件都比较不错，可以根据自己的需要进行选择：

1. [snipMate][4]
2. [xptemplate][5]

# 3. 语法检查(Syntastic)

[syntastic][6]是一款强大的语法检查插件，当你保存源文件时，它就会执行，并提示用户哪些代码存在语法错误，哪些代码风格不符合规范，并给出具体的提示。

例如，python代码风格默认设置为PEP8,即使你不知道PEP8的风格，只要你使用syntastic插件，并根据它给出的提示修改，那么，你就能写出完全符合PEP8风格的代码！


# 4. 编程提示(jedi-vim)

[jedi-vim][7]是基于jedi的自动补全插件，与Syntastic
不同的是，该插件更加智能，更贴切的称呼是"编程提示"，而不是代码补全插件。

如下图所示：


![image](/cn/image/jedi-vim.png "jedi-vim")

可以说，这个插件是写vim的标配，并且，真正让vim写python变成一件轻松愉快的事情。

*注意：* 安装惊jedi-vim插件，需要在电脑中安装jedi，根据jedi-vim给出的提示，正常按装即可。不过我之前遇到一个问题，在公司的虚拟机里面，安装以后不起作用，google了半天没有成功，最后更新了一下vim就可以了，希望遇到相同问题的人，能够看到。


        sudo aptitude install vim-gnome vim vim-common vim-tiny


完。

[1]: https://www.jetbrains.com/pycharm/
[2]: http://book.douban.com/subject/1152111/
[3]: http://www.zhihu.com/question/19655689
[4]: https://github.com/garbas/vim-snipmate
[5]: https://github.com/drmingdrmer/xptemplate
[6]: https://github.com/scrooloose/syntastic
[7]: https://github.com/davidhalter/jedi-vim
