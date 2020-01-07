---
title: Make lab
...

This lab is designed to help you feel comfortable using GNU Make, one of the oldest and still most-common build tools; as well as with many-file C projects.

# A bit about `bash`

Recall that each line of `bash` begins with a program and is followed by any number of arguments.
`bash` also has various control constructs and special syntax that can be used where programs are expected, like `for` and `x=`, which we will not cover in this class.
It can also redirect input and output using `|`, `>`, `<`, `>>`, `2>`, and a few others.

Because new-lines are important to `bash`, but sometimes line breaks help reading, you can end a `bash` line with `\` to say "I'm not really done with this line". Thus

```bash
echo 1 echo 2 \
echo \
3 \
echo 4
echo 5 \
    echo 6
echo 7
```

is equivalent to 

```bash
echo 1 echo 2 echo 3 echo 4
echo 5 echo 6
echo 7
```

You can also combine lines; a `;` is (almost) the same as a new-line to `bash`.

# Multi-file design

It is possible to have a compiler read every file involved in a project and from them create one big binary.
However, that is generally undesirable, as it means both that the build process takes time proportional to the total project size and that the resulting binary can get quite large.

## Vocabulary

Header File
:   A file (`.h` for C; `.h`, `.hpp`, `.H` or `.hh` for C++) that provides 
    
    - the signatures of functions, but not their definition
    - the names and types of global variables, but not where in memory they go
    - `typedef`s and `#define`s

Library
:   A collection of related implementations, generally provided as both a *library file* and a *header file*.

Library File
:   Either a *shared library* file or a *static library* file.

Object File
:   A file (`.o` on most systems, `.obj` on Windows) that contains assembled binary code in the target ISA, with metadata to allow the linker to place it in various locations in memory depending on what other files it is linked with.

Shared Library
:   A file (`.so` on most systems, `.dll` on Windows) that contains one more more *object files* together with metadata needed to connect them into a running code system by the *loader*. This load-time linkage means both that (a) multiple running programs can share the same copy of the shared library, saving memory; and (b) the library can be updated independently of the programs that use it, ideally allowing modular updates.
    
    Because they are linked by the *loader* each time the program is run, your OS needs to both (a) have a copy of the library files and (b) know where they are in order to run a program that uses *shared libraries*.

Standard Library
:   A *library* to which every program is linked by default.
    Virtually every language has a *standard library*, which does much to define the character of the language.
    
    The C language's *standard library* is called `libc` and is defined by the C standard (see <https://en.wikipedia.org/wiki/C_standard_library>); there are several implementations of it, but `glibc` is probably the most pervasive.

Static Library
:   A file (`.a` on most systems, `.lib` on Windows) that contains one more more *object files* together with metadata needed to connect them into a running code system by the *linker*. This compile-time linkage means that each executable has its own copy of the library stored internally to the file, without external dependencies.

Source File
:   A file (`.c` for C, `.cpp`, `.C`, or `.cc` for C++) that provides
    
    - the implementation of functions
    - the declaration and implementation of provide helper functions
    - the memory in which to store global variables

## Common flow

A project
:   consists of several source files, header files, and dependencies
    
    header file
    :   Contains the declaration of functions, and shared global variables, but not their definition.
        May also contain various `typedef`s.
        To be `#include`d by any file that wants to use those functions, etc.
    
    source file
    :   Contains the definition of closely related functions and global variables.
        May also declare helper functions that are not exported in a header file.
    
    dependency
    :   A separate project that creates a library used by this project
    
    Projects generally are designed to either create a library or a program, but not both.

Creating a shared library
:   1. Compile each `.c` file into a `.o` file; use the `-fPIC` compiler flag so the resulting code can be shared by multiple applications.
    2. Use the `ld` tool to combine the `.o` files. Since this is tricky to get right, your compiler (`gcc` or `clang`) will have a simpler interface that calls `ld` under the hood, generally like
        
        ````bash
        clang -shared \
            all.o your.o object.o files.o \
            -o libyourname.so
        ````

        Note that loaders often assume that shared library names begin `lib` and end `.so`, so you should abide by that pattern.
    
Creating a static library
:   1. Compile each `.c` file into a `.o` file.
    2. Use the `ar` tool to turn a collection of `.o` files into an `.a` file, generally like
        
        ````bash
        ar rcs \
            libyouname.a \
            all.o your.o object.o files.o
        ````

        Note that linkers often assume that static library names begin `lib` and end `.a`, so you should abide by that pattern.

Creating a program
:   1. Compile each `.c` file into a `.o` file.
    2. Link your `.o` files and `.a` files together, with references to any needed `.so` files.
        
        The tool that actually does this is `ld`, but you should use your compiler's wrapper around that instead.
        The compiler needs to be told all your `.o` files explicitly,
        as well as the "library path" (directories where library files are located; specified with `-L`)
        and "libraries" (the part of library names between `lib` and `.`; specified with `-l`)
        
        For example, the following code
        
        ````bash
        clang \
            all.o your.o object.o files.o \
            -L../mylib -Lanother \
            -lflub -lflux -lflibber \
            -o runme
        ````
        
        will create a program named `runme`
        by combining `all.o`, `your.o`, `object.o`, and `files.o`
        with `libflub.so` or `libflub.a` (whichever it can find),
        `libflux.so` or `libflux.a` (whichever it can find), and
        `libflibber.so` or `libflibber.a` (whichever it can find),
        looking for each library in the directories `../mylib/` and `another/`
        as well as the system-wide library path (usually `/usr/lib/` and `/usr/local/lib/`).

## Organizing a project
