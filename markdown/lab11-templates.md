---
title: C++ templates
...

We showed you some basic C++ last semester, including using templates like Java uses generics.
But that is far from all C++ templates can do...
This lab will help you explore this topic in more detail.

# Caveat

There exists a subset of programmers who believe that C++ templates beyond type generics are an abomination and should never be used.
I hope this lab will help you make your own opinion, but be aware that if you work for a company that uses C++ and suggest some of these techniques, you might get some blow-back...

# A Mathematical Vector type

In mathematics, a vector has fixed length and adding vectors of different length is meaningless.
In this lab you'll use C++ templates to make a mathematical vector class
that uses the type checker to ensure at compile time that no wrong-sized operations occur.

We'll also use a `struct`, not a `class`, to encourage passing small vectors by value instead of by pointer.

{.exercise ...}
Implement a templated mathematical vector type in C++.
When you are done, the following code should do as the comments suggest (you'll need to comment out the "does not compile" lines to get the others to work):

```cpp
typedef vec<double, 2> vd2;
typedef vec<double, 3> vd3;
typedef vec<int,4> vi4;

int main() {
    vd3 a;
    std::cout << a << std::endl; // prints (0, 0, 0)
    
    a[2] = 2.5;
    std::cout << a << std::endl; // prints (0, 0, 2.5)

    std::cout << (a + a) << std::endl; // prints (0, 0, 5)
    std::cout << a << std::endl; // prints (0, 0, 2.5)

    vd2 b;
    std::cout << b << std::endl; // prints (0, 0)
    vi4 c;
    std::cout << c << std::endl; // prints (0, 0, 0, 0)
    
    std::cout << (a + b) << std::endl; // does not compile
    std::cout << (a + c) << std::endl; // does not compile
    
    
    b[2] = 2.5; // assigns off the end of b...
    c[2] = 2.5; // implictly casts a double to an int...
    b[1] = 2.5;
    c[2] = 2;

    std::cout << b << std::endl; // prints (0, 2.5)
    std::cout << c << std::endl; // prints (0, 1074003968, 2, 0)
                                 // -- the 1074003968 is from overflow
    vd3 x = {1.5, 2.5, 3.5};
    std::cout << x << std::endl; // prints (1.5, 2.5, 3.5)

    vd3 y = {1, 2, 3};
    std::cout << y << std::endl; // prints (1, 2, 3)
    
    vd3 z = {1, 2.3, 4.5}; // does not compile
}
```

You code should not contain any heap-allocated memory (no `new` or `malloc`)
{/}

# Guides

- Precede your `struct` with `template <typename N, int n>`{.cpp} so it will work with multiple types and lengths

- Use a static array `N data[n]` (or the like) as the only field so avoid heap memory allocations

- Constructors have no return type and the same name as the `struct`
    - your default constructor should sets the data to all `0`s
    - the other should accept an "initializer list", which is what the `{1,2,3}` turns into at compile time
        - `#include <initializer_list>`
        - 1 argument, a `std::initializer_list<R>` where `R` is the type of value you expect to be passed in.
        - inside the code, the argument is an iterator, so something like
            
            ````cpp
            for(auto x=val.begin(); x!=val.end(); ++x) {
                // use *x here (* looks inside the iterator)
            }
            ````
            
            should work
        - you'll need to verify that the initializer list has the right number of values (it's `size()` method should help)
        - we recommend using a template to pick the initializer list contained type, e.g. with `template<typename R>`{.cpp} before the function

- Operator overloading uses special function names
    - `operator +` should accept only vectors of the same length
    - `operator []` needs two variants
        - `N& operator[] (int idx)`{.c} -- allows setting by index by returning a reference
        - `N operator[] (int idx) const`{.c} -- allows reading when no reference exists
    - Printing needs a special "friend" notation to let you access between the `ostream` and `vec` classes:
        - `friend std::ostream& operator << (std::ostream& out, const vec<N,n>& x)`{.c}
        - this function should use `out << thing` and then `return out`



