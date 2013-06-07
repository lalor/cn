---
layout: post
title: "函数对象"
description: "STL学习笔记，函数对象的使用"
category: 程序语言
tags: [function object, functor]
---

本文乃作者学习《[C++标准程序库][1]》的学习笔记，首先介绍了仿函数（函数对象）和函数适配器（配接器）的概念，然后列出STL中所有的仿函数，以及函数适配器，并摘录了几个例子演示仿函数和函数适配器的用法，最后讨论了仿函数的组合，以及实现方法。


#1.仿函数是什么东西？

《[C++标准程序库][1]》里对仿函数的解释是：仿函数是泛型编程强大威力和纯粹抽象概念的又一例证。你可以说，任何东西，只要其行为像函数，它就是一个函数。因此如果你定义了一个对象，行为像函数，它就可以被当做函数来用。

那么，什么才算具备函数行为呢？所谓函数行为，是指可以"使用小括号传递参数，籍以调用某个东西"。例如：

    function(argc1, argc2);


如果你指望对象也可以如此这般，就必须让它们也可以被"调用"——通过小括号的运用和参数的传递。你只需要定义operator()，并给予合适的参数型别：

    class X 
    {
        public:
        // define 'function call' operator
        return-value operator()(arguments) const;
        ......
    };

现在，你可以把这个类别的对象当做函数来调用了：

    X fo;
    ......
    // call operator () for function object to
    fo(argc1, argc2);


上述调用等价于：

    fo.operator() (argc1, argc2); 


**总结如下:**

* 函数对象是一个普通对象
* 重载了operator ()操作符
* 函数对象一般都比较简单，主要用到operator()操作，其他成员函数和成员变量都是为operator()服务

为了帮助理解仿函数，我们来看一个[例子][2]：


    #include <algorithm>
    class gen_by_two {
    public:
        gen_by_two( int seed = 0 ) : _seed( seed ){}
        int operator()() { return _seed += 2; }
    private:
        int _seed;
    };
    vector<int> ivec( 10 );
    
    // fills ivec: 102 104 106 108 110 112 114 116 118 120
    generate_n( ivec.begin(), ivec.size(), gen_by_two(100) );


上面的程序使用generate_n函数，给容器赋值，在调用gen_by_two(100)的时候，我们其实是生成了一个对象，并调用对象的构造函数给成员变量赋值，然后多次（v.size())调用该变量的operator()成员函数，将返回的值依次存入容器中。


#2.函数适配器又是什么东西？

函数适配器又称"函数配接器"，是只能够将仿函数和另一个仿函数（或某个值，或某一个函数）结合起来的仿函数。函数适配器声明于`<functional>`中。

下面来看一个例子：

    find_if(coll.begin(), coll.end(), //range
            bind2nd(greater<int>(), 42));criterion

其中表达式`bind2nd(greater<int>(),42)`导致一个组合型的仿函，检查某个int值是否大于42.实际上bind2nd是将一个二元仿函数转换为一个一元仿函数。bind2nd的意思就是将42作为比较`(greater<int>())`函数的第二个参数，也就相当于是`elem.value > 42`，如果容器中没有42这个值，那么下面这条语句和上面的语句是一样的。

    find_if(coll.begin(), coll.end(), //range
            bind1st(less<int>(), 42));criterion


上面两个小例子演示了适配器的功能，同时还讲解了bind1st和bind2nd的区别。

#3.预定义的仿函数

<a href="http://imgur.com/aRK5q"><img src="http://i.imgur.com/aRK5q.png" title="Hosted by imgur.com" alt="" /></a>

#4.预定义的函数适配器

<a href="http://imgur.com/zo1So"><img src="http://i.imgur.com/zo1So.png" title="Hosted by imgur.com" alt="" /></a>
<a href="http://imgur.com/P8cjv"><img src="http://i.imgur.com/P8cjv.png" title="Hosted by imgur.com" alt="" /></a>
<a href="http://imgur.com/bBRs7"><img src="http://i.imgur.com/bBRs7.png" title="Hosted by imgur.com" alt="" /></a>


#5.仿函数(以及函数适配器)的使用示例

下面摘录几个《[C++标准程序库][1]》上的例子，以及个别我自己补充的实例，用于演示仿函数的使用，希望能够通过例子快速掌握仿函数。

###5.1 查找25或者35第一次出现的位置

    pos = find_if(coll.begin(), coll.end(), //range
            compose_f_gx_hx(logical_or<bool>(), //criterion
                bind2nd(equal_to<int>(), 25),
                bind2nd(equal_to<int>(), 35)));


###5.2 将集合中全部元素都设为相反值

    transform(coll.begin(), coll.end(),//source
            coll.begin(),//destination
            negate<int>());//operation


###5.3 对集合中的所有元素求平方

    transform(coll.begin(), coll.end(),//first source
            coll.begin(),//second source
            coll.begin(),//destination
            multiplies<int>());//operation


###5.4 所有元素乘以10

    transform(coll.begin(), coll.end(),//source
            back_inserter(coll2),//destination
            bind2nd(multiplies<int>(),10));//operation


###5.5 将a替换为b

    replace_if(coll2.begin(), coll2.end(),//range
            bind2nd(equal_to<int>(), 70),//replace criterion
            42);//new value


###5.6 删除小于50的元素

    coll.erase(remove_if(coll.begin(), coll.end(),//range
                bind2nd(less<int>(), 50)), //remove criterion
            coll.end());  


###5.7 返回第一个偶数

    pos = find_if(coll.begin(), coll.end(), //range
            not1(bind2nd(modulus<int>(), 2)));


上面几个例子都还是挺使用的，返回第一偶数么，怎么看怎么奇怪，没有关系，我们来看一个不奇怪的：

###5.8 调整数组顺序，使得奇数位于偶数前面

    stable_partition(v.begin(), v.end(), bind2nd(modulus<int>(), 2));


关于这里用到的泛型算法，如果还有不熟悉的，可以参考[这里][3]


###5.9 mem_fun_ref 与mem_fun 的区别以及用法

- mem_fun_ref:调用某个对象的成员函数
- mem_fun:功能和mem_fun_ref 一样，如果容器里存放的是指向对象的指针，而不是对象，则应该使用mem_fun

下面来看一个使用mem_fun_ref
的例子，假设我们定义了一个类Person,并且定义了一个print
的成员函数，容器coll 里存放了coll.size() 个Person
对象，现在要调用每个对象的print 成员函数，那么我们就可以像下面这样。


    class Person 
    {
    public:
        ...
            void print() const
            {
                std::cout << name << std::endl;
            }
    
        void printWithPrefix(std::string prefix) const
        {
            std::cout << prefix << name << std::endl;
        }
    private:
        std::string name;
    };
    
    
    void foo(const std::vector<Person> &coll)
    {
    
        for_each(coll.begin(), coll.end(), 
                mem_fun_ref(&Person::print));
    }


我们不能直接把一个成员函数传递给一个算法，所以这里必须运用函数适配器，下面这种做法会导致编译错误。

    for_each(coll.begin(), coll.end(),
            &Person::print); //ERROR


通过使用函数适配器，我们还可以像被调用的成员函数传递一个参数。如下所示：

        //call member function printWithPrefix() for each element
        for_each(coll.begin(), coll.end(), 
                bind2nd(mem_fun_ref(&Person::printWithPrefix),
                    "person:"));


###5.10 ptr_fun

ptr_fun 使得我们能够在其他函数适配器中使用一般函数，加入你自己定义了一个函数check()，用于检验容器中的中的元素是否符合某种条件，你就可以这样：

    pos = find_if(coll.begin(), coll.end(),
            not1(ptr_fun(check)));


这里不能使用not1(check)，因为not1()需要用到由仿函数提供的某些特殊型别.

第二种用法是，当你有一个双参数的全局函数，又想把它当做一个单参数函数来使用，可以用如下语句：

    //find first string that is not empty
    pos = find_if(coll.begin(), coll.end(), 
            bind2nd(ptr_fun(strcmp),""));


#6.让自定义的仿函数也可以使用函数适配器

你可以编写自己的仿函数，但如果希望它们能够和函数适配器搭配运用，就必须满足某些条件：必须提供一些型别成员来反映其参数和返回值的型别。为了方便我们，C++标准库提供了一些结构如下：

待补充


#7.待补充

仿函数的组合。等我学习了boost 再来补充吧。

[1]: http://www.amazon.cn/C-%E6%A0%87%E5%87%86%E7%A8%8B%E5%BA%8F%E5%BA%93-%E8%87%AA%E4%BF%AE%E6%95%99%E7%A8%8B%E4%B8%8E%E5%8F%82%E8%80%83%E6%89%8B%E5%86%8C-Nicolai-M-Josuttis/dp/B0011BDOM8
[2]: http://mingxinglai.com/cn/2012/09/generic-algorithm-handbook/#generate_n
[3]: http://mingxinglai.com/cn/2012/09/generic-algorithm-handbook
