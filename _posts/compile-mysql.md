<!--本文使用了markdown编辑，感兴趣的同学可以自行google-->

本文介绍了在Ubuntu 12.04LTS下源码mysql源码的详细步骤。

##1. 安装make 编译器（一般linux自带已经有了）

下载地址：http://www.gun.org/software/make/

	tar zxvf make-3.82.tar.gz
	cd make-3.82
	./configure
	make
	make install


##2. 安装bison

下载地址：http://www.gun.org/software/bison/

	tar zxvf bison-2.7.tar.gz
	cd bison-3.7
	./configure
	make
	make install


##3. 安装cmake

下载地址：http://www.cmake.org

	tar zxvf cmake-2.8.4.tar.gz
	cd cmake-2.8.4
	./configure
	make
	make install

##4. 安装mysql 源码需要的库

	sudo apt-get install build-essential libncurses5-dev

##5. 开始安装MySQL

	下载地址：http://dev.mysql.com/, 解压并安装MySQL

	tar zxvf mysql-5.6.13.tar.gz
	cd mysql-5.6.13 #/home/lalor/code/mysql-5.6.13
    cmake . -DCMAKE_INSTALL_PREFIX=/home/lalor/code/mysql \
    -DMYSQL_DATADIR=/home/lalor/code/mysql/data
	make
	make install

注意1:执行cmake 命令时，反斜杠后面不能有空格。

注意2:编译mysql-5.6.13时，请自行修改mysql的安装路径。

注意3:我的用户名是lalor,在home目录下新建了一个code目录作为工作目录，示例将mysql安装到/home/lalor/code/mysql目录下。

##6. 完成后，继续下面的操作初始化数据库

	cd /home/lalor/code/mysql
	./scripts/mysql_install_db --user=lalor --no-defaults

切记后面有个`--no-defaults`选项，如果没有该选项，则程序会自动载入默认的配置文档，而目前你还没有完成配置文件的编写，因此，很可能载入的是错误的信息。如果该指令能够运行成功，那么恭喜你，你的MySQL可以成功启动了。

注意4:不同于其他安装方式，源码编译mysql 以后还需要使用mysql自带的perl脚本（mysql_install_db）初始化一个数据库，这是新手最容易出错的地方，因为新手不知道需要这一步操作。

##7. 修改配置文件

关于mysql的配置文件的前后顺序，可以参考[这里](http://weibo.com/2216172320/zxUaAaiuu)。

在/home/lalor/code/mysql目录下新建my.cnf文件，加入下面几条语句(请自行修改路径)：

	[client]
    port = 8501
	socket = /home/lalor/code/mysql/data/mysqld.sock

	[mysqld]
    port = 8501
	basedir =/home/lalor/code/mysql
	datadir =/home/lalor/code/mysql/data
	socket = /home/lalor/code/mysql/data/mysqld.sock
    default_storage_engine  = innodb


##8. 运行服务进程：

	cd /home/lalor/code/mysql
    ./bin/mysqld --defaults-file=my.cnf &

测试完成以后，可以使用kill关闭mysqld或者使用mysqladmin关闭mysqld。

##9. 运行客户端进程：

	./bin/mysql --defaults-file=my.cnf -uroot

初始时，root密码为空，直接回车即可。

