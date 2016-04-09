---
layout: post
title: "redis源码好在哪里之命令处理"
description: ""
category: 源码分析
tags: [redis]
---


redis的命令处理流程代码写得非常好，值得我们借鉴学习。

## 为啥要学习redis的命令处理

redis需要处理不同的命令，对于每一个命令，具体到代码实现时，就是不同的函数。这种需求是如此的常见，以致于你应该见过或者写过很多类似下面这样的代码：

    if (cmd == 'a') 
        processA()
    else if (cmd == 'b')
        processB()
    else if (cmd == 'C')
        processC()
    else if (cmd == 'd')
        processD()
        ……
    else
        raise NotImplementException

考虑到redis有160+个命令，如果按照这种写法，那么，这段if语句将会非常长，长到骇人听闻，长到难以维护。

## redis的命令处理流程

那么，redis是怎么处理的呢？要回答这个问题，先简单介绍一下redis的命令处理流程：

1. 接收请求：读入客户端发送过来的请求(query_string)，解析请求获取命令
2. 查找命令：在hash表中查找命令，判断用户输入的命令是否正确、是否存在
3. 预检查：检查命令的参数个数是否满足要求，客户端是否已经认证等
4. 执行命令：执行命令
5. 执行完成：发送结果到客户端，记录慢日志等


下面是redis命令处理的堆栈：

    #0  quicklistCreate () at quicklist.c:98
    #1  0x000000000043c297 in createQuicklistObject () at object.c:184
    #2  0x000000000044b08b in pushGenericCommand (c=0x7f529e11c000, where=0) at t_list.c:209
    #3  0x000000000044b1ed in lpushCommand (c=0x7f529e11c000) at t_list.c:228
    #4  0x0000000000429591 in call (c=0x7f529e11c000, flags=7) at redis.c:2091
    #5  0x000000000042a054 in processCommand (c=0x7f529e11c000) at redis.c:2341
    #6  0x00000000004390f5 in processInputBuffer (c=0x7f529e11c000) at networking.c:1171
    #7  0x00000000004393cf in readQueryFromClient (el=0x7f529e058150, fd=6, privdata=0x7f529e11c000, mask=1) at networking.c:1232
    #8  0x0000000000421ded in aeProcessEvents (eventLoop=0x7f529e058150, flags=3) at ae.c:412
    #9  0x0000000000421f77 in aeMain (eventLoop=0x7f529e058150) at ae.c:455
    #10 0x000000000042ddc1 in main (argc=1, argv=0x7fffbab43408) at redis.c:3889
    
## redis的命令处理流程详解

我们直接将目光聚焦到call函数，call函数的部分内容如下：
    
    /* Call() is the core of Redis execution of a command */
    void call(redisClient *c, int flags) {
        long long dirty, start, duration;
        int client_old_flags = c->flags;
    
        /* Sent the command to clients in MONITOR mode, only if the commands are
         * not generated from reading an AOF. */
        if (listLength(server.monitors) &&
            !server.loading &&
            !(c->cmd->flags & (REDIS_CMD_SKIP_MONITOR|REDIS_CMD_ADMIN)))
        {
            replicationFeedMonitors(c,server.monitors,c->db->id,c->argv,c->argc);
        }
    
        /* Call the command. */
        c->flags &= ~(REDIS_FORCE_AOF|REDIS_FORCE_REPL);
        redisOpArrayInit(&server.also_propagate);    dirty = server.dirty;
        start = ustime();
        c->cmd->proc(c);  //重点
        duration = ustime()-start;
        dirty = server.dirty-dirty;
        if (dirty < 0) dirty = 0;
        
无论是什么命令，都会进入到call函数体，call函数通过调用`c->cmd-proc(c)`执行具体的命令。那么，proc又是啥？我们看一下redisCommand结构体：
        
        struct redisCommand {
        char *name;
        redisCommandProc *proc;
        int arity;
        char *sflags; /* Flags as string representation, one char per flag. */
        ……
    };
    
`proc`只是一个指针，具体指向可以看redisCommand的初始化结构体：

    struct redisCommand redisCommandTable[] = {
        {"get",getCommand,2,"rF",0,NULL,1,1,1,0,0},
        {"set",setCommand,-3,"wm",0,NULL,1,1,1,0,0},
        {"setnx",setnxCommand,3,"wmF",0,NULL,1,1,1,0,0},
        {"setex",setexCommand,4,"wm",0,NULL,1,1,1,0,0},
        {"psetex",psetexCommand,4,"wm",0,NULL,1,1,1,0,0},
        {"append",appendCommand,3,"wm",0,NULL,1,1,1,0,0},
        {"strlen",strlenCommand,2,"rF",0,NULL,1,1,1,0,0},
        {"del",delCommand,-2,"w",0,NULL,1,-1,1,0,0},
        
这里简单解释一下，proc是一个函数指针，在命令初始化的时候，对每一个命令，传递不同的函数指针。个人觉得，将proc理解为回调函数（c语言似乎没有这种说法）更加合适。

对于每一个命令，都会有一个redisCommand结构体，该结构中的proc保存了一个回调函数，redis接收到客户端的请求以后，通过调用回调函数，执行相应的命令。这就实现了对于每个命令调用不同函数的需求，同时也保证的代码的可读性，更重要的，代码可维护性显著提高。

## 在面向对象的编程语言中的处理方法

类似上面的需求，在面向对象中还有更好的处理办法。例如，[ZooKeeper][1]中（ZooKeeper代码写得是真好啊），通过多态实现了类似的需求。


    protected boolean processZKCmd(MyCommandOptions co)
        throws KeeperException, IOException, InterruptedException
    {
        String[] args = co.getArgArray();
        String cmd = co.getCommand();

        if (!commandMap.containsKey(cmd)) {
            usage();
            return false;
        }
 
        // execute from commandMap
        CliCommand cliCmd = commandMapCli.get(cmd);
        if(cliCmd != null) {
            cliCmd.setZk(zk);
            watch = cliCmd.parse(args).exec(); // **********重点************
        } else if (!commandMap.containsKey(cmd)) {
             usage();
        }
    }

[1]: https://github.com/lalor/zookeeper/blob/trunk/src/java/main/org/apache/zookeeper/ZooKeeperMain.java#L610-L685
