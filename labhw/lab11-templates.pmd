---
title: C++ templates
...

We showed you some basic C++ last semester, including using templates like Java uses generics.
But that is far from all C++ templates can do...
This lab will help you explore this topic in more detail.

You'll work in pairs via discord for this lab, like those before it.

# Caveat

There exists a subset of programmers who believe that C++ templates beyond type generics are an abomination and should never be used.
I hope this lab will help you make your own opinion, but be aware that if you work for a company that uses C++ and suggest some of these techniques, you might get some blow-back...

# Review

Begin by reviewing the C++ labs from COA1:

- [name overloading, namespaces, references, methods](/luther/COA1/F2019/lab10-cpp.html)
- [STL, references, templates, operator overloading, iostream](/luther/COA1/F2019/lab11-stl.html)

We'll be picking up from where those left off in this lab.

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
    c[2] = 2.5; // implicitly casts a double to an int...
    b[1] = 2.5;
    c[2] = 2;

    std::cout << b << std::endl; // prints (0, 2.5)
    std::cout << c << std::endl; // prints (0, 1074003968, 2, 0)
                                 // -- the 1074003968 is from b[2] overflow
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
        - a `std::initializer_list<R>` is a collection, meaning you access it by [iterator](#cpp-iterators)
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

- It is best practice (but not technically required) to
    
    - put `template` declarations on the line before the struct or function they modify
    
        ````{.cpp}
        template <typename R>
        R dostuff(const R& t) { /*...*/ }
        ````
    
    - use `const` for all arguments you will not modify
    
        `double sqrt(const double x)`{.cpp}

    - use reference types `const mytype&` for `struct` arguments you don't want to copy
    
        `double length(const vec<double,3>& x)`{.cpp}
    
    - use `const` for any method that does not modify `this`'s fields
    
        `double length() const`{.cpp}

# C++ Iterators {#cpp-iterators}

Many STL structures in C++ use the iterator pattern to allow access.
This has a somewhat different look than it does in Java or the like.

The collection offers two functions of note:

- `.begin()` returns an iterator pointing to the first element of the collection
- `.end()` returns an iterator pointing to the "past the end" element of the collection

The iterator overloads three operators of note:

- `operator !=` tells if two iterators point to distinct entries in the collection
- `operator *` gets the item pointed to out of the iterator
- `operator ++` moves the item to the next spot in the collection

Thus, a loop that prints all items in a collection might look like

```cpp
for(auto it = mycollection.begin(); // create an iterator
    it != mycollection.end();       // and while it's not off the end
    ++it) {                         // move it forward
    std::cout << *it << std::endl;  // derreference to print
}
```

Note in the above that when you combine a declaration and initialization,
you can declare the type to be `auto`{.cpp} meaning "use whatever type the initialization gives me".
