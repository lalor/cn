
#1. 介绍

Python 2.2 开始提供了装饰器（decorator），装饰器作为修改函数的一种便捷方式，为程序员编写程序提供了便利性和灵活性，适当使用装饰器，能够有效的提高代码的可读性和可维护性，然而，装饰器并没有被广泛的使用，主要还是因为大多数人并不理解装饰器的工作机制。

本文首先介绍了装饰器的概念和用法（第2节），然后介绍了装饰器使用过程中的注意事项（第3节），最后讨论了装饰器的使用场景（第4节）。

# 2. 装饰器

装饰器本质上就是一个函数，这个函数接收其他函数作为参数，并将其以一个新的修改后的函数进行替换。概念比较抽象，一起来看两个装饰器的例子。

## 2.1 装饰器的概念

考虑这样一组函数，它们在被调用时需要对某些参数进行检查，在本例中，需要对用户名进行检查，以判断用户是否有相应的权限进行某些操作。

程序清单1


    class Store(object):
        def get_food(self, username, food):
            if username != 'admin':
                raise Exception("This user is not allowed to get food")
            return self.storage.get(food)

        def put_food(self, username, food):
            if username != 'admin':
                raise Exception("This user is not allowed to put food")
            self.storage.put(food)



显然，代码有重复，作为一个有追求的工程师，我们严格遵守DRY(Don't repeat yourself)原则，于是，代码被改写成了程序清单2这样：


程序清单2

    def check_is_admin(username):
        if username != 'admin':
            raise Exception("This user is not allowed to get food")

    class Store(object):
        def get_food(self, username, food):
            check_is_admin(username)
            return self.storage.get(food)

        def put_food(self, username, food):
            check_is_admin(username)
            return self.storage.put(food)



现在代码整洁一点了，但是，有装饰器能够做的更好：

程序清单3

    def check_is_admin(f):
        def wrapper(*args, **kwargs):
            if kwargs.get('username') != 'admin':
                raise Exception("This user is not allowed to get food")
            return f(*arg, **kargs)
        return wrapper

    class Storage(object):
        @check_is_admin
        def get_food(self, username, food):
            return self.storage.get(food)

        @check_is_admin
        def put_food(self, username, food):
            return storage.put(food)

上面这段代码就是使用装饰器的典型例子:函数里面定义了一个函数，并将定义的这个函数作为返回值。这个例子足够简单，所以它的好处也不够明显，但是，却可以很好的演示装饰器的语法。

即使这样，我们也可以说，程序清单3比程序清单2更好，因为程序清单3能够将条件检查与具体逻辑分隔开来。在本例中，`check_is_admin`只是预检查，它的重要性不如具体的业务逻辑。我们将业务逻辑看做是这段程序的"重点"的话，那么，程序清单3一眼看过去就能看到"重点"，而程序清单2则不能，需要简单的思考（转个弯）才能区分条件检查和业务逻辑。当然，你可能觉得这没什么，但是，作为一名有追求的工程师，我们希望我们写出的代码能和散文一样优美。


## 2.2 装饰器的本质

前面说过，**装饰器本质上就是一个函数，这个函数接收其他函数作为参数，并将其以一个新的修改后的函数进行替换**。下面这个例子能够更好地理解这句话。


程序清单4


    def bread(func):
        def wrapper():
            print "</''''''\>"
            func()
            print "</______\>"
        return wrapper

    sandwich_copy = bread(sandwich)
    sandwich_copy()


输出结果如下：


    </''''''\>
    --ham--
    </______\>

分析如下：bread是一个函数，它接受一个函数作为参数，然后返回一个新的函数，新的函数对原来的函数进行了一些修改和扩展，且这个新函数可以当做普通函数进行调用。

下面这段代码和程序清单4输出结果一摸一样，只是用了python提供的装饰器语法，看起来更加简单直接。

程序清单5 

    def bread(func):
        def wrapper():
            print "</''''''\>"
            func()
            print "</______\>"
        return wrapper

    @bread
    def sandwich(food="--ham--"):
        print food


到这里，我们应该已经能够理解装饰器的作用和用法了，再强调一遍： 装饰器本质上就是一个函数，这个函数接收其他函数作为参数，并将其以一个新的修改后的函数进行替换

# 3. 使用装饰器需要注意的地方

我们在上一节中演示了装饰器的用法，可以看到，装饰器其实很好理解，也非常简单。但是，要用好装饰器，还有一些我们需要注意的地方，这一节就对这些需要注意的地方进行了讨论，首先讨论了使用多个装饰器时，各个装饰器的调用顺序(见3.1节);然后介绍了在使用装饰器的情况下，如果保留原有函数的属性(见3.2节)；

## 3.1 多个装饰器的调用顺序


## 3.2 函数的属性变化

装饰器动态创建的新函数替换原来的函数，但是，新函数缺少很多原函数的属性，如docstring和名字。

程序清单4


    def is_admin(f):
        @functools.wraps(f)
        def wrapper(*args, **kwargs):
            if kwargs.get("username") != 'admin':
                raise Exception("This user is not allowed to get food")
            return f(*arg, **kwargs)
        return wrapper

    def foobar(username='someone'):
        """Do crazy stuff"""
        pass

    @is_admin
    def barfoo(username='someone'):
        """Do crazy stuff"""
        pass

    def main():
        print foobar.func_doc
        print foobar.__name__

        print barfoo.func_doc
        print barfoo.__name__

    if __name__ == '__main__':
        main()

程序清单4的输出结果：

    Do crazy stuff
    foobar

    None
    wrapper

程序清单4中，我们定义了两个函数`foobar`与`barfoo`，其中，`barfoo`使用装饰器进行了封装，我们获取`foobar`与`barfoo`的docstring和函数名字，可以看到，使用了装饰器的函数，不能够正确获取函数原有的docstring与名字，为了解决这个问题，可以使用python内置的 functools模块。

程序清单5


    import functools

    def is_admin(f):
        def wrapper(*args, **kwargs):
            if kwargs.get("username") != 'admin':
                raise Exception("This user is not allowed to get food")
            return f(*arg, **kwargs)
        return wrapper


我们只需要增加一行代码，就能够正确地获取函数的属性。

## 3.3 使用inspect获取函数参数

下面看一下程序清单6,它是否会正确输出结果呢？

程序清单6

    import functools

    def check_is_admin(f):
        @functools.wraps(f)
        def wrapper(*args, **kwargs):
            print kwargs
            if kwargs.get('username') != 'admin':
                raise Exception("This user is not allowed to get food")
            return f(*args, **kwargs)
        return wrapper


    @check_is_admin
    def get_food(username, food='chocolate'):
        return "{0} get food: {1}".format(username, food)


    def main():
        print get_food('admin')

    if __name__ == '__main__':
        main()

程序清单6会抛出一个异常，因为我们传入的'admin'是一个位置参数，而我们却去关键字参数(kwargs)获取用户名，因此，`kwargs.get('username')返回None，那么，权限检查发现，用户没有相应的权限，抛出异常。

为了提供一个更加智能的装饰器，我们需要使用python的inspect模块。inspect能够取出函数的签名，并对其进行操作，如下所示：

程序清单7


    import functools
    import inspect

    def check_is_admin(f):
        @functools.wraps(f)
        def wrapper(*args, **kwargs):
            func_args = inspect.getcallargs(f, *args, **kwargs)
            print func_args
            if func_args.get('username') != 'admin':
                raise Exception("This user is not allowed to get food")
            return f(*args, **kwargs)
        return wrapper


    @check_is_admin
    def get_food(username, food='chocolate'):
        return "{0} get food: {1}".format(username, food)


    def main():
        print get_food('admin')

    if __name__ == '__main__':
        main()

承担主要工作的函数是inspect.getcallargs，它返回一个将参数名字和值作为键值对的字典，这程序清单7中，这个函数返回`{'username':'admin', 'food':'chocolate'}`。这意味着我们的装饰器不必检查参数username是基于位置的参数还是基于关键字的参数，而只需在字典中查找即可。

## 3.3 多个装饰器的调用顺序

    def bread(func):
        def wrapper():
            print "</''''''\>"
            func()
            print "<\______/>"
        return wrapper

    def ingredients(func):
        def wrapper():
            print "#tomatoes#"
            func()
            print "~salad~"
        return wrapper

    def sandwich(food="--ham--"):
        print food

    sandwich()
    #outputs: --ham--
    sandwich = bread(ingredients(sandwich))
    sandwich()
    #outputs:
    #</''''''\>
    # #tomatoes#
    # --ham--
    # ~salad~
    #<\______/>
    Using the Python decorator syntax:

    @bread
    @ingredients
    def sandwich(food="--ham--"):
        print food

    sandwich()
    #outputs:
    #</''''''\>
    # #tomatoes#
    # --ham--
    # ~salad~
    #<\______/>
    The order you set the decorators MATTERS:

    @ingredients
    @bread
    def strange_sandwich(food="--ham--"):
        print food

    strange_sandwich()
    #outputs:
    ##tomatoes#
    #</''''''\>
    # --ham--
    #<\______/>
    # ~salad~

## 3.4 给装饰器传递参数

from funcy import dector

http://stackoverflow.com/questions/739654/how-can-i-make-a-chain-of-function-decorators-in-python/1594484#1594484

## 3.5 装饰器的装饰器

# 4. 装饰器的使用场景


# 4.1 装饰器可以做哪些事情

这有一份Python官方的装饰器实例列表，你可以在里边看到装饰器的各种妙用：PythonDecoratorLibrary，基本上你差不多能想到的都有了。

差不多有这么几类：

1. 注入参数（提供默认参数，生成参数） 2. 记录函数行为（日志、缓存、计时什么的）
3. 预处理／后处理（配置上下文什么的）
4. 修改调用时的上下文（线程异步或者并行，类方法）

1. 使用场景

2. 预检查

# 4.2 装饰器有哪些缺点


4. flask


Now the big question: What can I use decorators for?

Seem cool and powerful, but a practical example would be great. Well, there are 1000 possibilities. Classic uses are extending a function behavior from an external lib (you can't modify it), or for debugging (you don't want to modify it because it’s temporary).

You can use them to extend several functions in a DRY’s way, like so:

    def benchmark(func):
        """
        A decorator that prints the time a function takes
        to execute.
        """
        import time
        def wrapper(*args, **kwargs):
            t = time.clock()
            res = func(*args, **kwargs)
            print func.__name__, time.clock()-t
            return res
        return wrapper


    def logging(func):
        """
        A decorator that logs the activity of the script.
        (it actually just prints it, but it could be logging!)
        """
        def wrapper(*args, **kwargs):
            res = func(*args, **kwargs)
            print func.__name__, args, kwargs
            return res
        return wrapper


    def counter(func):
        """
        A decorator that counts and prints the number of times a function has been executed
        """
        def wrapper(*args, **kwargs):
            wrapper.count = wrapper.count + 1
            res = func(*args, **kwargs)
            print "{0} has been used: {1}x".format(func.__name__, wrapper.count)
            return res
        wrapper.count = 0
        return wrapper

    @counter
    @benchmark
    @logging
    def reverse_string(string):
        return str(reversed(string))

    print reverse_string("Able was I ere I saw Elba")
    print reverse_string("A man, a plan, a canoe, pasta, heros, rajahs, a coloratura, maps, snipe, percale, macaroni, a gag, a banana bag, a tan, a tag, a banana bag again (or a camel), a crepe, pins, Spam, a rut, a Rolo, cash, a jar, sore hats, a peon, a canal: Panama!")

    #outputs:
    #reverse_string ('Able was I ere I saw Elba',) {}
    #wrapper 0.0
    #wrapper has been used: 1x
    #ablE was I ere I saw elbA
    #reverse_string ('A man, a plan, a canoe, pasta, heros, rajahs, a coloratura, maps, snipe, percale, macaroni, a gag, a banana bag, a tan, a tag, a banana bag again (or a camel), a crepe, pins, Spam, a rut, a Rolo, cash, a jar, sore hats, a peon, a canal: Panama!',) {}
    #wrapper 0.0
    #wrapper has been used: 2x
    #!amanaP :lanac a ,noep a ,stah eros ,raj a ,hsac ,oloR a ,tur a ,mapS ,snip ,eperc a ,)lemac a ro( niaga gab ananab a ,gat a ,nat a ,gab ananab a ,gag a ,inoracam ,elacrep ,epins ,spam ,arutaroloc a ,shajar ,soreh ,atsap ,eonac a ,nalp a ,nam A
    Of course the good thing with decorators is that you can use them right away on almost anything without rewriting. DRY, I said:

    @counter
    @benchmark
    @logging
    def get_random_futurama_quote():
        from urllib import urlopen
        result = urlopen("http://subfusion.net/cgi-bin/quote.pl?quote=futurama").read()
        try:
            value = result.split("<br><b><hr><br>")[1].split("<br><br><hr>")[0]
            return value.strip()
        except:
            return "No, I'm ... doesn't!"


    print get_random_futurama_quote()
    print get_random_futurama_quote()

    #outputs:
    #get_random_futurama_quote () {}
    #wrapper 0.02
    #wrapper has been used: 1x
    #The laws of science be a harsh mistress.
    #get_random_futurama_quote () {}
    #wrapper 0.01
    #wrapper has been used: 2x
    #Curse you, merciful Poseidon!
    Python itself provides several decorators: property, staticmethod, etc.


[1]: http://www.cnblogs.com/SeasonLee/articles/1719444.html
[2]:
