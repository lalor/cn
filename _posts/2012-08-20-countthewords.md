---
layout: post
title: "几种统计单词出现次数的方法"
description: ""
category: 算法
tags: [algorithm, STL, hash, shell]
---

##问题定义

今天通过[某热心童鞋][1]的帮助，顺利实现了代码高亮功能，便决定写篇文章测试效果，想来想去就想到了这个问题：写一个程序，统计一个文本文件中每个单词出现的次数。

这个问题虽然简单，但是在《[C程序设计语言][2]》《[编程珠玑][3]》《[编程珠玑(续)][4]》都有讨论，可见这是一个非常经典且具有代表性的题目，这个问题虽然不难，但是要你用不同的方法来实现，并且比较各个方法的优劣，是不是就没那么简单了？用不同的方法来实现此功能，有利于扩展我们的解题思路。


##使用二叉查找树实现

这个程序参考《[C程序设计语言][2]》，其基本思路就是一个单词为一个节点，比较新来的单词与当前节点，如果与当前节点一样，当前节点的count就加1，如果小于当前节点，就递归调用addtree函数，将单词插入到当前节点的右边，否则，就插入当前节点的左边。这是一种比较容易想到的方法，只要注意细节，也不难实现，下面的代码是《[C程序设计语言][2]》里的程序，读者可以比较自己写的代码和大师写的有什么不同，注意这里同样没有处理出错的情况，如内存不足。


	#include <stdio.h>  
	#include <string.h>  
	#include <stdlib.h>  
	
	struct tnode  /*  the tree node  */
	{  
		char *word;  /* points to the text*/
		int count;   /* number of occurrences */
		struct tnode* left; /*  left child */ 
		struct tnode* right; /*  right child */
	};  
	
	#define MAXWORD 100
	struct tnode *addtree(struct tnode *, char *);
	void treeprint(struct tnode *);
	/*  省略了,可以自己去《C 程序设计语言》上找 */
	int getword(char *, int);
	
	int main(int argc, char const* argv[])
	{
		struct tnode* root;
		char word[MAXWORD];
	
		FILE *fp = NULL;  
		root = NULL;
	
		fp = fopen("source.txt","r");  
		if ( fp == NULL )  
		{  
			printf("文件打开错误");  
			return 1;  
		}  
	
		while( fscanf(fp, "%s", word) != EOF )  
		{  
			root = addtree(root, word);
		}  
	
		treeprint(root);
		return 0;
	}
	
	struct tnode *talloc(void);
	char *strdup(char *);
	
	struct tnode* addtree(struct tnode *p, char *w)
	{
		int cond;
		if (p == NULL) 
		{
			p = talloc(); /* a new word has arrived */
			p->word = strdup(w); /*  make a new node */
			p->count = 1;
			p->left = p->right = NULL;
		}
		else if ( (cond = strcmp(w, p->word)) == 0 ) 
		{
			p->count++;/*  repeated word */
		}
		else if (cond < 0) 
		{
			p->left = addtree(p->left, w);
			/*  less than into left subtree */
		}
		else
		{
			p->right = addtree(p->right, w); 
			/*  greater than into right subtree */
		}
		return p;
	}
	
	/*  in-order print of tree p */
	void treeprint(struct tnode *p)
	{
		if (p != NULL) 
		{
			treeprint(p->left);
			printf("%4d %s\n", p->count, p->word);
			treeprint(p->right);
		}
	}
	
	struct tnode* talloc()
	{
		return (struct tnode *)malloc(sizeof(struct tnode));
	}
	
	char *strdup(char *s)
	{
		char *p;
	
		p = (char *)malloc(strlen(s) + 1); /*  +1 for '\0' */
		if (p != NULL) 
		{
			strcpy(p, s); 
		}
		return p;
	}


##使用hash 表实现

上面的二叉查找树是一种很不错的实现方法，但是并不意味着我们没有更好的办法，而且二叉查找树有一个隐含的缺陷就是当数据到来得不是很随机的时候，二叉树就趋向于不平衡，导致程序运行效率会随之恶化。

下面的代码使用hash表实现,参考《[编程珠玑][3]》采用链式法解决hash冲突,还是一样,程序并不难,但是大师写的程序确实不一样,简洁高效。见程序,几乎不用注释,认真看都能看懂。

	#include <stdio.h>  
	#include <string.h>  
	#include <stdlib.h>  
	
	typedef struct node *nodeptr;
	typedef struct node
	{
		char *word;
		int count;
		nodeptr next;
	}node;
	
	#define MAXWORD 100 
	#define NHASH 29989 /*  the size of hash table */
	#define MULT 31
	nodeptr bin[NHASH];
	
	/*  calculate the hash value */
	unsigned int hash(char *p)
	{
		unsigned int h = 0;
		for (  ; *p	; p++) 
		{
			h = MULT * h + *p;
		}
		return h % NHASH;
	}
	
	void incword(char *s)
	{
		unsigned int h = hash(s);
		nodeptr p;
		for (p = bin[h]; p != NULL; p = p->next	) 
		{
			if (strcmp(s, p->word) == 0) 
				/*  has repeated word */
			{
				p->count++;
				return;
			}
		}
	
		p = (nodeptr)malloc(sizeof(struct node));
		/*  new a node */
		p->count = 1;
		p->word = (char*)malloc(strlen(s) + 1);
		strcpy(p->word, s);
		p->next = bin[h];
		bin[h] = p;
	}
	
	int main(int argc, char const* argv[])
	{
		int i;
		/*  initialized hash table */
		for (i = 0; i < NHASH; i++) 
		{
			bin[i] = NULL;
		}
	
		char buf[MAXWORD];
		FILE *fp = NULL;  
	
		fp = fopen("source.txt","r");  
		if ( fp == NULL )  
		{  
			printf("文件打开错误\n");  
			return 1;  
		}  
	
		while (fscanf(fp, "%s", buf) != EOF) 
		{/*  incword words */
			incword(buf);
		}
	
		/*  print words and it's count */
		for (i = 0; i < NHASH; i++) 
		{
			nodeptr p;
			for ( p = bin[i]; p != NULL; p = p->next) 
			{
				printf("%4d %s\n", p->count, p->word);
			}
		}
	}


##C+＋标准库

上面的程序虽然高效,但是开发效率却不够高,我们可以考虑使用C++标准库,这题很明显,我们需要一个map容器（**注：**这里为了统一,我们采用C++处理文件的输入,如果采用C语言的方式处理输入输出,那么效率并不会比上面两个程序慢多少）

	#include <iostream>
	#include <iterator>
	#include <map>
	#include <string>
	#include <fstream>
	using namespace std;
	
	int main(int argc, char* argv[])
	{
		map<string, int> M;	
		map<string, int>::iterator j;
		string t;
		ifstream in("source.txt");
		while (in >> t) 
		{
			M[t]++;
		}
	
		for (j = M.begin(); j != M.end(); ++j) 
		{
			cout << j->second << " " << j->first << endl;
		}
		return 0;
	}


如果你对标准库很熟悉的,你可能会写出下面这样的代码。这里有两点要说明：
- 第一,程序应该清晰明了,不应该故意写得别人看不懂。
- 第二,正式工作中,为了提高开发效率,STL应用非常广泛

所以我们讨论下面这段代码还是有意义的,为了演示 foreach
的用法,我故意这么输出。关于foreach 的详细资料,请参考STL。

	#include <algorithm>  
	#include <iostream>  
	#include <iterator>
	#include <fstream>  
	#include <string>  
	#include <map>  
	
	using namespace std;  
	  
	map<string , int> M;  
	void print( const pair<string, int> &r)  
	{  
	        cout << r.second << " " << r.first << endl;  
	}  
	
	void record( const string &s)  
	{  
	        M[ s ] ++;  
	}  
	
	int main()  
	{  
	    fstream in("source.txt");  
	    istream_iterator<string> it(in);  
	    istream_iterator<string> eos;  
	    for_each(it,eos,record);  
	    for_each( M.begin(), M.end(), print);  
	  
	    return 0;  
	}


##shell 脚本

虽然shell脚本是所有方法里面最慢的一个,但是确是实现起来最快的一种方法,问题只是说找出单词出现次数,并没有说具体是什么应用,可能只是想看一下哪些单词出现得比较多呢。而且shell脚本是最好扩展的,我们可以快速的通过shell按出现次数降序输出单词,也可以快速的找出哪些单词出现得比较多,哪些单词出现的比较少,还可以计算总共有多少单词等。详细内容可以参考[linux命令行工具妙用][5]

	cat source.txt | tr -cs a-zA-Z '\n' | sort | uniq -c


[1]: http://www.heiniuhaha.com/ "某天使姐姐"
[2]: http://book.douban.com/subject/1139336/ "C程序设计语言"
[3]: http://book.douban.com/subject/3227098/ "编程珠玑"
[4]: http://book.douban.com/subject/6124333/ "编程珠玑(续)"
[5]: http://mingxinglai.com/cn/2012/08/toos-of-bash/ "linux 命令行工具妙用"
