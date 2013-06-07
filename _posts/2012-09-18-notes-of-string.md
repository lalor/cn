---
layout: post
title: "String 字符串"
description: ""
category: 程序语言
tags: [string]
---

以前学习通过《[C++ Primer][1]》学习string时，总觉得它的成员函数太多，而且它还可以使用标准库的泛型函数，总觉得函数太多记不住，现在系统的学完STL以后，发现其实string提供的函数还不够多，比如不支持忽略大小写的字符串比较，不支持正则表达式子，还有我觉得没有awk语言中定义FS作用域,然后取出个字段这个功能，我很喜欢awk的。

关于string的用法，我就不班门弄斧了，况且网上这么多，在此，摘录几个注意事项，算是自己学习STL的学习笔记。首先列出string的所有成员函数，关于各函数的用法，可以参考[这里][2]


#Iterators &  Member functions

    //Construct string object 
    constructor
        //String assignment 
    operator= 
        //Return iterator to beginning 
    begin 
        //Return iterator to end 
    end 
        //Return reverse iterator to reverse beginning 
    rbegin 
        //Return reverse iterator to reverse end 
    rend 


#Capacity:

    //Return length of string 
    size 
        //Return length of string 
    length 
        //Return maximum size of string 
    max_size 
        //Resize string 
    resize 
        //Return size of allocated storage 
    capacity 
        //Request a change in capacity 
    reserve 
        //Clear string 
    clear 
        //Test if string is empty 
    empty 


#Element access

    //Get character in string 
    operator[] 
    //Get character in string 
    at() 


#Modifiers:

        //Append to string 
    operator+= 
        //Append to string 
    append 
        //Append character to string 
    push_back 
        //Assign content to string 
    assign 
        //Insert into string 
    insert 
        //Erase characters from string 
    erase 
        //Replace part of string 
    replace 
        //Swap contents with another string 
    swap 


#String operations:

        //Get C string equivalent 
    c_str 
        //Get string data 
    data 
        //Get allocator 
    get_allocator 
        //Copy sequence of characters from string 
    copy 
        //Find content in string 
    find 
        //Find last occurrence of content in string 
    rfind 
        //Find character in string 
    find_first_of 
        //Find character in string from the end 
    find_last_of 
        //Find absence of character in string
    find_first_not_of 
        //Find absence of character in string from the end 
    find_last_not_of 
        //Generate substring 
    substr 
        //Compare strings 
    compare 


#string::npos

string提供了各种形式的find函数，当我们使用find时，如果搜寻失败（即没有找到目标字符/字符串），必须返回一个特殊的值表示搜寻失败，即string::npos ,如验证搜寻动作是否失败：

    string::size_type idx=filename.find(".");
    if(idx == string::npos)
    {
        doSomeThings();
    }


需要注意的是，当打算验证搜寻函数的返回值时，应该使用string::size_type而不是int或unsigned int，否则上述与string::npos的比较动作将无法有效运行。那么我们不禁要问，这个string::size_type到底是什么类型？


#string::size_type

按照《[C++ 标准库][3]》的原文：string::size_type是string class定义的一个无正负号的整型型别。

我们来做个小试验：

    #include <iostream>
    #include <string>
    #include <vector>
    #include <limits>
    using namespace std;
    
    int main(int argc, char* argv[])
    {
        vector<char> v;
        cout << "string::npos             = "
            << string::npos  << endl;
        cout << "max(int)                 = "
            << numeric_limits<int>::max() << endl;
        cout << "max(unsigned int)        = "
            << numeric_limits<unsigned int>::max() << endl;
        cout << "vector<char>::max_size() = "
            << v.max_size() << endl;
        return 0;
    }


输出结果如下：

    string::npos             = 4294967295
    max(int)                 = 2147483647
    max(unsigned int)        = 4294967295
    vector<char>::max_size() = 4294967295


按照上面的结果，可能stirng::size_type是unsigned int,但是我们还是不能这么认为，因为标准库没这么说，可能巧好我使用的STL版本里是unsigned int。

下面是我找到的比较靠谱的[解释][4]:

虽然我们不知道string::size_type的确切类型，但可以知道它是unsigned型。对于任意一种给定的数据类型，它的unsigned型所能表示的最大正数值比对应的signed型要大一倍。这个事实表明size_type存储的string长度是int所能存储的两倍。使用int变量的另一个问题是，有些机器上int变量的表示范围太小，甚至无法存储实际并不长的string对象。如在有16位int型的机器上，int类型变量最大只能表示32767个字符的string对象。而能容纳一个文件内容的string对象轻易就会超过这个数字。因此，为了避免溢出，保存一个stirng对象size的最安全的方法就是使用标准库类型string::size_type。


#string与C-string

我们可以轻易的使用char型数组够造string
型变量，但是反之却没那么容易，有很多注意事项。

第一点需要注意的是，你不能以单一字符来初始化某个字符串，但是你可以这么做：

    std::string s('x');    //ERROR
    std::string s(1, 'x');    //OK


第二点需要注意的是有三个函数可以将字符传内容转换为字符型数组或C-string
- data() 不在末尾添加 '\0'
- c_str() 在末尾添加 '\0'
- copy() 不在末尾添加 '\0'

**注意：**data()和c_str()返回的字符数组由该字符串拥有。也就是说调用者千万不可修改它或释放其内存。

由于data()和copy()都不在末尾添加'\0'，而且c_str()和data()在string
变量变动以后就失效，所以这几个函数，没一个是免费的午餐。

先看copy()，我表示很不理解，为什么copy()不在末尾添加'\0'，当我们使用copy时需预留一个空间，并自己显示添加'\0'，如果你忘了，写出了如下代码，噩梦就开始了。

    string s("hello, world");
    char buffer[5];
    s.copy(buffer, 5);
    print(buffer); //call function 


《[C++ 标准库][3]》里友情提示：一般而言，整个程序中，你应该坚持使用strings
直到你必须将其内容转化为char\*时才把他们转换为C-string 。请注意，c_str() 和
data() 的返回值有效期限在下一次调用non-const 成员函数时即告终止，如下所示：

    #include <iostream>
    #include <string>
    using namespace std;
    
    void print(const char *p)
    {
        while (p != NULL) 
        {
            cout << *p++;
        }
        cout << endl;
    }
    
    int main(int argc, char* argv[])
    {
        string s("hello, world");
        const char *p;
        p = s.c_str(); // p refers to the contents of s as a c-string p
        print(p); //OK, p is still valid
    
        s+="!"; //invalidates p
        print(p); //Error: argument p is not valid
    }


#string 的compare()和assign()成员函数

我第一次学习string时，觉得这两个函数就是多余，因为我们可以直接使用大于，小于符号来比较字符串，也可以直接使用等于号或者构造函数来赋值，为什么还需要compare()和assign()呢，答案很简单，只是为了更加灵活的进行比较和赋值。我们确实可以使用大于符号，小于符号来进行字符串比较，但是这样进行的比较操作都是两个string变量的比较，如果我们只需要比较两个字符串的其中一部分呢，这个时候大于小于符号就没辄了。同样，assign()也是为了更加灵活的赋值，比如我们要令s为100个x,使用assign()我们就可以这样：

    string s;
    s.assing(100, 'x');


如果不能使用assgin,那要我们输入100次x或者写一个循环么?下面再举两个例子：

    std::string s("abcd");

    s.compare("abcd"); // return 0
    s.compare("dcba"); // less
    s.compare("ab");   // greater

    s.compare(s);
    s.compare(0,2, s, 2, 2); // "ab" is less than "cd"
    s.compare(1,2,"bcx",2);  // return 0

    const std::string aString("other");
    std::string s;

    s = aString;
    s = "two\nlines";
    s = ' ';

    s.assign(aString);
    s.assign(aString, 1, 3);
    s.assign(aString, 2, std::string::npos);

    s.assign("nico", 5);
    s.assign(5, 'x');


#把所有字符串转换为大写

因为string提供和vector一样的随机迭代器，所以我们可以方便的使用各种泛型算法，例如，把字符串转化为大写：

    #include <iostream>
    #include <algorithm>
    #include <string>
    using namespace std;
    
    int toUpper(char ch)
    {
        if (ch > 'a' ) 
            ch -= 32;
        return ch;
    }
    
    int main(int argc, char* argv[])
    {
        string s("hello, world");
    
        transform(s.begin(), s.end(), // source
                s.begin(),            // destination
                toUpper);             // operation
    
        cout << s << endl;
    }


#忽略大小写比较字符串

跟上面一样，我们可以使用标准库的泛型算法：

    #include <iostream>
    #include <algorithm>
    #include <string>
    using namespace std;
    
    bool nocase_compare(char c1, char c2)
    {
        return (toupper(c1) == toupper(c2));
    }
    
    int main(int argc, char* argv[])
    {
        string s1("hello, world");
        string s2("HELLO, WORLD");
    
        if (s1.size() == s2.size() &&
               equal(s1.begin(), s1.end(),
                   s2.begin(),
                   nocase_compare)) 
        {
            cout <<" these string are equal" << endl;
        }
    
    }



貌似还有更牛叉的算法:

    compose_f_gx_hy(equal_to<int>(),
            ptr_fun(toupper),
            ptr_fun(toupper))


#用empty来代替检查size()是否为0

用empty来代替检查size()是否为0,这是《[Effective STL][5]》item 4,主要原因见下面的说明，但是为何《[C++标准库][3]》里说，最好使用empty()来判空，这样可能效率更高一点，这我就不得而知了，以后知道了再来补充。

对于任意容器c，写下

    if (c.size() == 0)…


本质上等价于写下

    if (c.empty())…


但是为什么第一种方式比第二种优呢？理由很简单：对于所有的标准容器，empty是一个常数时间的操作，但对于一些list实现，size花费线性时间。
这什么造成list这么麻烦？为什么不能也提供一个常数时间的size？如果size是一个常数时间操作，当进行增加/删除操作时每个list成员函数必须更新list的大小，也包括了splice，这会造成splice的效率降低（现在的splice是常量级的），反之，如果splice不必修改list大小，那么它就是常量级地，而size则变为线性复杂度，因此，设计者需要权衡这两个操作的算法：一个或者另一个可以是常数时间操作。完。

[1]: http://www.amazon.cn/C-Primer%E4%B8%AD%E6%96%87%E7%89%88-%E6%9D%8E%E6%99%AE%E6%9B%BC/dp/B00114DX08/ref=sr_1_1?ie=UTF8&qid=1347974690&sr=8-1
[2]: http://www.cplusplus.com/
[3]: http://www.amazon.cn/C-%E6%A0%87%E5%87%86%E7%A8%8B%E5%BA%8F%E5%BA%93-%E8%87%AA%E4%BF%AE%E6%95%99%E7%A8%8B%E4%B8%8E%E5%8F%82%E8%80%83%E6%89%8B%E5%86%8C-Nicolai-M-Josuttis/dp/B0011BDOM8 
[4]: http://www.cppblog.com/lantionzy/archive/2009/10/22/99164.html
[5]: http://www.amazon.cn/Effective-STL-50-Specific-Ways-to-Improve-the-Use-of-the-Standard-Template-Library-Meyers-Scott/dp/0201749629/ref=sr_1_3?ie=UTF8&qid=1348029443&sr=8-3
