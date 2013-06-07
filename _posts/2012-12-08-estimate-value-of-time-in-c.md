---
layout: post
title: "C/C++中单个操作的时间估计"
description: ""
category: 程序语言
tags: [时间估计]
---

*闲来无事，我也来在自己的电脑上估计一下单个操作的执行时间。*

为了估计单个操作的运行时间，让每个语句执行1000000000(10^9 )次，例如，执行两个整数相加`i1=i2+i3` 10^9 次，然后获取执行该操作10^9 次所需要的执行时间，执行时间以秒为单位，那么，都不用思考，单个操作的执行时间的单位就是纳秒。

**这里要说明两点：**

* 其一，程序不要优化，优化过就不准了；
* 其二，每台电脑的执行速度不一样，得到的结果也不一样，但是总的来说，各个操作的执行时间还是成比例的。

为了阅读方便，首先分析结果，然后给出结果，最后再给出我的代码，如果你有兴趣，也可以把代码拿去试试，挺有趣的噢。

#1.结果分析

**从执行结果中可以看到：**

1. 移位操作并没有比乘法操作快很多，所以，为了程序清晰可见，我们还是用乘法吧
1. 整数操作速度非常快，但是除法和取模操作就比较耗时间了
1. 浮点运算也很快有没有？对于多年来都习惯性的接受，浮点运算比整数运算代价昂贵的说法的你，是不是很意外
1. 函数调用也还比较快
1. 但是，输入输出就很慢很慢很慢了
1. `sscanf` 和 `sprintf` 你以后还敢随便用吗
1. `atoi`和`atof`代价都很高
1.  函数调用代价也很大，所以，当你使用数学函数的时候，最好多留一个心眼。比如，你要比较两个点哪个离原点更近，你可能会写出 `if ( sqrt(dx1 * dx1 + dy1 * dy1) < sqrt(dx2 * dx2 + dy2 * dy2))`,如果你多留一个心眼，你就会这么写 `if ( (dx1 * dx1 + dy1 * dy1) < (dx2 * dx2 + dy2 * dy2))`，这两种表达方式的结果一样有没有，后者能节约很多时间有没有，尤其是在这个语句经常被执行的情况下，能够节省很多很多的时间对不对。

#2.输出结果

我的电脑是华硕U31S，4核的处理器，i5,2.3GHz,其实在运行得慢的电脑上做测试更直观。

**Bit Operations:**  
i1 > > 1      		***2.39***   
i1 < < 1	    	***2.41***   
i1 > > 3	    	***2.41***   

**Int Operations:**   
i1++		        ***2.58***   
i1 = i2 + i3		***2.2***   
i1 = i2 - i3		***2.25***   
i1 = i2 * i3		***2.26***   
i1 = i2 / i3		***4.9***   
i1 = i2 % i3		***4.91***   

**Float Operations:**  
f1 = f2 + f3		***2.27***   
f1 = f2 - f3		***2.27***   
f1 = f2 * f3		***2.36***   
f1 = f2 / f3		***8.4***   

**Double Operations:**  
d1 = d2 + d3		***2.39***   
d1 = d2 - d3		***2.35***   
d1 = d2 * d3		***2.46***   
d1 = d2 / d3		***8.44***   

**Numeric Conversions:**  
i1=(int)f1		    ***5.61***   
i1=(int)d1		    ***5.61***   
f1=(float)i1		***2.18***   

**Math Functions:**  
i1 = rand()		    ***15.59***   
f1 = log(f2)		***27.79***   
f1 = exp(f2)		***18.8***   
f1 = sin(f2)		***40.9***   
f1 = cos(f2)		***35.77***   
f1 = sqrt(f2)		***8.55***   

**Fun Call:**  
fun1()		        ***2.47***   
i1 = fun2()		    ***2.82***   
i1 = fun3(i1)		***2.82***   
i1 = fun4(i1,i2,i3)	***3.17***   
if ( i1 == 5)		***2.57***   
if ( i1 > 5)		***2.53***   
if ( i1 != 5)		***2.26***   

**Malloc:**  
free(malloc(8))		***32.33***   

**String Functinos:**  
i1 = strlen(str1)       	***4.58***   
strcpy(str2, str1)	        ***8.18***   
i1 = strcmp(str2, str1)		***8.17***   

**String/Number Conversions:**  
i1 = atoi("12345")		    ***33.88***   
sscanf("12345", "%d", &i1)	***238.12***   
sprintf(s, "%d", i)		    ***146.52***   
f1 = atof("123.45")		    ***167.94***   
sscanf("123.45", "%f", &f1)	***367.52***   
sprintf(s, "%6.2f", 123.45)	***470.47***   

**Input/Ouput:**  
fprintf(fp, "%d\n", i1)		***113.59***   
fscanf(fp, "%d", &i1)		***307.73***   
fputs("hello", fp)		    ***32.14***   
fgets(s, 100, fp)		    ***281.46***   

#3.测试代码

测试代码很帅有没有，学计算机真好，所有重复劳动都让计算机帮我们搞定。

    #include <iostream>
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    #include <cmath>
    #include <cstring>
    using namespace std;
    
    const int n = 1000000000; //10^9
    
    #define EXPR(CODE) {    \
        begin = clock();    \
        for (i = 0; i < n; i++) { CODE; }    \
        cout << #CODE << "\t\t" << \
        (clock() - begin) / CLOCKS_PER_SEC << endl;    \
    }
    
    void fun1() { }
    int fun2() { return 0; }
    int fun3(int i) { return 0; }
    int fun4(int a, int b, int c){ return 0; }
    
    
    int main(int argc, char* argv[])
    {
        int i = 0;
    
        int i1, i2, i3;
        float f1, f2, f3;
        double d1, d2, d3;
    
        i1 = i2 = i3 = 5000;
        f1 = f2 = f3 = 5000.0;
        d1 = d2 = d3 = 5000.0;
    
        double begin;
    
        cout << "\nBit Operations:" << endl;
        EXPR(i1>>1);
        EXPR(i1<<1);
        EXPR(i1>>3);
    
        cout << "\nInt Operations:" << endl;
        EXPR(i1++);
        EXPR(i1 = i2 + i3);
        EXPR(i1 = i2 - i3);
        EXPR(i1 = i2 * i3);
        EXPR(i1 = i2 / i3);
        EXPR(i1 = i2 % i3);
    
    
        cout << "\nFloat Operations:" << endl;
        EXPR(f1 = f2 + f3);
        EXPR(f1 = f2 - f3);
        EXPR(f1 = f2 * f3);
        EXPR(f1 = f2 / f3);
    
    
        cout << "\nDouble Operations:" << endl;
        EXPR(d1 = d2 + d3);
        EXPR(d1 = d2 - d3);
        EXPR(d1 = d2 * d3);
        EXPR(d1 = d2 / d3);
    
    
        cout << "\nNumeric Conversions:" << endl;
        EXPR(i1=(int)f1);
        EXPR(i1=(int)d1);
        EXPR(f1=(float)i1);
    
    
        cout << "\nMath Functions:" << endl;
        EXPR(i1 = rand());
        EXPR(f1 = log(f2));
        EXPR(f1 = exp(f2));
        EXPR(f1 = sin(f2));
        EXPR(f1 = cos(f2));
        EXPR(f1 = sqrt(f2));
    
    
        cout << "\nFun Call:" << endl;
        EXPR(fun1());
        EXPR(i1 = fun2());
        EXPR(i1 = fun3(i1));
        EXPR(i1 = fun4(i1, i2, i3));
    
        EXPR(if ( i1 == 5));
        EXPR(if ( i1 > 5));
        EXPR(if ( i1 != 5));
    
        cout << "\nMalloc:" << endl;
        EXPR(free(malloc(8)));
    
        char *str1 = "1234567890";
        char *str2;
        str2 = (char*)malloc(strlen(str1) + 1);
        strcpy(str2, str1);
    
        cout << "\nString Functinos:" << endl;
        EXPR(i1 = strlen(str1));
        EXPR(strcpy(str2, str1));
        EXPR(i1 = strcmp(str2, str1));
    
        char s[100];
        cout << "\nString/Number Conversions:" << endl;
    
        EXPR(i1 = atoi("12345"));
        EXPR(sscanf("12345", "%d", &i1));
        EXPR(sprintf(s, "%d", i));
    
        EXPR(f1 = atof("123.45"));
        EXPR(sscanf("123.45", "%f", &f1));
        EXPR(sprintf(s, "%6.2f", 123.45));
    
    
        cout << "\nInput/Ouput:" << endl;
    
        FILE *fp = fopen("source1", "w+");
    
        if (fp == NULL) { return -1; }
        EXPR(fprintf(fp, "%d\n", i1));
        EXPR(fscanf(fp, "%d", &i1));
    
        fclose(fp);
    
        fp = fopen("source2", "w+");
        if (fp == NULL) { return -1; }
    
        EXPR(fputs("hello", fp));
        EXPR(fgets(s, 100, fp));
    
        fclose(fp);
    
        return 0;
    }


完。转载请注明出处http://mingxinglai.com
