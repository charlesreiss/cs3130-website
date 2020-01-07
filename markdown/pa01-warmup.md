---
title: PA01 -- warmup
...

This is mostly a reminder/refresher on C.
It also makes use of the [introduction to makefiles](make.html).

# Three files

Write the following files:

`main.c`
:   contains a `main` function.
    If there are command-line arguments,
    convert each to an integer and invoke `pprime` with each.
    If there are no command line arguments,
    read integers from standard input (provided in base-10, one number per line)
    until the end of standard input is reached
    and invoke `pprime` with each integer.
    `main` should not print anything itself.

`primes.c`
:   contains (at least) three functions:

    `isprime`
    :   Accepts a single integer and returns 1 if it is prime, 0 otherwise.
        You are welcome to brute-force this or use the primality checking from COA1's `smallc` program.
    
    `nextprime`
    :   See [COA1's smallc](/luther/COA1/F2018/pa06-smallc.html#unsigned-long-nextprimeunsigned-long-x) for a description.
    
    `pprime`
    :   Given an integer, print a single line to standard out containing either
    
            (number) is prime
        
        or
        
            (number) is not prime, but (bigger number) is
        
        where `(number)` is the argument integer
        and `(bigger number)` is the result of invoking `nextprime` on the argument
        
        Do not include the parentheses in your actual output; correct formatting is
            
            3 is prime
            4 is not prime, but 5 is

`primes.h`
:   contains the header for `pprime`, `isprime`, and `nextprime`.

Also create a makefile named `Makefile` such that typing

    make

will rebuild (if necessary) `primes.o` and `main.o` and link them together into an executable `primes`.

See `info make` for detailed converation about how `make` works;
<https://www.gnu.org/software/make/manual/make.html> for online documentation;
and <https://www.gnu.org/software/make/manual/make.html#Rule-Example> for an example to work from.

If your code uses any function in `math.h` (such as `sqrt`, `pow`, `log`, etc.), make sure you add `-lm` to the `LDFLAGS` of your `Makefile` or it will not compile on the department servers.

# Collaboration

This assignment is intended to be exploratory and get your feet wet;
you are welcome to help one another with it as much as you wish.
However, you are responsible for both (a) citing help and (b) understanding what you submit.

