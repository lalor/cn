---
layout: post
title: "使用Git管理自己的dotfiles"
description: ""
category: 工具
tags: [git, dotfiles]
---

面对linux下如此多的配置文件，以及vim下各种各样的插件，备份可谓是一大难题，尤其是你同时使用多台电脑，需要在不同电脑中进行相同的配置，这种琐碎而无聊的工作不光浪费时间，而且还容易出错，之前为了备份vim的插件，我坚持不泄的寻找简单又高效的方法，偶然间被我知道dotfiles，这篇博客转载自[blogspot][1]，转载的原因有两个，一是作者已经讲得清楚明了，暂时没有发现有什么需要补充的，二是blogspot被墙了，作为一个有道德有原则的IT男，我已著名文章出处。文中有个别错误，我已改正。

---

什麼是 dotfile 呢？dotfile 就是你的那些 dot 開頭的檔案，譬如說 `.vim` `.vimrc`
`.emacs.d` `.bashrc`

其實作法很簡單，先建立一個資料夾，初始化 Git repository，建立一個 Makefile
來自動產生 link。

    mkdir ~/dotfiles
    cd ~/dotfiles
    mv ~/.vim  vim
    mv ~/.vimrc vimrc
    git init
    git add vim
    git add vimrc


然后攥写你的Makefile:

    init:
            ln -fs `pwd`/vim    ~/.vim
            ln -fs `pwd`/vimrc   ~/.vimrc
        
    sync:
            git pull
            git push

記得建立 .gitignore 來忽略你不想納入版本控制的檔案: .gitignore


然後現在你有自己的 Git Repository ，你可以把它 push 到任何其他的 remote (參見 git help remote 命令)

到了別的機器上頭，你只需要:

    git clone git@host:/path/to/repo
    cd repo
    make init 

即可初始化完成。

所以我寫好了這樣一段的 script 來自動建置環境(有小错误，我已更改):

#!/bin/bash
    mkdir ~/git-dotfile
    cd ~/git-dotfile
    git init
    mv ~/.vim  vim
    mv ~/.vimrc vimrc
    
    git add vim  vimrc
    echo "init:" >> Makefile
    echo -e "\t\tln -fs `pwd`/vim ~/.vim" >> Makefile
    echo -e "\t\tln -fs `pwd`/vimrc ~/.vimrc" >> Makefile
    
    # initialize your dotfile env
    make init
    
    # do github-import ? :p


在我电脑上mv
目录老出错，加上`pwd`后就好了。下面是我的备份脚本，以后只需要在另外一台电脑上`git clone`下来，然后`make init`就行了，非常方便。


    #!/bin/bash
    mkdir ~/.git-dotfile
    cd ~/.git-dotfile
    
    git init
    
    mv   ~/.vim        `pwd`/vim
    mv   ~/.vimrc      `pwd`/vimrc
    mv   ~/.tmux.conf  `pwd`/tmux.conf
    mv   ~/.profile    `pwd`/profile
    mv   ~/.bashrc     `pwd`/bashrc
    
    echo "init:" >> Makefile
    echo -e "\t\tln -fs `pwd`/vim ~/.vim" >> Makefile
    echo -e "\t\tln -fs `pwd`/vimrc ~/.vimrc" >> Makefile
    echo -e "\t\tln -fs `pwd`/tmux.conf ~/.tmux.conf" >> Makefile
    echo -e "\t\tln -fs `pwd`/profile ~/.profile" >> Makefile
    echo -e "\t\tln -fs `pwd`/bashrc ~/.bashrc" >> Makefile
    
    # initialize your dotfile env
    make init
    # do github-import ? :p


[1]: http://c9s.blogspot.com/2009/11/git-dotfiles.html
