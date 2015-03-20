---
layout: post
title: "Rust：一门值得关注的新语言"
description: ""
category: 编程语言
tags: [Rust]
---
# Rust语言简介

### 前言

我个人并不是一个对编程语言有特别偏好的人，对各种酷炫的新概念也不是很感冒。而且，我一直认为，语言仅仅是一个工具，每一种语言都有自己的适用场景，软件背后的思想、理论和数据结构才是最重要的。所以，我了解的语言并不多。不过，今天要介绍的Rust语言算是个特例，这个语言还不是特别成熟，不过目前来看，发展得都很好，定位也很明确，我个人非常喜欢，所以在这里介绍给周围的人。

Rust是一门系统编程语言，它的目标是取代C/C++，这一点上，它与Golang相似，不过它比Golang更“大”。所谓更大，就是它的概念和知识点更多，需要花较大的力气才能学会（类似于C++）。

在详细介绍Rust语言之前，先来看一下它的历史：

1. 2006年，Rust作为Graydon Hoare的个人项目首次出现；
1. 2009年，Graydon Hoare成为Mozilla雇员；
1. 2010年，Rust首次作为Mozilla官方项目出现。同年，Rust开始从初始编译（由OCaml写成）转变为自编译；
1. 2011年，Rust成功的完成了移植。Rust的自编译器采用LLVM作为其编译后端；
1. 2012年1月20日，第一个有版本号的预览版Rust编译器发布。当前最新版本为0.12，于2014年10月发布；
1. 2013年4月4日，Mozilla基金会宣布将与三星集团合作开发浏览器排版引擎Servo将以Rust来实现；

从发明时间可以看到，Rust相对于C++/Java，是一门非常新的语言。

Rust对自己的定位是和C/C++类似的系统编程语言，由于C和C++都非常容易出现内存错误（如：segmentation faults ），并由此引发的一系列相关的问题，而Rust的出现就是用来避免类似问题的发生。可以说，Rust拥有了更高的开发效率和安全性，同时拥有和C/C++一样的性能。那么，Rust是如何在保证系统的安全性的同时，又能够保证程序的效率的呢？

Rust是把安全性检查提前到了编译阶段，对于Rust编译器，当遇到灰色情况它会拒绝编译，直至你搞清楚它，当然，这对程序员来说是一种束缚，势必会牺牲一点开发的效率，不过这都是为了安全性考虑。

### 示例

在介绍一门新语言的时候，第一个介绍的总是hello world程序，下面这段代码就是Rust中的Hello，world：

    fn main() {
        println!("Hello, world!");
    }

cargo是Rust的辅助工具，用于生成Project、编译和运行Rust代码，通过Cargo编译运行：

    $ cargo run
       Compiling hello_world v0.0.1 (file:///Users/you/src/hello_world)
         Running `target/hello_world`
    Hello, world!

下面再来看一个例子，用以对Rust的特性有一个感性的认识，先看一段C++代码：

    #include<iostream>
    #include<vector>
    #include<string>

    int main() {
        std::vector<std::string> v;
        v.push_back("Hello");
        std::string& x = v[0];
        v.push_back("world");
        std::cout << x;
    }

这段C++代码编译可以通过，但是运行是会出现问题：

    $ g++ hello.cpp -Wall -Werror
    $ ./a.out
    Segmentation fault (core dumped)

类似的代码，在Rust中则不可能出现，因为，Rust在编译阶段就会发现问题：

    fn main() {
        let mut v = vec![];
        v.push("Hello");
        let x = &v[0];
        v.push("world");
        println!("{}", x);
    }

Rust会拒绝编译，并给出清楚明了的提示信息：

    $ cargo run
       Compiling hello_world v0.0.1 (file:///Users/you/src/hello_world)
    main.rs:8:5: 8:6 error: cannot borrow `v` as mutable because it is also borrowed as immutable
    main.rs:8     v.push("world");
                  ^
    main.rs:6:14: 6:15 note: previous borrow of `v` occurs here; the immutable borrow prevents subsequent moves or mutable borrows of `v` until the borrow ends
    main.rs:6     let x = &v[0];
                           ^
    main.rs:11:2: 11:2 note: previous borrow ends here
    main.rs:1 fn main() {
    ...
    main.rs:11 }
               ^
    error: aborting due to previous error

上面这段代码很好的解释了，为什么Rust能够兼顾安全性和性能。

### 特性

Rust的语法跟C比较相似，不过它是一个更现代的语言。Rust是针对多核体系提出的语言，并且吸收一些其他动态语言的重要特性，比如不需要管理内存，比如不会出现Null指针等等。下面是Rust的一些特性：

* 模式匹配和代数型的数据类型（枚举）
* 基于任务的并发性，轻量级的任务都可以在不共享内存的情况下并发运行（threads without data races  ）
* 高阶函数（闭包）
* 多态性，结合类似Java的接口特性和Haskell的类类型。
* 泛型
* 变量默认情况下不可变（const）
* 非阻塞的垃圾收集器
* 零成本抽象保证内存安全guaranteed memory safety
* 模式匹配pattern matching
* 类型推导type inference
* 极小运行环境minimal runtime
* 高效C绑定efficient C bindings

这里只是给大家引荐一门新语言，让大家对Rust有一个初步的概念，如果对Rust感兴趣，可以看它官网的《[30分钟入门教程](http://doc.rust-lang.org/nightly/intro.html)》
