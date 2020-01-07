---
title: Minimal makefiles
...

In this class, you will generally submit multiple files for each assignment
with an accompanying makefile to help us run them.
Makefiles are quite powerful and, while sometimes considered antiquated compared to tools like [cmake](https://cmake.org), [gradle](https://gradle.org), and [scons](https://scons.org),
it remains the dominant build automation tool in the GNU and Linux ecosystems.
You're welcome to [read a little more](https://uva-cs.github.io/pdr/tutorials/05-make/index.html) or [read a lot more](https://www.gnu.org/software/make/manual/)
than this one example summary if you wish.


The following is a base makefile for our course

````makefile
# set up how compilation will happen
CC = clang
CFLAGS = -g -O1
LDFLAGS = 

# define the set of files used
objects = foo.o bar.o



# default to building everything, running nothing
all: runner tester



# Create a runner target ...
runner: main.o $(objects)
	$(CC) $(LDFLAGS) $^ -o $@

# ... and a target that runs it
run: runner
	./runner



# Create a tester target ...
tester: tester.o $(objects)
	$(CC) $(LDFLAGS) $^ -o $@

# ... and a target that runs it
test: tester
	./tester



# generic rule to build a .o from any .c
# see https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html
%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@



# something to remove files the makefile created
clean:
	rm -f runner tester main.o tester.o $(objects)

# mark a few targets as not producing files
.PHONEY: all run test clean 
````

A toy project that uses this makefile [is available](files/make_example.tar)
and can be expanded from the command line with `tar vxf make_example.tar`{.bash}
