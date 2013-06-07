---
layout: post
title: "一个处理CSV文件库的成长过程"
description: ""
category: 程序设计
tags: [CSV, 程序设计实践]
---

**目录：**

1.  问题描述
1.  第一个原型
1.  第二个原型
1.  最终版
1.  C++ 版
1.  总结


#1.  问题描述

这里摘录了一个《[程序设计实践][1]》第四章的一个例子，用来演示程序的一步步进化过程，并体验程序开发的乐趣和实际开发中的思考过程。

**问题：**读入一个[CSV][2]文件，并将其转换为内部表示。具体来讲，就是读入一个CSV(comma-separated value)文件，CSV文件是用逗号分隔的数据,一行表示一条记录，每个字段用逗号分隔，说得简单一点，它就是一个用逗号分割的excel文档。程序的任务就是将该文件读入，解析该文件，使得库的调用者可以像使用数组一样使用CSV文件。


#2.  第一个原型

我们的第一个版本很简单，直接一行一行的读入数据，然后根据逗号，将数据分割成一个一个的数据域。

    /* Copyright (C) 1999 Lucent Technologies */
    /* Excerpted from 'The Practice of Programming' */
    /* by Brian W. Kernighan and Rob Pike */
    
    #include <stdio.h>
    #include <string.h>
    
    char buf[200];		/* input line buffer */
    char *field[20];	/* fields */
    char *unquote(char *);
    /* csvgetline: read and parse line, return field count */
    /* sample input: "LU",86.25,"11/4/1998","2:19PM",+4.0625 */
    int csvgetline(FILE *fin)
    {	
    	int nfield;
    	char *p, *q;
    /* spacer */
    	if (fgets(buf, sizeof(buf), fin) == NULL)
    		return -1;
    	nfield = 0;
    	for (q = buf; (p=strtok(q, ",\n\r")) != NULL; q = NULL)
    		field[nfield++] = unquote(p);
    	return nfield;
    }
    
    /* unquote: remove leading and trailing quote */
    char *unquote(char *p)
    {
    	if (p[0] == '"') {
    		if (p[strlen(p)-1] == '"')
    			p[strlen(p)-1] = '\0';
    		p++;
    	}
    	return p;
    }
    
    extern char *field[];
    
    /* csvtest main: test csvgetline function */
    int main(void)
    {
    	int i, nf;
    /* spacer */
    	while ((nf = csvgetline(stdin)) != -1)
    		for (i = 0; i < nf; i++)
    			printf("field[%d] = `%s'\n", i, field[i]);
    	return 0;
    }
    

csvgetline 函数读入一行CSV 数据行，将它放入缓冲区，在一个数组里把该行分解为一些数据域，最后返回数据域的个数。

函数unquote 的功能是去除数据行里的引号，它并不能处理引号嵌套的情况。

**分析:** 对于普通的程序员，这个程序顶多半个小时就能搞定了，对于像我现在这水平的程序员，也就止步于此了。但是，这个程序能够安全的运行吗，能够处理各种诡异的输入吗?下面列出了该程序的问题。

* 没有处理特别长的行，很多域的情况。遇到这些情况时它可能给出错误结果，甚至垮台，因为它没有检查溢出，在出现错误时也没有返回某种合理的值
* 这里假定输入是由换行字符结尾的行组成
* 数据域由逗号分隔，数据域前后的引号将被去除，但没有考虑潜逃引号或逗号的情况
* 输入行没有保留，在构造数据域的过程中将它覆盖了
* 从一行输入转到另一行的时候没有保留任何数据。如果需要记录什么东西，那么就必须做一个拷贝
* 对数据域的访问是通过全局变量进行的。这个数组由csvgetline 与调用函数共享。这里对数据域内容或指针的访问都没有任何限制。对于超出最后一个域的访问也没有任何防御措施
* 使用了全局变量，这就使得这个设计不适合多线程环境，甚至也不允许两个交替进行的调用序列
* 输入与划分纠缠在一起：每个调用读入一行并把它切分为一些域，不管实际应用中是否真的需要后一个服务

#3.  第二个原型

上面列出了很多问题，并不是每一个问题都能够解决，可以根据实际需求进行变化，但是"长的输入行，很多的数据域以及未预料到的或者欠缺的分隔符都可能造成大麻烦"这个问题是迫切需要解决的。

csv.h

    /* csv.h: interface for csv library */
    
    extern char *csvgetline(FILE *f); /* read next input line */
    extern char *csvfield(int n);	  /* return field n */
    extern int csvnfield(void);		  /* return number of fields */


主文件

    /* Copyright (C) 1999 Lucent Technologies */
    /* Excerpted from 'The Practice of Programming' */
    /* by Brian W. Kernighan and Rob Pike */
    
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <assert.h>
    
    #include "csv.h"
    
    enum { NOMEM = -2 };          /* out of memory signal */
    
    static char *line    = NULL;  /* input chars */
    static char *sline   = NULL;  /* line copy used by split */
    static int  maxline  = 0;     /* size of line[] and sline[] */
    static char **field  = NULL;  /* field pointers */
    static int  maxfield = 0;     /* size of field[] */
    static int  nfield   = 0;     /* number of fields in field[] */
    
    static char fieldsep[] = ","; /* field separator chars */
    
    static char *advquoted(char *);
    static int split(void);
    
    /* endofline: check for and consume \r, \n, \r\n, or EOF */
    static int endofline(FILE *fin, int c)
    {
    	int eol;
    
    	eol = (c=='\r' || c=='\n');
    	if (c == '\r') {
    		c = getc(fin);
    		if (c != '\n' && c != EOF)
    			ungetc(c, fin);	/* read too far; put c back */
    	}
    	return eol;
    }
    
    /* reset: set variables back to starting values */
    static void reset(void)
    {
    	free(line);	/* free(NULL) permitted by ANSI C */
    	free(sline);
    	free(field);
    	line = NULL;
    	sline = NULL;
    	field = NULL;
    	maxline = maxfield = nfield = 0;
    }
    
    /* csvgetline:  get one line, grow as needed */
    /* sample input: "LU",86.25,"11/4/1998","2:19PM",+4.0625 */
    char *csvgetline(FILE *fin)
    {	
    	int i, c;
    	char *newl, *news;
    
    	if (line == NULL) {			/* allocate on first call */
    		maxline = maxfield = 1;
    		line = (char *) malloc(maxline);
    		sline = (char *) malloc(maxline);
    		field = (char **) malloc(maxfield*sizeof(field[0]));
    		if (line == NULL || sline == NULL || field == NULL) {
    			reset();
    			return NULL;		/* out of memory */
    		}
    	}
    	for (i=0; (c=getc(fin))!=EOF && !endofline(fin,c); i++) {
    		if (i >= maxline-1) {	/* grow line */
    			maxline *= 2;		/* double current size */
    			newl = (char *) realloc(line, maxline);
    			if (newl == NULL) {
    				reset();
    				return NULL;
    			}
    			line = newl;
    			news = (char *) realloc(sline, maxline);
    			if (news == NULL) {
    				reset();
    				return NULL;
    			}
    			sline = news;
    
    
    		}
    		line[i] = c;
    	}
    	line[i] = '\0';
    	if (split() == NOMEM) {
    		reset();
    		return NULL;			/* out of memory */
    	}
    	return (c == EOF && i == 0) ? NULL : line;
    }
    
    /* split: split line into fields */
    static int split(void)
    {
    	char *p, **newf;
    	char *sepp; /* pointer to temporary separator character */
    	int sepc;   /* temporary separator character */
    
    	nfield = 0;
    	if (line[0] == '\0')
    		return 0;
    	strcpy(sline, line);
    	p = sline;
    
    	do {
    		if (nfield >= maxfield) {
    			maxfield *= 2;			/* double current size */
    			newf = (char **) realloc(field, 
    						maxfield * sizeof(field[0]));
    			if (newf == NULL)
    				return NOMEM;
    			field = newf;
    		}
    		if (*p == '"')
    			sepp = advquoted(++p);	/* skip initial quote */
    		else
    			sepp = p + strcspn(p, fieldsep);
    		sepc = sepp[0];
    		sepp[0] = '\0';				/* terminate field */
    		field[nfield++] = p;
    		p = sepp + 1;
    	} while (sepc == ',');
    
    	return nfield;
    }
    
    /* advquoted: quoted field; return pointer to next separator */
    static char *advquoted(char *p)
    {
    	int i, j;
    
    	for (i = j = 0; p[j] != '\0'; i++, j++) {
    		if (p[j] == '"' && p[++j] != '"') {
    			/* copy up to next separator or \0 */
    			int k = strcspn(p+j, fieldsep);
    			memmove(p+i, p+j, k);
    			i += k;
    			j += k;
    			break;
    		}
    		p[i] = p[j];
    	}
    	p[i] = '\0';
    	return p + j;
    }
    
    /* csvfield:  return pointer to n-th field */
    char *csvfield(int n)
    {
    	if (n < 0 || n >= nfield)
    		return NULL;
    	return field[n];
    }
    
    /* csvnfield:  return number of fields */ 
    int csvnfield(void)
    {
    	return nfield;
    }
    
    /* csvtest main: test CSV library */
    int main(void)
    {
    	int i;
    	char *line;
    
    	while ((line = csvgetline(stdin)) != NULL) {
    		printf("line = `%s'\n", line);
    		for (i = 0; i < csvnfield(); i++)
    			printf("field[%d] = `%s'\n", i, csvfield(i));
    	}
    	return 0;
    }


**分析：**我们的第二版本将程序分为三个文件，分别用于读入一个新的CSV行，返回当前行的第n
个数据域，返回当前行中数据域的个数。

这里最值得关注的是csvgetline 函数，它通过变成数组，可以处理无限长的数据行，和很多的数据域，并通过endofline 函数处理各种可能的结束情况，通过split 函数处理引号嵌套的情况，这样，我们的第二个版本就比第一版本健壮多了，也显得更像专业人士写的代码，但是，它还有以下问题：

* 使用了全局变量，无法适用于多线程的情况
* 数据的分割为逗号，不能适用于非逗号分割的情况
* 一次完成所有的切分，可以到要求的时候再做

#4.  最终版

基于上面的分析，我们实现了处理CSV文件库的最终版本，该版本通过结构体封装了全局数据，适用于多线程的情况

    /* Copyright (C) 1999 Lucent Technologies */
    /* Excerpted from 'The Practice of Programming' */
    /* by Brian W. Kernighan and Rob Pike */
    /* Modified by David Fifield for exercise 4-8 */
    
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <assert.h>
    
    enum { NOMEM = -2 };          /* out of memory signal */
    
    struct csv {
    	char *line;      /* input chars */
    	char *sline;     /* line copy used by split */
    	int  maxline;    /* size of line[] and sline[] */
    	char **field;    /* field pointers */
    	int  maxfield;   /* size of field[] */
    	int  nfield;     /* number of fields in field[] */
    };
    
    static char fieldsep[] = ","; /* field separator chars */
    
    static char *advquoted(char *);
    static int split(struct csv *);
    
    /* endofline: check for and consume \r, \n, \r\n, or EOF */
    static int endofline(FILE *fin, int c)
    {
    	int eol;
    
    	eol = (c=='\r' || c=='\n');
    	if (c == '\r') {
    		c = getc(fin);
    		if (c != '\n' && c != EOF)
    			ungetc(c, fin);	/* read too far; put c back */
    	}
    	return eol;
    }
    
    /* reset: set variables back to starting values */
    static void reset(struct csv *csv)
    {
    	free(csv->line);	/* free(NULL) permitted by ANSI C */
    	free(csv->sline);
    	free(csv->field);
    	csv->line = NULL;
    	csv->sline = NULL;
    	csv->field = NULL;
    	csv->maxline = csv->maxfield = csv->nfield = 0;
    }
    
    /* csvnew:  initialize a struct csv */
    void csvnew(struct csv *csv)
    {
    	csv->line = NULL;
    	csv->sline = NULL;
    	csv->maxline = 0;
    	csv->field = NULL;
    	csv->maxfield = 0;
    	csv->nfield = 0;
    }
    
    /* csvgetline:  get one line, grow as needed */
    /* sample input: "LU",86.25,"11/4/1998","2:19PM",+4.0625 */
    char *csvgetline(struct csv *csv, FILE *fin)
    {	
    	int i, c;
    	char *newl, *news;
    
    	if (csv->line == NULL) {			/* allocate on first call */
    		csv->maxline = csv->maxfield = 1;
    		csv->line = (char *) malloc(csv->maxline);
    		csv->sline = (char *) malloc(csv->maxline);
    		csv->field =
                (char **) malloc(csv->maxfield*sizeof(csv->field[0]));
    		if (csv->line == NULL ||
               csv->sline == NULL || csv->field == NULL) {
    			reset(csv);
    			return NULL;		/* out of memory */
    		}
    	}
    	for (i=0; (c=getc(fin))!=EOF && !endofline(fin,c); i++) {
    		if (i >= csv->maxline-1) {	/* grow line */
    			csv->maxline *= 2;		/* double current size */
    			newl = (char *) realloc(csv->line, csv->maxline);
    			if (newl == NULL) {
    				reset(csv);
    				return NULL;
    			}
    			csv->line = newl;
    			news = (char *) realloc(csv->sline, csv->maxline);
    			if (news == NULL) {
    				reset(csv);
    				return NULL;
    			}
    			csv->sline = news;
    
    
    		}
    		csv->line[i] = c;
    	}
    	csv->line[i] = '\0';
    	if (split(csv) == NOMEM) {
    		reset(csv);
    		return NULL;			/* out of memory */
    	}
    	return (c == EOF && i == 0) ? NULL : csv->line;
    }
    
    /* split: split line into fields */
    static int split(struct csv *csv)
    {
    	char *p, **newf;
    	char *sepp; /* pointer to temporary separator character */
    	int sepc;   /* temporary separator character */
    
    	csv->nfield = 0;
    	if (csv->line[0] == '\0')
    		return 0;
    	strcpy(csv->sline, csv->line);
    	p = csv->sline;
    
    	do {
    		if (csv->nfield >= csv->maxfield) {
    			csv->maxfield *= 2;			/* double current size */
    			newf = (char **) realloc(csv->field, 
    						csv->maxfield * sizeof(csv->field[0]));
    			if (newf == NULL)
    				return NOMEM;
    			csv->field = newf;
    		}
    		if (*p == '"')
    			sepp = advquoted(++p);	/* skip initial quote */
    		else
    			sepp = p + strcspn(p, fieldsep);
    		sepc = sepp[0];
    		sepp[0] = '\0';				/* terminate field */
    		csv->field[csv->nfield++] = p;
    		p = sepp + 1;
    	} while (sepc == ',');
    
    	return csv->nfield;
    }
    
    /* advquoted: quoted field; return pointer to next separator */
    static char *advquoted(char *p)
    {
    	int i, j;
    
    	for (i = j = 0; p[j] != '\0'; i++, j++) {
    		if (p[j] == '"' && p[++j] != '"') {
    			/* copy up to next separator or \0 */
    			int k = strcspn(p+j, fieldsep);
    			memmove(p+i, p+j, k);
    			i += k;
    			j += k;
    			break;
    		}
    		p[i] = p[j];
    	}
    	p[i] = '\0';
    	return p + j;
    }
    
    /* csvfield:  return pointer to n-th field */
    char *csvfield(struct csv *csv, int n)
    {
    	if (n < 0 || n >= csv->nfield)
    		return NULL;
    	return csv->field[n];
    }
    
    /* csvnfield:  return number of fields */ 
    int csvnfield(struct csv *csv)
    {
    	return csv->nfield;
    }
    
    /* csvtest main: test CSV library */
    int main(void)
    {
    	int i;
    	char *line;
    	struct csv csv;
    
    	csvnew(&csv);
    	while ((line = csvgetline(&csv, stdin)) != NULL) {
    		printf("line = `%s'\n", line);
    		for (i = 0; i < csvnfield(&csv); i++)
    			printf("field[%d] = `%s'\n", i, csvfield(&csv, i));
    	}
    	return 0;
    }


**分析:**与上一个版本相比，最终版只是将全局数据封装为一个结构体，对与所有要用到全局变量的函数，只需要将结构体指针传给该函数即可。

#5.  C++ 版

既然上面的最终版本使用结构体来封装数据，那么用C++
必然可以封装得更加彻底。而且，通过C++
提供的string,大大减轻了我们处理字符串的负担。程序如下：

    /* Copyright (C) 1999 Lucent Technologies */
    /* Excerpted from 'The Practice of Programming' */
    /* by Brian W. Kernighan and Rob Pike */
    
    #include <iostream>
    #include <algorithm>
    #include <string>
    #include <vector>
    
    using namespace std;
    
    class Csv {	// read and parse comma-separated values
    	// sample input: "LU",86.25,"11/4/1998","2:19PM",+4.0625
    
      public:
    	Csv(istream& fin = cin, string sep = ",") : 
    		fin(fin), fieldsep(sep) {}
    
    	int getline(string&);
    	string getfield(int n);
    	int getnfield() const { return nfield; }
    
      private:
    	istream& fin;			// input file pointer
    	string line;			// input line
    	vector<string> field;	// field strings
    	int nfield;				// number of fields
    	string fieldsep;		// separator characters
    
    	int split();
    	int endofline(char);
    	int advplain(const string& line, string& fld, int);
    	int advquoted(const string& line, string& fld, int);
    };
    
    // endofline: check for and consume \r, \n, \r\n, or EOF
    int Csv::endofline(char c)
    {
    	int eol;
    
    	eol = (c=='\r' || c=='\n');
    	if (c == '\r') {
    		fin.get(c);
    		if (!fin.eof() && c != '\n')
    			fin.putback(c);	// read too far
    	}
    	return eol;
    }
    
    // getline: get one line, grow as needed
    int Csv::getline(string& str)
    {	
    	char c;
    
    	for (line = ""; fin.get(c) && !endofline(c); )
    		line += c;
    	split();
    	str = line;
    	return !fin.eof();
    }
    
    // split: split line into fields
    int Csv::split()
    {
    	string fld;
    	int i, j;
    
    	nfield = 0;
    	if (line.length() == 0)
    		return 0;
    	i = 0;
    
    	do {
    		if (i < line.length() && line[i] == '"')
    			j = advquoted(line, fld, ++i);	// skip quote
    		else
    			j = advplain(line, fld, i);
    		if (nfield >= field.size())
    			field.push_back(fld);
    		else
    			field[nfield] = fld;
    		nfield++;
    		i = j + 1;
    	} while (j < line.length());
    
    	return nfield;
    }
    
    // advquoted: quoted field; return index of next separator
    int Csv::advquoted(const string& s, string& fld, int i)
    {
    	int j;
    
    	fld = "";
    	for (j = i; j < s.length(); j++) {
    		if (s[j] == '"' && s[++j] != '"') {
    			int k = s.find_first_of(fieldsep, j);
    			if (k > s.length())	// no separator found
    				k = s.length();
    			for (k -= j; k-- > 0; )
    				fld += s[j++];
    			break;
    		}
    		fld += s[j];
    	}
    	return j;
    }
    
    // advplain: unquoted field; return index of next separator
    int Csv::advplain(const string& s, string& fld, int i)
    {
    	int j;
    
    	j = s.find_first_of(fieldsep, i); // look for separator
    	if (j > s.length())               // none found
    		j = s.length();
    	fld = string(s, i, j-i);
    	return j;
    }
    
    
    // getfield: return n-th field
    string Csv::getfield(int n)
    {
    	if (n < 0 || n >= nfield)
    		return "";
    	else
    		return field[n];
    }
    
    // Csvtest main: test Csv class
    int main(void)
    {
    	string line;
    	Csv csv;
    
    	while (csv.getline(line) != 0) {
    		cout << "line = `" << line <<"'\n";
    		for (int i = 0; i < csv.getnfield(); i++)
    			cout << "field[" << i << "] = `"
    				 << csv.getfield(i) << "'\n";
    	}
    	return 0;
    }
    

#6.  总结

这个例子充分证明了这本书的定位，即实践编程，作者通过一个真实的问题，一步步改进，在改进的同时，不断解释和讨论做这种改进的原因，看书时有种与作者讨论问题解法的错觉，作者还给出了完整的代码，便于新手学习、参考。

《[程序设计实践][1]》是本好书，可惜内容太少，推荐每位同学读一下，作者很牛逼，作者还写了几本有名的书，一本是《C程序设计语言》，没错，就是传说中的C语言圣经，想必大家都听说过其大名，另一本牛逼的书是《[unix编程环境][3]》，准备把《[unix编程环境][3]》学习一下。

[1]: http://book.douban.com/subject/1173548/
[2]: http://baike.baidu.com/view/468993.htm
[3]: http://book.douban.com/subject/1033144/
