---
title: Make lab
...

This lab is designed to help you feel comfortable using GNU Make, one of the oldest and still most-common build tools; as well as with many-file C projects.

{.exercise ...} Your task:

With a partner (unless you are a loner by choice),

1. Download [`lab01-make.tar`](files/lab01-make.tar) (you can do this using the link on this page, or from a command line via `wget http://www.cs.virginia.edu/luther/COA2/F2020/files/lab01-make.tar)`{.bash})

2. Expand it using `tar xvf lab01-make.tar`{.bash}

3. Read enough of the files to get an idea what they do. Then build them with `clang *.c` and run `./a.out` to verify your understanding. Ask a TA for help if you are not sure you fully understand.

4. Add a `Makefile` in the resulting `lab01-make` directory, which
    - has targets for the following
        - build each `.c` file into a `.o` file
        - combine `cheer.o` and `grunt.o` into a library (either `libsay.a` or `libsay.so`)
        - combine the library and `guesser.o` into program `guesser`
    - has `CC`, `CFLAGS`, and `LDFLAGS` we can edit
    - `make all` creates `guesser` and its dependencies
    - `make clean` deletes all files that `make` can recreate
    - editing `say.h` causes all files to be rebuilt
    - editing any .c file causes its .o file to be rebuilt, along with the library and/or program if they depend on the .c file
        - Note: if you chose library format `libsay.a`, then `guesser` should be rebuilt if `libsay.a` changes;
            but if you chose library format `libsay.so`, then `guesser` should not be rebuilt if `libsay.so` changes, only if `say.h` or `guesser.c` change.

5. Show a TA your `Makefile`. They may ask you to show it working, or to look at its contents, or both.
{/}

# Overview

`make` is a took that reads a file (called a makefile, named `Makefile` be default) and decides what commands it need to run to update your project.
A well-constructed makefile can simplify building and running code on many platforms.

## Rules

The key component of a makefile is a **rule**, consisting of three parts:

1. A **target**, the name of a file that will be created or updated by the rule.
1. A list of **dependencies** (or "prerequisites"): if the *target* is missing or is older than at least one dependency, the *system commands* will be run.
1. A list of **system commands** (or the "recipe"): shell commands that should result in (re)making the *target*

The *target* must begin a line (i.e., must not be indented) and end in a colon.
Anything after that colon is a *dependency*.
Indented lines that follow (which must be indented with a single tab character, not spaces) are *systems commands*.

{.example ...} The following rule will build `hello.o` if either `hello.c` or `hello.h` is newer than `hello.o`.
Otherwise, it will do nothing
```makefile
hello.o: hello.c hello.h
	clang -c hello.c
```
{/}

When you run `make` it reads `Makefile` and executes the *first* rule it finds.
If you want to run a different rule, you can give its target as an argument to `make`.

{.example ...} Consider the following `Makefile`:
```makefile
hello.o: hello.c hello.h
	clang -c hello.c

bye.o: bye.c bye.h
	clang -c bye.c
```

Running `make` will ensure `hello.o` is up to date.
Running `make bye.o` will ensure `bye.o` is up to date.
{/}

If a dependency of an executed rule is the target of another rule, that other rule will be executed first.

{.example ...} Consider the following `Makefile`:
```makefile
runme: hello.o bye.o main.c main.h
    clang main.c hello.o bye.o -o runme

hello.o: hello.c hello.h
	clang -c hello.c

bye.o: bye.c bye.h
	clang -c bye.c
```

Running `make` will ensure `runme` is up to date;
since `runme`'s dependencies include `hello.o` and `bye.o`, both of those rules will also be executed.
{/}

## Inference rules

Often we want to do similar work for a large set of files. 

## Macros

Makefiles routinely use variables (which they call macros) to separate out the list of files from the rules that make them, as well as to allow easy swapping out of different compilers, compilation flags, etc.

Variables a defined with `NAME := meaning`{.makefile} syntax. Traditionally, variable names are in all-caps.
Variables are used by placing them in parentheses, preceded by a dollar sign: `$(NAME)`.

Most makefiles define at least the following variables:

Name        example     notes
--------    ----------  ----------------------------------
CC          clang       The C compiler to use
CFLAGS      -O2 -g      Compile-time flags for the compiler
LDFLAGS     -lm         Link-time flags for the compiler
--------    ----------  ----------------------------------

Even if you need no linker flags, it's still common to define a blank `LDFLAGS :=` and use `$(LDFLAGS)` in all linking locations so that if you later realize you need to link to an external library (like the math library, `-lm`), you can easily do so.

## And beyond

`make` has several special features, such as superpowered "internal macros", special archive suffixes, etc.
Two of these we will find useful:

You can have a (set of) targets that do not represent files, commonly including `all` and `clean`.
These are specified by having the line `.PHONEY: all clean`, and then using `all` and `clean` like regular targets.
You *should* always have your first rule be named `all` and have it do the "main" task (i.e., building the program or library you are providing).
You *should* always have a rule named `clean` that removes all files that can be build by `make`.

You will often want a few "pattern rules" using a few "automatic variables". There are a lot of things [you could learn](https://www.gnu.org/software/make/manual/make.html#Pattern-Rules) about these, but a simple version will often suffice:

- Use `%.ending` as a target. `%` is a wild-card and can represent any name.
- Use `%.other_ending` as a dependency. `%` has the same meaning it did in the target.
- Use `$<` in the system commands as the name of the first dependency.
- Use `$@` in the system commands as the name of the target.

{.example ...}
Many makefiles will include a command like

```makefile
%.o: %.c %.h config.h
	$(CC) -c $(CFLAGS) $< -o $@
```

In other words, 

Part            Meaning
-------         -------------------------------
`%.o`           To make any .o file
`: %.c`         from a C file of the same name
`%.h config.h`  (with its .h file and the file `config.h` as extra dependencies)
`$(CC)`         use our C compiler
`$(CFLAGS)`     with our compile-time flags
`$<`            to build that .c file
`-o $@`         and name the result the name of our target.
{/}



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
    This lab is an exception, having both a library and a program using it in one project to keep the lab short enough to be manageable.

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
    
    Alternatively, `make` provides special syntax for making a library:
    
    1. Use the library name as the target
    2. Use the library name followed by an object file in parentheses as the dependencies
    3. Use the command `ranlib` in the system command.
    
    ````makefile
    libname.a: libname.a(your.o) libname.a(object.o) libname.a(files.o)
        ranlib libname.a
    ````

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
            -L../mylib -L./ \
            -lflub -lflux -lflibber \
            -o runme
        ````
        
        will create a program named `runme`
        by combining `all.o`, `your.o`, `object.o`, and `files.o`
        with `libflub.so` or `libflub.a` (whichever it can find),
        `libflux.so` or `libflux.a` (whichever it can find), and
        `libflibber.so` or `libflibber.a` (whichever it can find),
        looking for each library in the directories `../mylib/` and `./`
        as well as the system-wide library path (usually `/usr/lib/` and `/usr/local/lib/`).

