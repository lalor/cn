---
layout: post
title: "命令行工具是怎么来的？"
description: ""
category: 程序设计
tags: [工具, cat, 源码]
---

**问：**linux下这些命令到底是用什么写的？用linux这么久，有没有想过这些命令是怎么实现的。

之前我觉得linux下的命令强大得不得了，为此我还搜了一下这些命令是怎么
实现的，无果。今天下载了freeBSD源码，正好看到bin目录下有与各个命令相同的目录，就好奇进
去看了一下，这些命令远比我想象中的简单，很多我们平时常用的命令，都只有两三百行代码。

以cat为例（因为它直接编译可用，不依赖任何其他文件），加上注释才263行。考虑到cat
使用的频繁程度，我忍不住叹息：生不逢时啊！为什么cat这么简单这么常用的代码不是我写的!

我在bin/cat/目录下直接编译cat.c文件，得到a.out这个二进制文件，在当前目录下a.out的用法
和cat命令的用法一样，恩，它就是cat。

如果我们把它重命名为`mycat`，然后拷贝到/usr/local/bin目录下，之后`mycat`
这个命令就和cat命令一摸一样了，它可以完全代替cat这个命令。这就是一个终端工具的产生过程？
是的，它并不神奇，它就是一个普通的程序。


下来是cat的源码：


    /*
     * Copyright (c) 1989, 1993
     *	The Regents of the University of California.  All rights reserved.
     *
     * This code is derived from software contributed to Berkeley by
     * Kevin Fall.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * 1. Redistributions of source code must retain the above copyright
     *    notice, this list of conditions and the following disclaimer.
     * 2. Redistributions in binary form must reproduce the above copyright
     *    notice, this list of conditions and the following disclaimer in the
     *    documentation and/or other materials provided with the distribution.
     * 3. All advertising materials mentioning features or use of this software
     *    must display the following acknowledgement:
     *	This product includes software developed by the University of
     *	California, Berkeley and its contributors.
     * 4. Neither the name of the University nor the names of its contributors
     *    may be used to endorse or promote products derived from this software
     *    without specific prior written permission.
     *
     * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
     * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
     * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
     * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
     * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
     * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
     * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
     * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
     * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
     * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
     * SUCH DAMAGE.
     */
    
    #ifndef lint
    static char const copyright[] =
    "@(#) Copyright (c) 1989, 1993\n\
    	The Regents of the University of California.  All rights reserved.\n";
    #endif /* not lint */
    
    #ifndef lint
    #if 0
    static char sccsid[] = "@(#)cat.c	8.2 (Berkeley) 4/27/95";
    #endif
    static const char rcsid[] =
      "$FreeBSD$";
    #endif /* not lint */
    
    #include <sys/param.h>
    #include <sys/stat.h>
    
    #include <ctype.h>
    #include <err.h>
    #include <fcntl.h>
    #include <locale.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    
    int bflag, eflag, nflag, sflag, tflag, vflag;
    int rval;
    const char *filename;
    
    void cook_args __P((char *argv[]));
    void cook_buf __P((FILE *));
    int main __P((int argc, char *argv[]));
    void raw_args __P((char *argv[]));
    void raw_cat __P((int));
    
    int
    main(argc, argv)
    	int argc;
    	char *argv[];
    {
    	int ch;
    
    	setlocale(LC_CTYPE, "");
    
    	while ((ch = getopt(argc, argv, "benstuv")) != -1)
    		switch (ch) {
    		case 'b':
    			bflag = nflag = 1;	/* -b implies -n */
    			break;
    		case 'e':
    			eflag = vflag = 1;	/* -e implies -v */
    			break;
    		case 'n':
    			nflag = 1;
    			break;
    		case 's':
    			sflag = 1;
    			break;
    		case 't':
    			tflag = vflag = 1;	/* -t implies -v */
    			break;
    		case 'u':
    			setbuf(stdout, NULL);
    			break;
    		case 'v':
    			vflag = 1;
    			break;
    		default:
    			(void)fprintf(stderr,
    			    "usage: cat [-benstuv] [-] [file ...]\n");
    			exit(1);
    		}
    	argv += optind;
    
    	if (bflag || eflag || nflag || sflag || tflag || vflag)
    		cook_args(argv);
    	else
    		raw_args(argv);
    	if (fclose(stdout))
    		err(1, "stdout");
    	exit(rval);
    }
    
    void
    cook_args(argv)
    	char **argv;
    {
    	register FILE *fp;
    
    	fp = stdin;
    	filename = "stdin";
    	do {
    		if (*argv) {
    			if (!strcmp(*argv, "-"))
    				fp = stdin;
    			else if ((fp = fopen(*argv, "r")) == NULL) {
    				warn("%s", *argv);
    				rval = 1;
    				++argv;
    				continue;
    			}
    			filename = *argv++;
    		}
    		cook_buf(fp);
    		if (fp != stdin)
    			(void)fclose(fp);
    	} while (*argv);
    }
    
    void
    cook_buf(fp)
    	register FILE *fp;
    {
    	register int ch, gobble, line, prev;
    
    	line = gobble = 0;
    	for (prev = '\n'; (ch = getc(fp)) != EOF; prev = ch) {
    		if (prev == '\n') {
    			if (ch == '\n') {
    				if (sflag) {
    					if (!gobble && putchar(ch) == EOF)
    						break;
    					gobble = 1;
    					continue;
    				}
    				if (nflag && !bflag) {
    					(void)fprintf(stdout, "%6d\t", ++line);
    					if (ferror(stdout))
    						break;
    				}
    			} else if (nflag) {
    				(void)fprintf(stdout, "%6d\t", ++line);
    				if (ferror(stdout))
    					break;
    			}
    		}
    		gobble = 0;
    		if (ch == '\n') {
    			if (eflag)
    				if (putchar('$') == EOF)
    					break;
    		} else if (ch == '\t') {
    			if (tflag) {
    				if (putchar('^') == EOF || putchar('I') == EOF)
    					break;
    				continue;
    			}
    		} else if (vflag) {
    			if (!isascii(ch) && !isprint(ch)) {
    				if (putchar('M') == EOF || putchar('-') == EOF)
    					break;
    				ch = toascii(ch);
    			}
    			if (iscntrl(ch)) {
    				if (putchar('^') == EOF ||
    				    putchar(ch == '\177' ? '?' :
    				    ch | 0100) == EOF)
    					break;
    				continue;
    			}
    		}
    		if (putchar(ch) == EOF)
    			break;
    	}
    	if (ferror(fp)) {
    		warn("%s", filename);
    		rval = 1;
    		clearerr(fp);
    	}
    	if (ferror(stdout))
    		err(1, "stdout");
    }
    
    void
    raw_args(argv)
    	char **argv;
    {
    	register int fd;
    
    	fd = fileno(stdin);
    	filename = "stdin";
    	do {
    		if (*argv) {
    			if (!strcmp(*argv, "-"))
    				fd = fileno(stdin);
    			else if ((fd = open(*argv, O_RDONLY, 0)) < 0) {
    				warn("%s", *argv);
    				rval = 1;
    				++argv;
    				continue;
    			}
    			filename = *argv++;
    		}
    		raw_cat(fd);
    		if (fd != fileno(stdin))
    			(void)close(fd);
    	} while (*argv);
    }
    
    void
    raw_cat(rfd)
    	register int rfd;
    {
    	register int off, wfd;
    	ssize_t nr, nw;
    	static size_t bsize;
    	static char *buf;
    	struct stat sbuf;
    
    	wfd = fileno(stdout);
    	if (buf == NULL) {
    		if (fstat(wfd, &sbuf))
    			err(1, "%s", filename);
    		bsize = MAX(sbuf.st_blksize, 1024);
    		if ((buf = malloc(bsize)) == NULL)
    			err(1, "buffer");
    	}
    	while ((nr = read(rfd, buf, bsize)) > 0)
    		for (off = 0; nr; nr -= nw, off += nw)
    			if ((nw = write(wfd, buf + off, (size_t)nr)) < 0)
    				err(1, "stdout");
    	if (nr < 0) {
    		warn("%s", filename);
    		rval = 1;
    	}
    }
    
