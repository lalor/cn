---
layout: post
title: "二叉树的层次遍历"
description: ""
category: 算法
tags: [C语言, 算法]
---

# 问题定义

给定一棵二叉树，要求按分层遍历该二叉树，即从上到下按层次访问该二叉树(每一层将单独输出一行)，每一层要求访问的顺序为从左到右，并将节点依次编号。下面是一个例子:

![img1][img1]

输出:

	1
	2 3
	4 5 6
	7 8

节点的定义:

	structNode {
	    Node *pLeft;
	    Node *pRight;
	    intdata;
	};


《编程之美》书上提供了两种解法，可以参考[这里](http://www.cnblogs.com/miloyip/archive/2010/05/12/binary_tree_traversal.html)。个人觉得编程之美对这个题目的分析不是很让人满意，这题有个简单而又有效的算法，就是图的广度优先搜索，那么我们需要用到一个队列，《编程之美》用到了一个vector，然后再用两个游标，实在是不直观，且浪费存储空间，如果用队列，则空间复杂度可以降低一半O(N/2)。

该题的一个小难点就在于要分层输出，如果不需要分层的话，则一个普通的广度优先模板就可以解决这个问题了，书中最后提到了叶劲峰编写的一个算法，其主要特点是在队列中每一层节点之后插入一个傀儡节点，当我们到达一个傀儡节点时，就知道我们已经遍历了一层，要开始新的一层，这时候需要换行了。

基于独立思考，我想到了一个差不多的方法，可能实现上更简单一点（我相信网络上早已有人想到了，不过我自己想到的，是我自己的收获，特记录之）我们可以在遍历当前层的时候，保存下一层的节点数，只需要每次插入一个节点的时候childSize++即可，这样我们就知道下一层有几个节点了，然后将childSize赋值给parentSize，开始新的一层遍历，从队列中取出parentSize个节点以后，也就知道这一层遍历完了。

由于这是二叉树，所以一开始的时候parentSize = 1, childSize = 0。

核心代码如下：

	void PrintNodeByLevel(Node *root)
	{
	    int parentSize = 1, childSize = 0;
	    Node * temp;
	    queue<Node *> q;
	    q.push(root);

	    do
	    {
	        temp = q.front();
	        cout << temp->data << "  ";
	       	q.pop();

	        	if (temp->pLeft != NULL)
	        	{
	        	    q.push(temp->pLeft);
	        	    childSize ++;
	        	}
	        	if (temp->pRight != NULL)
	        	{
	        	    q.push(temp->pRight);
	        	    childSize ++;
	        	}

	        	parentSize--;
	        	if (parentSize == 0)
	        	{
	        	    parentSize = childSize;
	        	    childSize = 0;
	        	    cout << endl;
	      }

	    } while (!q.empty());
	}



[img1]: /cn/image/traverse-binary-by-level.png
