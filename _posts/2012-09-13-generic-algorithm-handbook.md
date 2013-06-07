---
layout: post
title: "C+＋泛型算法手册"
description: ""
category: 程序语言
tags: [STL, Handbook]
---

Each generic algorithm, with the fistful of exceptions that make the rule, begins with a pair of iterators that marks the range of elements within the container over which to traverse. The range begins with the first iterator and ends with but does not include the second.

The algorithms are generally overloaded to support two versions: one that uses either the built-in equality or the less-than operator of the underlying element type, and a second one that accepts either a function object or a pointer to function providing an alternative implementation of that operator. For example, by default `sort()` orders the container elements using the less-than operator. To override that, we can pass in the predefined greater-than function object.

Other algorithms, however, are separated into two uniquely named instances; the predicate instance in each case ends with the suffix \_if, as in `find_if().`

Many of the algorithms that modify the container they are applied to come in two flavors: an in-place version that changes the container, and a version that returns a copy of the container with the changes applied. For example, there is both a `replace()` and a `replace_copy()` algorithm. The copy version always contains \_copy in its name. It accepts a third iterator that points to the first element of the container in which to copy the modified elements. By default, the copy is achieved by assignment. We can use one of three inserter adapters to override the assignment semantics and have the elements instead inserted. 

As programmers, we must quickly be able to look up which algorithms are available and how they are generally used. That is the purpose of this handbook.  Each listing provides a brief description of the algorithm, indicates the header file that must be included (either algorithm or numeric), and provides one or two usage examples.

<table>
<tr align="center">
<td align="left"><a href="#accumulate">accumulate()</a></td>
<td align="left"><a href="#adjacent_difference">adjacent_difference()</a></td>
<td align="left"><a href="#adjacent_find">adjacent_find()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#binary_search">binary_search()</a></td>
<td align="left"><a href="#count">count()</a></td>
<td align="left"><a href="#count_if">count_if()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#equal">equal()</a></td>
<td align="left"><a href="#fill">fill()</a></td>
<td align="left"><a href="#fill_n">fill_n()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#find">find()</a></td>
<td align="left"><a href="#find_end">find_end()</a></td>
<td align="left"><a href="#find_first_of">find_first_of()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#find_if">find_if()</a></td>
<td align="left"><a href="#for_each">for_each()</a></td>
<td align="left"><a href="#generate">generate()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#generate_n">generate_n()</a></td>
<td align="left"><a href="#includes">includes()</a></td>
<td align="left"><a href="#inner_product">inner_product()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#inplace_merge">inplace_merge()</a></td>
<td align="left"><a href="#iter_swap">iter_swap()</a></td>
<td align="left"><a href="#lexicographical_compare">lexicographical_compare()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#max">max(), min()</a></td>
<td align="left"><a href="#max_element">max_element() , min_element()</a></td>
<td align="left"><a href="#merge">merge()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#nth_element">nth_element()</a></td>
<td align="left"><a href="#partial_sort">partial_sort(), partial_sort_copy()</a></td>
<td align="left"><a href="#partial_sum">partial_sum()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#partition">partition(), stable_partition()</a></td>
<td align="left"><a href="#random_shuffle">random_shuffle()</a></td>
<td align="left"><a href="#remove">remove(), remove_copy()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#remove_if">remove_if(), remove_copy_if()</a></td>
<td align="left"><a href="#replace">replace(), replace_copy()</a></td>
<td align="left"><a href="#replace_if">replace_if(), replace_copy_if()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#reverse">reverse(), reverse_copy()</a></td>
<td align="left"><a href="#rotate">rotate(), rotate_copy()</a></td>
<td align="left"><a href="#search">search()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#search_n">search_n()</a></td>
<td align="left"><a href="#set_difference">set_difference()</a></td>
<td align="left"><a href="#set_intersection">set_intersection()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#set_symmetric_difference">set_symmetric_difference()</a></td>
<td align="left"><a href="#set_union">set_union()</a></td>
<td align="left"><a href="#sort">sort(), stable_sort()</a></td>
</tr>
<tr align="center">
<td align="left"><a href="#transform">transform()</a></td>
<td align="left"><a href="#unique">unique(), unique_copy()</a></td>
<td align="left"><a href="#conclusion">CONCLUSION</a></td>
</tr>
</table>



<p><a name="accumulate"> <h1>accumulate()</h1> </a></p>
By default, adds the container elements to an initial value specified by the third argument. A binary operation can be passed in to override the default addition.

    #include <numeric> 
    
    iresult = accumulate( ia, ia+8, 0); 
    iresult = accumulate( ilist.begin(), ilist.end(), 
            0, plus<int>() ); 


<p><a name="adjacent_difference"><h1>adjacent_difference()</h1></a></p>
By default, creates a new sequence in which the value of each new element, other than the first one, represents the difference of the current and the preceding element. Given the sequence `{0,1,1,2,3,5,8}`, the new sequence is `{0,1,0,1,1,2,3}`. A binary operation can be passed in to override subtraction. For example, `times<int>` yields the sequence `{0,0,1,2,6,15,40}`. The third argument is an iterator that addresses the container into which to copy the results.

    #include <numeric> 
    
    adjacent_difference( ilist.begin(), ilist.end(),
            iresult.begin() ); 
    adjacent_difference( ilist.begin(), ilist.end(),
            iresult.begin(), times<int>() ); 


<p><a name="adjacent_find"> <h1>adjacent_find()</h1></a></p>
By default, looks for the first adjacent pair of duplicate elements. A binary operator can override the built-in equality operator. Returns an iterator that addresses the first element of the pair.

    #include <algorithm> 
    class TwiceOver { 
    public: 
       bool operator() ( int val1, int val2 ) 
            { return val1 == val2/2 ? true : false; } 
    }; 
    
    piter = adjacent_find( ia, ia+8 ); 
    iter  = adjacent_find( vec.begin(), vec.end(), TwiceOver() ); 


<p><a name="binary_search"><h1>binary_search()</h1></a></p>
Assumes that the container is sorted by the less-than operator. If the container is sorted by some other ordering relationship, the binary operator must be passed in. The algorithm returns true or false.

    #include <algorithm> 
    found_it = binary_search( ilist.begin(), ilist.end(), value ); 
    found_it = binary_search( vec.begin(), vec.end(), value, 
                              greater<int>() ); 


<p><a name="count"><h1>count()</h1></a></p>
Returns a count of the number of elements within the container equal to value.

    #include <algorithm> 
    cout << value << " occurs " 
         << count( svec.begin(), svec.end(), value ) 
         << " times in string vector.\n"; 


<p><a name="count_if"><h1>count_if()</h1></a></p>
Returns a count of the number of times the operator evaluated as true.

    #include <algorithm> 
    class Even { 
    public: 
       bool operator()( int val ){ return !( val%2 ); } 
    }; 
    
    ires = count_if( ia, ia+8, bind2nd(less<int>(),10) ); 
    lres = count_if( ilist.begin(), ilist.end(), Even() ); 


<p><a name="equal"><h1>equal()</h1></a></p>
Returns true if the two sequences are equal for the number of elements contained within the first container. By default, the equality operator is used. Alternatively, a binary function object or pointer to function can be supplied.

    #include <algorithm> 
    class EqualAndOdd
    { 
    public: 
        bool operator()( int v1, int v2 ) 
           {
               return ((v1 == v2) &amp&amp (v1 % 2));
           } 
    }; 
    
    int ia1[] = { 1,1,2,3,5,8,13 }; 
    int ia2[] = { 1,1,2,3,5,8,13,21,34}; 
    res = equal( ia1, ia1+7, ia2 ); // true 
    res = equal( ia1, ia1+7, ia2, equalAndOdd() ); // false 


<p><a name="fill"><h1>fill()</h1></a></p>
Assigns a copy of value to each element within the container.

    #include <algorithm> 
    fill( ivec.begin(), ivec.end(), value ); 


<p><a name="fill_n"><h1>fill_n()</h1></a></p>
Assigns a copy of value to count elements within the container.

    #include <algorithm> 
    fill_n( ia, count, value ); 
    fill_n( svec.begin(), count, string_value ); 

<p><a name="find"><h1>find()</h1></a></p>
The elements within the container are compared for equality with value. If a match is found, the search ends. `find()` returns an iterator to the element. If no match is found, `container.end()` is returned.

    #include <algorithm> 
    piter = find( ia, ia+8, value ); 
    iter  = find( svec.begin(), svec.end(), "rosebud" ); 


<p><a name="find_end"><h1>find_end()</h1></a></p>
This algorithm takes two iterator pairs. The first pair marks the container to be searched. The second pair marks a sequence to match against. The elements within the first container are compared for the last occurrence of the sequence using either the equality operator or the specified binary operation. If a match is found, an iterator addressing the first element of the matched sequence is returned; otherwise, the iterator marking the end of the first container is returned. For example, given the character sequence Mississippi and a second sequence ss, `find_end()` returns an iterator to the first s of the second ss sequence.

    #include <algorithm> 
    int ia[ 17 ] = { 7,3,3,7,6,5,8,7,2,1,3,7,6,3,8,4,3 }; 
    int seq[ 3 ] = { 3, 7, 6 }; 
    
    // found_it addresses ia[10] 
    found_it = find_end( ia, ia+17, seq, seq+3 ); 


<p><a name="find_first_of"><h1>find_first_of()</h1></a></p>
`find_first_of()` accepts two iterator pairs. The first pair marks the elements to search. The second pair marks a collection of elements to search for. For example, to find the first vowel in the character sequence synesthesia, we define our second sequence as aeiou. `find_first_of()` returns an iterator to the first instance of an element of the sequence of vowels, in this case pointing to the first e. If the first sequence does not contain any of the elements, an iterator that addresses the end of the first sequence is returned. An optional fifth parameter allows us to override the default equality operator with any binary predicate operation.

    #include <algorithm> 
    string s_array[] = { "Ee", "eE", "ee", "Oo", "oo", "ee" }; 
    string to_find[] = { "oo", "gg", "ee" }; 
    
    // returns first occurrence of "ee" -- &s_array[2] 
    found_it = find_first_of( s_array, s_array+6, 
                             to_find, to_find+3 ); 
     

<p><a name="find_if"><h1>find_if()</h1></a></p>
The elements within the container are compared for equality with the specified binary operation. If a match is found, the search ends. `find_if()` returns an iterator to the element. If no match is found, `container.end()` is returned.

    #include <algorithm> 
    find_if( vec.begin(), vec.end(), LessThanVal(ival) ); 


<p><a name="for_each"><h1>for_each()</h1></a></p>
`for_each()` takes a third parameter that represents an operation that is applied to each element in turn. The operation cannot modify the elements (we can use `transform()` for that). Although the operation may return a value, that value is ignored.

    #include <algorithm> 
    template <typename Type> 
        void print_elements( Type elem ) { cout << elem << " "; } 
    
    for_each( ivec.begin(), ivec.end(), print_elements ); 


<p><a name="generate"><h1>generate()</h1></a></p>
`generate()` fills a sequence by applying the specified generator.

    #include <algorithm> 
    class GenByTwo { 
    public: 
       void operator()(){ 
          static int seed = -1; return seed += 2; } 
    }; 
    list<int> ilist( 10 ); 
    
    // fills ilist: 1 3 5 7 9 11 13 15 17 19 
    generate( ilist.begin(), ilist.end(), GenByTwo() ); 


<p><a name="generate_n"><h1>generate_n()</h1></a></p>
`generate_n()` fills a sequence by applying n successive invocations of the generator.

    #include <algorithm> 
    class gen_by_two { 
    public: 
       gen_by_two( int seed = 0 ) : _seed( seed ){} 
       int operator()() { return _seed += 2; } 
    private: 
       int _seed; 
    }; 
    vector<int> ivec( 10 ); 
    
    // fills ivec: 102 104 106 108 110 112 114 116 118 120 
    generate_n( ivec.begin(), ivec.size(), gen_by_two(100) ); 


<p><a name="includes"><h1>includes()</h1></a></p>
`includes()` returns true if every element of the second sequence is contained within the first sequence; otherwise, it returns false. Both sequences must be sorted, either by the default less-than operator or by the same operation passed as an optional fifth parameter.

    #include <algorithm> 
    int ia1[] = { 13, 1, 21, 2, 0, 34, 5, 1, 8, 3, 21, 34 }; 
    int ia2[] = { 21, 2, 8, 3, 5, 1 }; 
    
    // includes must be passed sorted containers 
    sort( ia1, ia1+12 ); sort( ia2, ia2+6 ); 
    res = includes( ia1, ia1+12, ia2, ia2+6 ); // true 


<p><a name="inner_product"><h1>inner_product()</h1></a></p>
`inner_product()` accumulates the product of two sequences of values, adding them in turn to a user-specified initial value. For example, given the two sequences `{2,3,5,8}` and `{1,2,3,4}`, the result is the sum of the product pairs (2\*1)+(3\*2)+(5\*3)+(8\*4). If we provide an initial value of 0, the result is 55.

A second version allows us to override the default addition operation and the default multiply operation. For example, if we use the same sequence but specify subtraction and addition, the result is the difference of the following addition pairs: (2+1),(3+2),(5+3),(8+4). If we provide an initial value of 0, the result is -28.

    #include <numeric> 
    int ia[] =  { 2, 3, 5, 8 }; 
    int ia2[] = { 1, 2, 3, 4 }; 
    int res = inner_product( ia, ia+4, ia2, 0); 
    
    vector<int> vec(  ia,  ia+4 ); 
    vector<int> vec2( ia2, ia2+4 ); 
    
    res = inner_product( vec.begin(), vec.end(), vec2.begin(), 0, 
                        minus<int>(), plus<int>() ); 


<p><a name="inplace_merge"><h1>inplace_merge()</h1></a></p>
`inplace_merge()` takes three iterator parameters: first, middle, and last. Two input sequences are marked by \[first,middle\] and \[middle,last\] (middle marks 1 past the last element of the first sequence). These sequences must be consecutive. The resulting sequence overwrites the two ranges beginning at first. An optional fourth parameter allows us to specify an ordering operation other than the default less-than operator.

    #include <algorithm> 
    int ia[20] = { 29,23,20,17,15,26,51,12,35,40, 
                   74,16,54,21,44,62,10,41,65,71 }; 
    
    int *middle = ia+10, *last = ia+20; 
    
    // 12 15 17 20 23 26 29 35 40 51 10 16 21 41 44 54 62 65 71 74 
    sort( ia, middle ); sort( middle, last ); 
    
    // 10 12 15 16 17 20 21 23 26 29 35 40 41 44 51 54 62 65 71 74 
    inplace_merge( ia, middle, last ); 


<p><a name="iter_swap"><h1>iter_swap()</h1></a></p>
Swaps the values contained within the elements addressed by two iterators.

    #include <algorithm> 
    typedef list<int>::iterator iterator; 
    iterator it1 = ilist.begin(), it2 = ilist.begin()+4; 
    iter_swap( it1, it2 ); 


<p><a name="lexicographical_compare"><h1>lexicographical_compare()</h1></a></p>
By default, the less-than operator is applied, although an optional fifth option allows us to provide an alternative ordering operation. Returns true if the first sequence is less than or equal to the second sequence.

    #include <algorithm> 
    class size_compare { 
    public: 
       bool operator()( const string &a, const string &b ) { 
            return a.length() <= b.length(); 
       } 
    }; 
    string sa1[] = { "Piglet", "Pooh",  "Tigger" }; 
    string sa2[] = { "Piglet", "Pooch", "Eeyore" }; 
    
    // false: 'c' less than 'h' 
    res = lexicographical_compare( sa1, sa1+3, sa2, sa2+3 ); 
    
    list<string> ilist1( sa1, sa1+3 ); 
    list<string> ilist2( sa2, sa2+3 ); 
    
    // true: Pooh < Pooch 
    res = lexicographical_compare( 
             ilist1.begin(), ilist1.end(), 
             ilist2.begin(), ilist2.end(), size_compare() ); 


<p><a name="max"><h1>max(), min()</h1></a></p>
Returns the larger (or smaller) of the two elements. An optional third argument allows us to provide an alternative comparison operation.


<p><a name="max_element"><h1>max_element() , min_element()</h1></a></p>
Returns an iterator pointing to the largest (or smallest) value within the sequence. An optional third argument allows us to provide an alternative comparison operation.

    #include <algorithm> 
    int mval = max( max( max( max( ivec[4],  ivec[3] ), 
                                   ivec[2] ),ivec[1] ),ivec[0] ); 
    
    mval = min( min( min( min( ivec[4], ivec[3] ), 
                               ivec[2] ),ivec[1] ),ivec[0] ); 
    vector<int>::const_iterator iter; 
    iter = max_element( ivec.begin(), ivec.end() ); 
    iter = min_element( ivec.begin(), ivec.end() ); 


<p><a name="merge"><h1>merge()</h1></a></p>
Combines two sorted sequences into a single sorted sequence addressed by the fifth iterator. An optional sixth argument allows us to indicate an ordering other than the default less-than operator.

    #include <algorithm> 
    int ia[12] =  {29,23,20,22,17,15,26,51,19,12,35,40}; 
    int ia2[12] = {74,16,39,54,21,44,62,10,27,41,65,71}; 
    
    vector<int> vec1( ia, ia+12 ),  vec2( ia2, ia2+12 ); 
    vector<int> vec_result(vec1.size()+vec2.size()); 
    
    sort( vec1.begin(), vec1.end(), greater<int>() ); 
    sort( vec2.begin(), vec2.end(), greater<int>() ); 
    
    merge( vec1.begin(), vec1.end(), 
           vec2.begin(), vec2.end(), 
           vec_result.begin(), greater<int>() ); 


<p><a name="nth_element"><h1>nth_element()</h1></a></p>
`nth_element()` reorders the sequence so that all elements less than the nth element occur before it and all elements that are greater occur after it. For example, given

    int ia[] = { 29,23,20,22,17,15,26,51,19,12,35,40 }; 

an invocation of `nth_element()` marking ia+6 as nth (it has a value of 26)

    nth_element( ia, ia+6, &ia[12] ); 

yields a sequence in which the seven elements less than 26 are to its left, and the four elements greater than 26 are to its right:

    { 23,20,22,17,15,19,12,26,51,35,40,29 } 

The elements on either side of the nth element are not guaranteed to be in any particular order. An optional fourth parameter allows us to indicate a comparison other than the default less-than operator.


<p><a name="partial_sort"><h1>partial_sort(), partial_sort_copy()</h1></a></p>
`partial_sort()` accepts three parameters ? first, middle, and last ? and an optional fourth parameter that provides an alternative ordering operation. The iterators first and middle mark the range of slots available to place the sorted elements of the container (middle is 1 past the last valid slot). The elements stored beginning at middle through last are unsorted. For example, given the array

    int ia[] = {29,23,20,22,17,15,26,51,19,12,35,40 }; 

an invocation of `partial_sort()` marking the sixth element as middle

    partial_sort( ia, ia+5, ia+12 ); 

yields the sequence in which the five smallest elements are sorted:

    { 12,15,17,19,20,29,23,22,26,51,35,40 } 

The elements from middle through last-1 are not placed in any particular order, although all their values fall outside the sequence actually sorted.


<p><a name="partial_sum"><h1>partial_sum()</h1></a></p>
Creates a new sequence in which, by default, the value of each new element represents the sum of all the previous elements up to its position. For example, given the sequence `{0,1,1,2,3,5,8}`, the new sequence is `{0,1,2,4,7,12,20}`. The fourth element, for example, is the partial sum of the three previous values (0,1,1) plus its own (2), yielding a value of 4. An optional fourth parameter allows the user to specify an alternative operation to apply.

    #include <numeric> 
    int ires[7], ia[7] = { 1, 3, 4, 5, 7, 8, 9 }; 
    vector<int> vres(7), vec( ia, ia+7 ); 
    
    // partial_sum(): 1 4 8 13 20 28 37 
    partial_sum( ia, ia+7, ires ); 
    
    //partial sum using times<int>(): 1 3 12 60 420 3360 30240 
    partial_sum(vec.begin(),vec.end(),vres.begin(),times<int>()); 
     

<p><a name="partition"><h1>partition(), stable_partition()</h1></a></p>
`partition()` reorders the elements based on the true/false evaluation of a unary operation. All the elements that evaluate as true are placed before the elements that evaluate as false. For example, given the sequence `{0,1,2,3,4,5,6}` and a predicate that tests for elements that are even, the true and false element ranges are `{0,2,4,6}` and `{1,3,5}`. Although all the even elements are guaranteed to be placed before any of the odd elements, the relative position of the elements within the reordering is not guaranteed to be preserved. `stable_partition()` guarantees to preserve the relative order of the elements within the container.

    #include <algorithm> 
    
    class even_elem { 
    public: 
        bool operator()( int elem ) 
             { return elem%2 ? false : true; } 
    }; 
    
    int ia[] = { 29,23,20,22,17,15,26,51,19,12,35,40 }; 
    vector<int> vec( ia, ia+12 ); 
    // partition based on whether element is even: 
    //   40 12 20 22 26 15 17 51 19 23 35 29 
    stable_partition( vec.begin(), vec.end(), even_elem() ); 


<p><a name="random_shuffle"><h1>random_shuffle()</h1></a></p>
By default, `random_shuffle()` reorders the elements randomly based on its own algorithm. An optional third parameter allows us to pass in a random-number-generating operation that must return a value of type double within the interval\[0,1\].

    #include <algorithm> 
    random_shuffle( ivec.begin(), ivec.end() ); 

 
<p><a name="remove"><h1>remove(), remove_copy()</h1></a></p>
`remove()` separates out all instances of a value within the sequence. It does not actually erase the matched elements (the container's size is preserved). Rather, each nonmatching element is assigned in turn to the next free slot. The returned iterator marks 1 past the new range of elements.

For example, consider the sequence `{0,1,0,2,0,3,0,4}`. Let's say that we wish to remove all 0 values. The resulting sequence is `{1,2,3,4,0,3,0,4}`. The 1 is copied into the first slot, the 2 into the second slot, the 3 into the third slot, and the 4 into the fourth slot. The 0 at the fifth slot represents the leftover of the algorithm. The returned iterator addresses that slot. Typically, this iterator is then passed to erase(). (The built-in array is not suited to the `remove()` algorithm because it cannot be resized easily. For this reason, the `remove_copy()` is the preferred algorithm for use with an array.)

    #include <algorithm> 
    
    int ia[] = { 0, 1, 0, 2, 0, 3, 0, 4, 0, 5 }; 
    vector<int> vec( ia, ia+10 ); 
    
    // vector after remove, without applying erase(): 
    // 1 2 3 4 5 3 0 4 0 5 
    vec_iter = remove( vec.begin(), vec.end(), 0 ); 
    
    // vector after erase(): 1 2 3 4 5 
    vec.erase( vec_iter, vec.end() ); 
    
    int ia2[5]; 
    // ia2: 1 2 3 4 5 
    remove_copy( ia, ia+10, ia2, 0 ); 


<p><a name="remove_if"><h1>remove_if(), remove_copy_if()</h1></a></p>
`remove_if()` removes all elements within the sequence for which the predicate operation evaluates as true. Otherwise, `remove_if()` and `remove_copy_if()` behave the same as `remove()` and `remove_copy()` ? see the earlier discussion.

    #include <algorithm> 
    
    class EvenValue { 
    public: 
       bool operator()( int value ) { 
            return value % 2 ? false : true; } 
    }; 
    
    int ia[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34 }; 
    vector<int> vec( ia, ia+10 ); 
    
    iter = remove_if( vec.begin(), vec.end(), bind2nd(less<int>(),10) ); 
    vec.erase( iter, vec.end() ); // sequence now: 13 21 34 
    
    int ia2[10]; // ia2: 1 1 3 5 13 21 
    remove_copy_if( ia, ia+10,ia2, EvenValue() ); 

 
<p><a name="replace"><h1>replace(), replace_copy()</h1></a></p>
`replace()` replaces all instances of old_value with new_value within the sequence.

    #include <algorithm> 
    string oldval( "Mr. Winnie the Pooh" ); 
    string newval( "Pooh" ); 
    string sa[] = { "Christopher Robin", "Mr. Winnie the Pooh", 
                    "Piglet", "Tigger", "Eeyore" }; 
    
    vector<string> vec( sa, sa+5 ); 
    
    // Christopher Robin Pooh Piglet Tigger Eeyore 
    replace( vec.begin(), vec.end(), oldval, newval ); 
    
    vector<string> vec2; 
    
    // Christopher Robin Mr. Winnie the Pooh Piglet Tigger Eeyore 
    replace_copy( vec.begin(), vec.end(), 
                  inserter(vec2,vec2.begin()), newval, oldval ); 

     
<p><a name="replace_if"><h1>replace_if(), replace_copy_if()</h1></a></p>
`replace_if()` replaces all elements within the sequence with new_value for which the predicate comparison operation evaluates as true.

    #include <algorithm> 
    int new_value = 0; 
    int ia[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34 }; 
    vector<int> vec( ia, ia+10 ); 
    
    // new sequence:  0 0 0 0 0 0 0 13 21 34 
    replace_if( vec.begin(), vec.end(), 
                bind2nd(less<int>(),10), new_value ); 

     
<p><a name="reverse"><h1>reverse(), reverse_copy()</h1></a></p>
Reverses the order of elements in a container.

    #include <algorithm> 
    list<string> slist_copy( slist.size() ); 
    
    reverse( slist.begin(), slist.end() ); 
    reverse_copy( slist.begin(), slist.end(), slist_copy.begin() ); 


<p><a name="rotate"><h1>rotate(), rotate_copy()</h1></a></p>
`rotate()` is passed three iterators: first, middle, and last. It exhanges the two ranges marked by the iterators first, middle-1 and middle, last-1. For example, given the following C-style character string "boohiss!!",

    char ch[] = "boohiss!!"; 

To change it to "hissboo!!", the call to `rotate()` looks like this:

    rotate( ch, ch+3, ch+7 ); 

Here is another example:

    #include <algorithm> 
    int ia[] = { 1, 3, 5, 7, 9, 0, 2, 4, 6, 8, 10 }; 
    vector<int> vec( ia, ia+11 ), vec2(11); 

In this first invocation, we exchange the last six elements, beginning with 0, with the first five elements, beginning with 1:

    // rotate on middle element(0) : 0 2 4 6 8 10 1 3 5 7 9 
    rotate( ia, ia+5, ia+11 ); 

In this second invocation, we exchange the last two elements, beginning with 8, with the first nine elements, beginning with 1:

    // rotate on next to last element(8): 8 10 1 3 5 7 9 0 2 4 6 
    rotate_copy( vec.begin(), vec.end()-2, vec.end(), vec2.begin() ); 


<p><a name="search"><h1>search()</h1></a></p>
Given two sequences, `search()` returns an iterator that addresses the first position in the first sequence in which the second sequence occurs. If the subsequence does not occur, an iterator that addresses the end of the first sequence is returned. For example, within Mississippi, the subsequence iss occurs twice, and `search()` returns an iterator to the start of the first instance. An optional fifth parameter allows the user to override the default equality operator.

    #include <algorithm> 
    
    char str[25] = "a fine and private place"; 
    char substr[4]  = "ate"; 
    int *piter = search( str,str+25,substr,substr+4 ); 


<p><a name="search_n"><h1>search_n()</h1></a></p>
`search_n()` looks for the first occurrence of n instances of a value within a sequence. In the following example, we search str for two occurrences of the character o in succession, and an iterator to the first o of moose is returned. If the subsequence is not present, an iterator that addresses the end of the first sequence is returned. An optional fifth parameter allows the user to override the default equality operator.

    #include <algorithm> 
    const char oh    = 'o'; 
    
    char str[ 26 ]  = "oh my a mouse ate a moose"; 
    char *found_str = search_n( str, str+26, 2, oh ); 


<p><a name="set_difference"><h1>set_difference()</h1></a></p>
`set_difference()` constructs a sorted sequence of the elements present in a first sequence but not present in a second. For example, given the two sequences {0,1,2,3} and `{0,2,4,6}`, the set difference is `{1,3}`. All the set algorithms (three additional algorithms follow) take five iterators: The first two mark the first sequence, and the second two mark the second sequence. The fifth iterator marks the position of the container into which to copy the elements. The algorithm presumes that the sequences are sorted using the less-than operator; an optional sixth argument allows us to pass in an alternative ordering operation.


<p><a name="set_intersection"><h1>set_intersection()</h1></a></p>
`set_intersection()` constructs a sorted sequence of the elements present in both sequences. For example, given the two sequences `{0,1,2,3}` and `{0,2,4,6}`, the set intersection is `{0,2}`.


<p><a name="set_symmetric_difference"><h1>set_symmetric_difference()</h1></a></p>
`set_symmetric_difference()` constructs a sorted sequence of the elements that are present in the first sequence but not present in the second, and those elements present in the second sequence are not present in the first. For example, given the two sequences `{0,1,2,3}` and `{0,2,4,6}`, the set symmetric difference is `{1,3,4,6}`.


<p><a name="set_union"><h1>set_union()</h1></a></p>
`set_union()` constructs a sorted sequence of the element values contained within the two sequences. For example, given the two sequences `{0,1,2,3}` and `{0,2,4,6}`, the set union is `{0,1,2,3,4,6}`. If the element is present in both containers, such as 0 and 2 in the example, the element of the first container is copied.


    #include <algorithm> 
    string str1[] = { "Pooh", "Piglet", "Tigger", "Eeyore" }; 
    string str2[] = { "Pooh", "Heffalump", "Woozles" }; 
    set<string> set1( str1, str1+4 ), 
                set2( str2, str2+3 ); 
    
    // holds result of each set operation 
    set<string> res; 
    
    //set_union(): Eeyore Heffalump Piglet Pooh Tigger Woozles 
    set_union( set1.begin(), set1.end(), 
               set2.begin(), set2.end(), inserter(res,res.begin())); 
    
    res.clear(); // empties the container of elements 
    
    // set_intersection(): Pooh 
    set_intersection( set1.begin(), set1.end(), set2.begin(), 
                      set2.end(), inserter( res,res.begin() )); 
    
    res.clear(); 
    
    // set_difference(): Eeyore Piglet Tigger 
    set_difference( set1.begin(), set1.end(), set2.begin(), 
                    set2.end(), inserter( res, res.begin() )); 
    
    res.clear(); 
    
    // set_symmetric_difference(): 
    //    Eeyore Heffalump Piglet Tigger Woozles 
    set_symmetric_difference( set1.begin(), set1.end(), set2.begin(), 
                              set2.end(), inserter( res, res.begin() )); 

     
<p><a name="sort"><h1>sort(), stable_sort()</h1></a></p>
By default, sorts the elements in ascending order using the less-than operator. An optional third parameter allows us to pass in an alternative ordering operation. `stable_sort()` preserves the relative order of elements within the container. For example, imagine that we have sorted our words alphabetically and now wish to order them by word length. To do this, we pass in a function object LessThan that compares two strings by length. Were we to use `sort(),` we would not be guaranteed to preserve the alphabetical ordering.

    #include <algorithm> 
    stable_sort( ia, ia+8 ); 
    stable_sort( svec.begin(), svec.end(), greater<string>() ); 


<p><a name="transform"><h1>transform()</h1></a></p>
The first version of `transform()` invokes the unary operator passed to it on each element in the sequence. For example, given a sequence `{0,1,1,2,3,5}` and a function object Double, which doubles each element, the resulting sequence is `{0,2,2,4,6,10}`. The second version invokes the binary operator passed to it on the associated elements of a pair of sequences. For example, given the sequences `{1,3,5,9}` and `{2,4,6,8}`, and a function object AddAndDouble that adds the two elements and then doubles their sum, the resulting sequence is `{6,14,22,34}`. The resulting sequence is copied into the container pointed to by either the third iterator of the first version or the fourth iterator of the second.

    #include <algorithm> 
    int double_val( int val ) { return val + val; } 
    int difference( int val1, int val2 ) { return abs( val1 - val2 ); } 
    
    int ia[]  = { 3, 5, 8, 13, 21 }; 
    vector<int> vec( 5 ), vec2( 5 ); 
    
    // first version: 6 10 16 26 42 
    transform( ia, ia+5, vec.begin(), double_val ); 
    
    // second version: 3 5 8 13 21 
    transform( ia, ia+5, vec.begin(), vec2.begin(), difference ); 


<p><a name="unique"> <h1>unique(), unique_copy()</h1> </a></p>
All consecutive groups of elements containing either the same value (using the equality operator) or evaluating as true when passed an optional alternative comparison operation are collapsed into a single element. In the word Mississippi, the semantic result is "Misisipi." Because the three i's are not consecutive, they are not collapsed, nor are the two pairs of s's. To guarantee that all duplicated elements are collapsed, we would first sort the container.

As with `remove(),` the container's actual size is not changed. Each unique element is assigned in turn to the next free slot, beginning with the first element of the container. In our example, the physical result is "Misisipippi," where the character sequence ppi represents the leftover piece of the algorithm. The returned iterator marks the beginning of the refuse. Typically this iterator is then passed to `erase().` (Because the built-in array does not support the `erase()` operation, `unique()` is less suitable for arrays; `unique_copy()` is more appropriate.)

    #include <algorithm> 
    int ia[] = { 0, 1, 0, 2, 0, 3, 0, 4, 0, 5 }; 
    vector<int> vec( ia, ia+10 ); 
    
    sort( vec.begin(), vec.end() ); 
    iter = unique( vec.begin(), vec.end() ); 
    vec.erase( vec_iter, vec.end() ); // vec: 0 1 2 3 4 5 
    
    int ia2[10]; 
    sort( ia, ia+10 ); 
    unique_copy( ia, ia+10, ia2 ); 


<p><a name="conclusion"> <h1>CONCLUSION</h1> </a></p>
这篇文章乃《[Essential C++](http://book.douban.com/subject/4934674/)》的附录B——Generic Algorithm Handbook,由于本人使用STL时经常查询该文档，特此整理，欢迎转载，转载请注明出处www.mingxinglai.com。
