---
title: Memoization lab
...

Hardware caches are constrained by the need to be created in hardware.
Software has more options available.
In this lab, you'll experiment with a few different approaches to "caching" in software.

# Memoization vs Caching

The core concept of caching is keeping in a high-speed data structure the results of past work in hopes that you'll use that data again soon. In software, this takes two forms:

- Memoizing stores *all* past work. It can change the asymptotic complexity of some algorithms.
- Caching stores *recent* past work. It can change the practical speed of some algorithms.

Both tend to use an associative array type (i.e., a map, dict, table, etc) as the underlying data structure.
The simplest memoization approach is

    RetType plainAlgo(ArgType arg) { ... }

    map<ArgType, RetType> memos;
    RetType memoizedAlgo(ArgType arg) {
        if (memos.contains(arg)) return memos[arg];
        else return memos[arg] = plainAlgo(arg);
    }

Caching requires a more involved data structure which acts like both a linked list and an associative array. The linked list stores which items have been used recently, and the associative array stores the relationship between keys and values. Neither C nor C++ comes with a suitable data structure, so we'll memoize instead in this lab.

# Your task

Start with this code:

```cpp
typedef unsigned long long ulong;

ulong fib(int i) {
    if (i < 2) return 1;
    return fib(i-1) + fib(i-2);
}

double phi(int i) {
    return fib(i) / (double)fib(i-1);
}
```

This is the classic recursive Fibbonacci sequence, a very inefficient way to compute it; and a function that uses it to compute the golden ratio $\phi$.

{.exercise ...}
1. Write a main function and time the asymptotic complexity of `fib` and `phi`.
    Estimate the complexity class of each.
    
    How much does the run time change when you compile with `clang++ -O3` instead of `clang++ -O0`?

    <textarea style="width:100%"></textarea>

2. Memoize `phi` (but not `fib`) and time it again.
    
    How has that changed the runtime?
    Has the complexity class changed?
    Does it matter how many times you run `phi`?
    If you try a lot of arguments, does it matter if you start with the smallest or the largest?

    <textarea style="width:100%"></textarea>

3. Memoize `fib` and time it.    

    How has that changed the runtime?
    Has the complexity class changed?
    Does it matter how many times you run `phi`?
    If you try a lot of arguments, does it matter if you start with the smallest or the largest?

    <textarea style="width:100%"></textarea>

4. Implement an iterative version of `fib` that mimics the pseudocode

        a,b = 1,1
        repeat i times:
            a,b = b,a+b
        return a
    
    Time this and estimate its complexity.
    Is it faster or slower than the memoized `fib`?
    Under what conditions?

    <textarea style="width:100%"></textarea>

5. Memoize the iterative `fib`.
    Time this and estimate its complexity.
    Is it faster or slower than each of the other `fib`s?
    Under what conditions?

    <textarea style="width:100%"></textarea>

6. You can stop here, but there's more to explore if you are interested...

    a. Classic Fibonacci is $f(n) = f(n-1) + f(n-2)$, but you can extend this to other $f_k(n) = f_k(n-k) + f_k(n-k-1)$ cases. The ratio of adjacent elements of $f_1$ approximate [the golden ratio](https://en.wikipedia.org/wiki/Golden_ratio);
    the ratio of adjacent elements of $f_2$ approximate [the plastic number](https://en.wikipedia.org/wiki/Plastic_number);
    $f_3$ and beyond also approximate numbers with interesting properties, though they are not named.
        
        Implement, time, memoize, and re-time this more general family of Fibonacci-like functions and constant computations.
        What's the measured complexity in terms of $n$?
        In terms of $k$?
        Is that true for both the with-memoizing and without cases?

        <textarea style="width:100%"></textarea>
    
    b. A cache (instead of a memo) for recursive Fibonacci would roughly end up caching small values only.
        Try what this runs like by only memoizing $i < 50$.
        How does this change the function's runtime?

        <textarea style="width:100%"></textarea>
    
    c. Computing runtimes is all well and good, but what about visualizing them?
        One of the easiest ways to visualize a line is to output a simple [SVG](https://en.wikipedia.org/wiki/Scalable_Vector_Graphics) file.
        This means the following:
        
        i. Open a file for writing
        i. Write `<?xml version="1.0" encoding="UTF-8" standalone="no"?>` to it
        i. write `<svg xmlns="http://www.w3.org/2000/svg" viewBox="`xmin` `ymin` `width` `height`">` to it. You'll need to know the extent of the x and y coordinates you want to draw to compute the xmin, ymin, width, and height appropriately.
        i. For each line you want to graph,
            - write `<path stroke-width="1" fill="none" d="M`
            - write each x,y coordinate separated with a comma and followed by a space
            - write `"/>`
        i. write `</svg>`
        
        SVG files can be viewed by most web browsers.
        
        Try making a graph of runtimes for one or more of your functions.
        Note that you may need to scale the x and y axes to be roughly the same width and height for the graphs to be useful.
{/}

The sections after this provide guidance on how to do some of the steps above.

# On using an STL map

The C++ standard template library (STL) was introduced in your [last COA1 lab](../../COA1/F2019/lab11-stl.html).
It uses the general rule of one data structure per header file; two that might be useful in this lab are [`std::map<K,V>`{.cpp} from `#include <map>`{.cpp}](https://en.cppreference.com/w/cpp/container/map), a red-black tree; and [`std::unordered_map<K,V>`{.cpp} from `#include <unordered_map>`{.cpp}](https://en.cppreference.com/w/cpp/container/unordered_map), a hash table.

Maps (either kind) has a slightly obtuse way of checking for membership: you first `.find()` a key, which returns an iterator to a key-value pair; then you compare it to the `.end()` of the map; if it is not the `.end()` then you found an entry and can get the value as the `->second` element of the iterator.

```cpp
std::map_type_you_picked<K,V> m;
m[key1] = value1;
// ...
auto finder = m.find(key);
if (finder != m.end()) {
    V value = finder->second;
    // ...
} else {
    // key not in the map
}
```

While Java has a datatype that can make efficient caches (the [LinkedHashMap](https://docs.oracle.com/en/java/javase/13/docs/api/java.base/java/util/LinkedHashMap.html)), C++ does not, so we'll only use memoizing in this lab.

# On estimating big-O

The following gives tips on how to turn empirical runtimes into rough big-O formulas. It uses $n$ to represent input size and $t$ to represent measured runtime.

Assymptotic         This will be constant       And will approximate
-----------         -----------------------     ---------------------
$t\sim r\log(n)$    $2^{t} \div n$              $2^r$
$t \sim r n$        $t \div n$                  $r$
$t \sim n^r$        $\log(t) \div \log(n)$      $r$
$t \sim r^n$        $\log(t) \div n$            $\log(r)$

Note that fancier functions like $n^k \log(n)$ are harder to identify with simple formulae like those above, and that the above only work for "large enough" $n$ -- as a rule of thumb, if an expression above is constant for $n$s that result in $t = 0.1$ second and $t = 2$ seconds, you probably have the right formula.

The general approach to estimating timing empirically is to make a loop that iterates over larger and large $n$, timing the code to get $t$ and then computing and printing out a should-be-constant expression above; if that seems to be converging, you have an estimated complexity class.


# On timing code

Timing code is more complicated than it at first sounds for the following reasons:

- You have more than one processor core, each with its own clock. Long-running tasks may begin on one and end on another.
- CPU clocks can be dynamically scaled; for example, one of my computers runs at 4.1 GHz when it has a lot of work, but slows to 700 MHz when most processes are idle.
- Hardware clocks drift a bit over time, so they periodically correct themselves by coordinating with time servers. That means that clocks might sometimes jump forward or backward by some significant fraction of seconds.
- The task of looking at the clock itself takes some time
- A given process may sometimes be suspended while the CPU handles other processes, making it unclear if the time we want is "wall-clock time" or "processor time".
- If the OS spends time on a system call or interrupt while your process is running, should that be counted or not?
- If the process uses multiple threads, do you try to measure the start-to-end time or the sum of the time used by each thread or the average time used by each thread?
- Etc.

The end result of this is that there are many different clocks and that picking the right one is not always straighforward. For this lab and other timings we'll do in this class, we'll use the following:

```c
#include <time.h>

/// returns the number of nanoseconds that have elapsed since 1970-01-01 00:00:00
long long nsecs() {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec*1000000000 + t.tv_nsec;
}
```

To time a piece of code run `nsecs` before and after the code and then subtract. Note that because this measures "wall-clock time" there is a reasonable chance that you'll get a strange result from time to time when there is a context switch (i.e., the OS or another process uses the processor for a bit) or time correction.
