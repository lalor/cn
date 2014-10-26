---
layout: post
title: "常见的设计模式"
description: ""
category: 程序语言
tags: [设计模式]
---

设计模式是程序设计中较难的主题，也是大学课程里较少涉及的知识。但是，却是比较重要的知识，
是成为合格的程序员必须掌握的技能。如果没有在具体的项目中用到，
像学习其他知识一样学习设计模式，必然会事半功倍。因此，
本文只介绍最常见和最简单的设计模式。
本文中介绍的设计模式来自于《C++编程思想 第2卷实用编程技术》
第10章设计模式以及4人帮的设计模式中读者指南里提到的几种最简单常用的模式。

虽然介绍的模式不够全面，也都是一些相对比较简单的设计模式，不过我相信，对于绝大多数
程序员来说，已经够用了。

## 1. 设计模式的分类

"四人帮"讨论了23个模式，按照下面3中目的分类：

1. 创建型
    用于创建对象。通常包括隔离对象创建细节，这样代码不依赖于对象是什么类型，因此，在增加一种新的对象类型时不需要修改代码。创建型模式包括单例(Singleton，又翻译为单件)模式、工厂(Factory)模式和构建器(Builder，又翻译为生成器)模式。其中，工厂模式可以细分为抽象工厂(Abstract Factory)和工厂方法(Factory Method)。
2. 结构型
    结构型设计模式影响对象之间的连接方式，确保系统的变化不需要改变对象间的连接。常见的结构型设计模式包括代理(Proxy)模式、适配器(Adapter，Wrapper又称封装器)模式、桥接模式(Bridge)、组成(Composite)模式和装饰器(Decorator)模式。
3. 行为型
    行为型设计模式在程序中处理对象的操作（即行为），比如解释一种语言、实践一个请求、遍历一个序列或者实现一个算法。行为型模式包括命令模式(Command)、模板方法(Template Method)模式、状态(State)模式、策略(Strategy)模式、职责链(Chain of Responsibility)模式、观察者(Observer)模式、多派遣(Multiple Dispatching)模式和访问者(Visitor)模式。

<!--此外，本文还介绍了广泛使用的Reactor模式。-->

需要再次强调，理解模式的分类是很重要的，能够有效地提升对设计模式的理解。其中，创建型模式用于创建对象，结构型模式影响对象之间的连接方式，而行为型设计模式处理对象的操作。

## 2 创建型设计模式

####2.1 单例(Singleton)模式

它允许一个类有且仅有一个实例

    #include <iostream>
    using namespace std;

    class Singleton{
        private:
        static Singleton s;
        int i;
        Singleton(int x):i(x){}
        Singleton& operator =( Singleton &);
        Singleton(const Singleton&);

        public:
        static Singleton& instance(){ return s;}
        int getV(){ return i; }
        void setV(int x){ i = x; }
    };

    Singleton Singleton::s(47);

    int main(int argc, char* argv[])
    {
        Singleton &s = Singleton::instance();

        cout << s.getV() << endl;
        s.setV(5);
        cout << s.getV() << endl;
        return 0;
    }


####2.2 抽象工厂模式

工厂模式用以解决创建对象的问题，即不允许将创建对象的代码散布于整个系统。
工厂模式可能是所有设计模式中最有用的模式之一。

有两种实现工厂模式的方法，第一种方法是在基类中定义一个静态成员函数，
所有的对象都通过这个静态成员函数创建。由于使用静态成员函数实现工厂模式比较简单，
因此，它比多态工厂更为常见。

    #include <iostream>
    #include <string>
    #include <vector>
    using namespace std;

    class Shape
    {
        public:
            virtual void draw() = 0;
            virtual void erase() = 0;
            virtual ~Shape(){}
            static Shape* factory(const string& type);
    };


    class Circle: public Shape
    {
        Circle(){}
        friend class Shape;
        public:
        void draw(){ cout << "Circle::draw()" << endl; }
        void erase(){ cout << "Circle::erase()" << endl; }
        ~Circle(){ cout << "Circle::~Circle" << endl; }
    };

    class Square: public Shape
    {
        Square(){}
        friend class Shape;
        public:
        void draw(){ cout << "Square::draw()" << endl; }
        void erase(){ cout << "Square::erase()" << endl; }
        ~Square(){ cout << "Square::~Square" << endl; }
    };


    Shape* Shape::factory(const string &type)
    {
        if (type == "Circle"){cout << "create Circle" << endl; return new Circle;}
        if (type == "Square"){cout << "create square" << endl; return new Square;}
    }

    char* sl[] = {"Circle", "Square", "Square", "Square"};
    int main(int argc, char* argv[])
    {
        vector<Shape*> shapes;
        for (size_t i = 0; i < sizeof sl / sizeof sl[0]; i++)
            shapes.push_back(Shape::factory(sl[i]));

        for (size_t i = 0; i < shapes.size(); i++)
        {
            shapes[i]->draw();
            shapes[i]->erase();
        }
        return 0;
    }


多态工厂方法模式比较复杂，在大多数情况下，都不需要这么复杂的设计，基类中的一个静态成员函数就能很好的完成工作。

<!--有一个Factory类和若干个innerFactory类，innerFactory类位于各个目标类的内部，用以创建目标类的对象。

通过一个FactoryInitializer类定义若干个innerFactory的对象，由Factory类调用各个innerFactory对象穿件目标对象。-->


####2.3 工厂方法模式

工厂方法模式（Factory Method），定义一个用于创建对象的接口，让子类决定实例化哪一个类。
工厂方法使一个类的实例化延迟到其子类。
[这篇文章](http://blog.csdn.net/zhengzhb/article/details/7348707)很好的介绍了工厂方法模式，
非常值得学习。


##3 结构型设计模式

#### 3.1 代理模式

代理类派生自一个基类，由平行地派生自同一个基类的一个或多个类提供实际的实现。

当一个代理对象被创建的时候，一个实现对象就分配给了它，代理对象就将函数调用发给实现对象。

    #include <iostream>
    using namespace std;

    class ProxyBase
    {
        public:
            virtual void f() = 0;
            virtual void g() = 0;
            virtual void h() = 0;
            virtual ~ProxyBase(){}
    };

    class Implementation: public ProxyBase
    {
        public:
            void f(){ cout << "Implementation .f()" << endl; }
            void g(){ cout << "Implementation .g()" << endl; }
            void h(){ cout << "Implementation .h()" << endl; }
    };

    class Proxy: public ProxyBase
    {
        private:
            Implementation *imp;
        public:
            Proxy(){ imp = new Implementation(); }
            ~Proxy(){ delete imp; }
            void f() { imp->f();}
            void g() { imp->g();}
            void h() { imp->h();}
    };

    int main(int argc, char* argv[])
    {
        Proxy p;
        p.f();
        p.g();
        p.h();
        return 0;
    }



#### 3.2 适配器模式


适配器的作用就是将一个类的接口转换成客户希望的另外一个接口，Adapter模式使得原本由于接口不兼容而不能一起工作的那些类可以一起工作。

它的主要作用是在新接口和老接口之间进行适配。

例如，你有一个产生斐波那契数列的生成器，如下所示：

    class FibonacciGenerator
    {
        int n;
        int val[2];
        public:
        FibonacciGenerator():n(0){ val[0] = val[1] = 0; }
        int operator()
        {
            int result = n > 2 ? val[0] + val[1] : n > 0 ? 1 : 0;
            ++n;
            val[0] = val[1];
            val[1] = result;
            return result;
        }
        int count() { return n; }
    };


也许你希望利用这个生成器来执行STL数值算法操作。遗憾的是，STL算法只能使用迭代器才能工作，这就存在接口不匹配的问题。解决方法就是创建一个适配器，它将接受FibonacciGenerator
并产生一个供STL算法使用的迭代器。由于数值算法只要求一个输入迭代器，这个适配器模式相当的直观。

    class FibonacciAdapter
    {
        FibonacciGenerator f;
        int length;
        public:
        FibonacciAdapter(int size):length(size){}
        class iterator;
        friend class iterator;
        class iterator: public std::iterator<std::input_iterator_tag, FibonacciAdapter, ptrdiff_t>
        {
            FibonacciAdapter& ap;
            public:
            typedef int value_type;
            iterator(FibonacciAdapter& a):ap(a){}
            bool operator== (const iterator &)const
            {
                return ap.f.count() == ap.length;
            }
            bool operator != (const iterator &)const
            {
                return !(*this == x);
            }
            int operator*() const { return ap.f(); }
            iterator& operator++() { return *this; }
            iterator operator++(int) { return *this; }
        };
        iterator begin(){ return iterator(*this); }
        iterator end(){ return iterator(*this); }
    }


有了适配器，就可以对这个产生器执行STL算法了，如下所示：

    int main(int argc, char const* argv[])
    {
        const int sz = 20;
        FibonacciAdapter a1(sz);
        cout << "accumulate:" << accumulate(a1.begin(), a1.end(), 0) << endl;
        return 0;
    }


#### 3.3 桥接模式

按照4人帮的说法，桥接模式就是将抽象部分和它的实现部分分离，使它们都可以独立的变化。桥接模式是通过对象组合的方式，把两个角色之间的继承关系改为了耦合的关系，从而使这两者可以从容自若的各自独立的变化。更加详细地介绍见<http://www.cnblogs.com/houleixx/archive/2008/02/23/1078877.html>

#### 3.4 组成模式

在编程中，可能仅仅需要操作单个对象，也可能需要操作包含多个对象的容器，组合模式的作用就是，为单个对象和组合对象提供一致的接口，方便用户使用。<http://blog.csdn.net/hguisu/article/details/7530783>

#### 3.5 装饰器模式

装饰器模式就是在不改变类的实现的情况下，改变类的行为。即，在类的外层再封装一层，与通过继承增加功能不同的是，装饰器可以只对某个对象进行装饰，对其他对象没有任何影响。

更加详细地介绍见<http://www.cnblogs.com/octobershiner/archive/2011/11/04/2236730.html>。


## 4 行为型设计模式

#### 4.1 命令模式

从最直观的角度来看，命令模式就是一个函数对象：一个作为对象的函数。通过将函数封装为对象，就能够以参数的形式将其传递给其他函数或者对象，告诉
它们在履行请求过程中执行特定的操作。

命令模式的一个常见的例子就是在应用程序中“撤销”操作功能的实现。每次在用户进行某项操作的时候，相应的“撤销操作”命令对象就被置入一个队列中。而
每个命令对象被执行完以后，程序的状态就倒退一步。

    #include <iostream>
    #include <vector>
    using namespace std;

    class Command
    {
        public:
            virtual void execute() = 0;
    };

    class Hello:public Command
    {
        public:
            void execute(){ cout << "hello" << endl; }
    };

    class World:public Command
    {
        public:
            void execute(){ cout << "hello" << endl; }
    };

    class IAm:public Command
    {
        public:
            void execute(){ cout << "I'm the command pattern !" << endl; }
    };



    class Macro
    {
        vector<Command*> commands;
        public:
        void add(Command *c){ commands.push_back(c);}
        void run()
        {
            vector<Command*>::iterator it = commands.begin();
            while (it != commands.end())
                (*it++)->execute();
        }
    };
    int main(int argc, char* argv[])
    {
        Macro macro;

        macro.add(new Hello);
        macro.add(new World);
        macro.add(new IAm);

        macro.run();
        return 0;
    }

#### 4.2 模板方法模式


这里所说的模板与C++中的模板不是一回事。简单地说，模板方法就是定义一个结构框架，应用程序继承这个框架，重用该框架中的大部分函数，并覆盖（重写）其中的小部分函数以实现应用程序自定义的功能。

详细介绍虽然比较拗口，但是也很能帮助我们理解。详细解释如下：应用程序结构框架允许一个或一组类中继承以便创建一个新的应用程序，重用现存类中几乎所有的代码，并且覆盖其中一个或多个函数以便自定义所需要的应用程序。应用程序结构框架中的一个基本的概念是模板方法，它很典型地隐藏在覆盖的下方，通过调用基类的不同函数（这里覆盖了其中一些函数以创建应用程序）来驱动程序运行。模板方法模式的一个重要特征是它的定义在基类中（有时作为一个私有成员函数）并且不能改动——模板方法模式就是“坚持相同的代码”。它调用其他基类函数（就是那些被覆盖的函数）以便完成其工作，但是客户程序员不必直接调用这些函数，如下所示：


    #include <iostream>
    using namespace std;

    class ApplicationFramework
    {
        protected:
            virtual void customize1() = 0;
            virtual void customize2() = 0;
        public:
            void templateMethod()
            {
                int i;
                for (i = 0; i < 2; i++)
                {
                    customize1();
                    customize2();
                }
            }
    };


    //Create a new "application"
    class MyApp: public ApplicationFramework
    {
        protected:
            void customize1(){ cout << "hello, "; }
            void customize2(){ cout << "world" << endl; }
    };

    int main(int argc, char* argv[])
    {
        MyApp app;
        app.templateMethod();
        return 0;
    }

#### 4.3 状态模式

状态模式产生一个可以改变其类的对象，当发现大多数或者所有函数中都存在有条件的代码时，这种模式很有用。

和代理模式一样，状态模式通过一个前端对象来使用后端实现对象履行职责。然而，在前端对象生存期期间，状态模式从一个实现对象到另一个实现对象进行
切换，以实现对于相同的函数调用产生不同的行为。

    #include <iostream>
    using namespace std;

    class Creature
    {
        class State
        {
            public:
                virtual string response() = 0;
        };
        class Frog: public State
        {
            public:
                string response(){ return "Ribbet";}
        };
        class Prince: public State
        {
            public:
                string response(){ return "darling !";}
        };
        State *state;

        public:

        Creature():state(new Frog()){}
        void greet()
        {
            cout << state->response() << endl;
        }
        void kiss()
        {
            delete state;
            state = new Prince();
        }
    };

    int main(int argc, char* argv[])
    {
        Creature creature;
        creature.greet();
        creature.kiss();
        creature.greet();
        return 0;
    }

代理模式和状态模式都提供了一个代理类。代码与代理类打交道，而做实际工作的类隐藏在代理类背后，当调用代理类中的一个函数时，代理类仅转换去调用
实现类中 相应的函数。这两种模式如此相似，从结构上看，可以认为代理模式只是状态模式的一个特例。代理模式只有一个实现类，而状态模式有多个实现，虽然在
运行的时候，同一时刻只能代理一个实现，但是，在程序的声明周期中，实现类是可以改变的。

#### 4.4 策略模式

"策略"的意思就是：可以使用多种方法来解决某个问题，即“条条大路通罗马”。具体实现时，策略模式就是定义一系列的算法，把它们一个个封装起来，并且使它们可以相互替换。这里有一篇很好的介绍策略模式的[文章](http://blog.csdn.net/hguisu/article/details/7558249)，强烈推荐。

    class NameStrategy
    {
        public:
            virtual void greet() = 0;
    };

    class SayHi: public NameStrategy
    {
        public:
            void greet() { cout << "Hi! How's it going ? " << endl; }
    };

    class Ignore: public NameStrategy
    {
        public:
            void greet(){ cout << "Pretend I don't see you" << endl; }
    };

    class Admission: public NameStrategy
    {
        public:
            void greet(){ cout < "I'm sorry. I forgot your name. " << endl; }
    };

    class Contex
    {
        NameStrategy& strategy;
        public:
        contex(NameStrategy& strat):strategy(strat){}
        void greet(){ strategy.greet(); }
    };

    int main()
    {
        SayHi sayhi;
        Ignore ignore;
        Admission admission;
        Context c1(sayhi), c2(ignore), c3(admission);
        c1.greet();
        c2.greet();
        c3.greet();
    }


策略模式和其它许多设计模式比较起来是非常类似的。策略模式和状态模式最大的区别就是策略模式只是的条件选择只执行一次，而状态模式是随着实例参数（对象实例的状态）的改变不停地更改执行模式。换句话说，策略模式只是在对象初始化的时候更改执行模式，而状态模式是根据对象实例的周期时间而动态地改变对象实例的执行模式。

#### 4.5 职责链模式

职责链模式的本质，就是尝试多个解决方法找到一个起作用的方法。在具体实现时，会有一个链表，该链表中的每个策略都试图满足这个调用，遍历这个链表直到找到一个满足调用或到达链序列的末尾才结束。

职责链模式也许被看做一个使用策略对象的"递归动态一般化"。此时提出一个调用，在一个链序列中的每个策略都试图满足这个调用。这个过程直到有一个策略成功满足或者到达链序列末尾才结束。


    enum Answer{ NO, YES};
    class GimmeStrategy
    {
        public:
            virtual Answer canIHave() = 0;
            virtual ~GimmeStrategy(){}
    };

    class AskMom: public GimmeStrategy
    {
        public:
            Answer canIHave()
            {
                cout << "Moom? Can I have this ? " << endl;
                return NO;
            }
    };

    class AskDad: public GimmeStrategy
    {
        public:
            Answer canIHave()
            {
                cout << "Dad, I really need this!" << endl;
                return NO;
            }
    };

    class AskGrandpa: public GimmeStrategy
    {
        public:
            Answer canIHave()
            {
                cout << "Grandpa, is it my birthday yet?" << endl;
                return NO;
            }
    };

    class AskGrandpa: public GimmeStrategy
    {
        public:
            Answer canIHave()
            {
                cout << "Grandama, I really love you!" << endl;
                return YES;
            }
    };

    class Gimme: public GimmeStrategy
    {
        vector<GimmeStrategy*> chain;
        public:
        Gimme()
        {
            chain.push_back(new AskMom());
            chain.push_back(new AskDad());
            chain.push_back(new AskGrandpa());
            chain.push_back(new AskGrandma());
        }
        Answer canIHave()
        {
            vector<GimmeStrategy*>::iterator it = chain.begin();
            while (it != chain.end())
                if ((*it)->canIHave() == YES)
                    return YES;
            //reached end without success
            cout << "Whiiiiinne!" << endl;
            return NO;
        }
        ~Gimme(){ purge(chain); }
    };

    int main(int argc, char const* argv[])
    {
        Gimme chain;
        chain.canIHave();
        return 0;
    }

#### 4.6 观察者模式

观察者模式用于解决一个相当常见的问题：当某些其他对象改变状态时，
如果一组对象需要进行相应的更新，那么，应该如何处理呢？

Observer模式的具体实现中，在被观察的对象中保持观察者的引用，如果被观察者的状态发生了变化，
则通过保持的引用调用观察者的update函数，以实现通知观察者的目的。



    class Observable;

    class Argument {};

    class Observer
    {
        public:
            //called by the observerd object, whenever
            //the observed object is changed:
            virtual void update(Observer &o, Argument* arg) == 0;
            virtual ~Observer(){}
    };


    class Observable
    {
        bool changed;
        std::set<Observer*> observers;
        protected:
        virtual void setChanged(){ changed = true; }
        virtual void clearChanged(){ changed = false; }
        public:
        virtual void addObserver(Observer &o)
        {
            observers.insert(&o);
        }
        virtual void deleteObservers()
        {
            observers.clear();
        }
        virtual int countObservers()
        {
            return observers.size();
        }

        virtual bool hasChanged(){ return changed; }

        //If this object has changed, notify all
        //of its observers
        virtual void notifyObservers(Argument* arg = 0)
        {
            if(!hasChanged()) return;
            clearChanged();
            std::set<Observer*>::iterator it;
            for(it = observers.begin(); it != observers.end(); it++)
                (*it)->update(this, arg);
        }
        virtual ~Observable(){}
    };

#### 4.7 访问者模式

visitor模式实现了不改变类本身，却改变类的行为的功能。根据《C++编程思想》的作者
Bruce Eckel的说法：因为其不寻常的动机以及愚笨的约束，使得人们极不可能使用访问者模式。

**后记:**

对于Java程序员，设计模式接触得更多一些，C++不像Java一样抽象得彻底，一般在C++代码中，只
用到最简单的一些设计模式。现在已经有一部分人呼吁反设计模式，太多的抽象同样会导致程序难以
理解。所以，我个人觉得，学习一些简单的设计模式，能够在阅读开源代码的时候轻松的识别代码
中使用的设计模式，在自己的程序中使用一些简单的模式就足够了。
