---
title: Processes and threads
...

# Terminology

**Multitasking** is a generic term for having multiple flows through code occurring concurrently.

**Preemptive multitasking** is multitasking where the scheduling of which task has access to the processor and related resources is controlled by the OS, hardware, or other system external to the task itself
and the task's access to them can be *preempted* or taken away from it by an asynchronous event like an interrupt.

**Cooperative multitasking** is multitasking where the scheduling of which task has access to the processor and related resources is controlled by a library that must be manually invoked by code that is willing to lose control of the resource.

## Preemptive multitasking


A **process** has a full compliment of private state:
program registers, condition codes, virtual address space, kernel-managed resources like file descriptors, etc.

A **thread** has a limited compliment of private state:
primarily just program registers and condition codes.
All the threads of a given process share the same virtual address space, kernel-managed resources like file descriptors, etc.
Because the stack is managed by a program register (`%rsp`), this means each thread has its own stack.
Because heap, code, and globals are stored at known virtual addresses,
that memory is shared by all threads.

Both processes and threads are schedule by the operating system, with periodic interrupts being used to switch between them preemptively.
On some hardware, two threads can be scheduled on the same core in parallel using "hyperthreading;" this involves creating a limited amount of duplicated state on the processor, and generally does not work for processes, only threads.

## Cooperative multitasking

Some operating systems (including 1980s versions of Windows and Mac OS, as well as some modern systems for resource-constrained hardware) use cooperative multitasking for processes;
this means, in particular, that if a process enters an infinite loop or otherwise runs forever without using the "give up control" system call, the entire computer can freeze.

More commonly today, cooperative multitasking is used for user-mode-only event libraries.
Being user-mode software, these are relatively easy to implement and change, resulting in many names (fibers, tasklets, coroutines, promises, futures, the asyc/await pattern, event libraries, continuation-passing style, etc.) and nuances of detail.

# `fork` et al

The most common functions for handling processes are `fork` and `waitpid`:
`fork` creates a new process and `waitpid` removes one.

## New process, what's in memory?

When creating a new process, a challenge exists:
since it has its own address space, what do we put in that memory?
In particular, what code will it run?

We could say "you can only create a process if you give it a file containing memory contents to initialize it with"
but that decision ties the ability to create processes
to the file format of a new process's memory, which is an unnecessary limitation.

As an alternative, we could say "the new process has a copy of the same memory as the old."
That way we can have code that wants to create a new process set up its memory and code arbitrarily.
Conversely, this involves copying a lot of memory...

Fortunately, we can avoid copying by adding a "copy-on-write" bit to each page table entry.
If this bit is set and we try to write to that page of memory,
the MMU first duplicates the page table and^[This is an oversimplification. What if three processes were sharing the page? Keeping a count of number of copies needed works better, but that count has to be placed in a finite-sized field with special logic for it that overflows... The full details are a bit distracting, hence the simple "one bit" discussion above.] reset the bit to 0.
This way whichever process writes to that page first gets its own copy and if neither write, only read, then no copying is needed.

The most common implementation is the "copy of memory" approach, implemented by `fork`.
when new contents of memory are desired, the `fork` is immediately followed by a system call to replace the entire contents of memory (i.e., via `execve` or its friends).

## Using `fork`

The `fork` function wraps the process-creating system call.
It's semantics are you invoke it once and it returns twice, once in each process.
We need the two to do different things next (else what was the point of `fork`ing?)
so each return has a different return value:

- One process (traditionally called the **child**) has a return value of 0
- The other process (traditionally called the **parent**) has as its return value the unique integer the OS uses to identify the child process.

{.aside ...} **Fork bombing**

Consider (but **never run!**) the following code:

````c
for(int i=0; i<30; i+=1)
    fork();
````

This code runs a loop 30 times, but each run doubles the number of processes.
If we try to work it out in low-level step-by-step action:

Processes                   Code        `i`
--------------------------- --------    ---
1                           `i=0`       0
1                           `i<30`      
1 → 2                       `fork()`
2                           `i+=1`      1
2                           `i<30`
2 → 4                       `fork()`
4                           `i+=1`      2
4                           `i<30`
4 → 8                       `fork()`
8                           `i+=1`      3
8                           `i<30`
8 → 16                      `fork()`
16                          `i+=1`      4
16                          `i<30`
16 → 32                     `fork()`
32                          `i+=1`      5
                            ...
268,435,456                 `i<30`
268,435,456 → 536,870,912   `fork()`    
536,870,912                 `i+=1`      29
536,870,912                 `i<30`
536,870,912 → 1,073,741,824 `fork()`    
1,073,741,824               `i+=1`      30
1,073,741,824               `i<30`

... we see that we'd need over 1 billion processes.
The OS cannot handle anywhere near that many; it will generally crash,
but might first start swapping out the pages of kernel memory that store the list of processes,
resulting in many thousands of times slowdown on all operations.

This is called a "fork bomb" and is a common rookie mistake that results in freezing or crashing an entire computer.
{/}

## Replacing memory

Because copy-on-write makes `fork` an efficient and simple way to make new processes, the usual pattern for launching a new program begins is

1. The launcher `fork`s.
2. The child process changes its memory to the new code using `execve` or its relatives.

The `execve` program is defined as follows:

```c
#include <unistd.h>

int execve(const char *filename, char *const argv[], char *const envp[]);
```

`filename`
:   The path of a binary executable.

`argv`
:   An array of argument strings passed to the new program.
    By convention, the first of  these  strings (i.e., `argv[0]`) should contain the filename associated with the file being executed.

    The `argv` array must include a `NULL` pointer to mark the end of the array.

`envp`
:   An array of strings, conventionally of the form `key=value`,
    which are passed as environment^[We have not spent much time on environments, but every program has access to global array of strings `extern char **environ`{.c} which is used to communicate such things as what directories should be searched for executable and where windows should be opened. See `man 7 environ`{.bash} for more.] to the new program.
    
    The `envp` array must include a `NULL` pointer to mark the end of the array.

Assuming there are no errors in the arguments, `execve` does not return;
instead, it overwrites all of user-space memory with the memory contents described in the binary executable file and jumps to the initial instruction of the newly-loaded code.

## Using `waitpid`

A common use of `fork` is from a terminal, shell, or graphical launcher.
In this situation, the basic flow is

1. The launcher forks.
2. The child process changes its memory to the new code.
3. The child process runs.
4. The child process terminates.
5. The launcher displays something if the child crashed instead of terminating gracefully.

The first four of these steps can be done with `fork` alone,
but the last requires some way for the launcher to be notified when a child terminates, with information about how it ended.

When a process ends, it becomes a **zombie**---still listed in the process list in the OS, but its memory is freed and it is not ever scheduled.
If a parent process has a zombie child, it can **reap** that process by using the `waitpid` function.
This can be run in several ways, as documented in `man waitpid`{.bash},
but they all involve removing one zombie process from the list of processes in the OS and returning some information about how long it ran, what exit code it provided, etc.

{.example ...} The following program
```c
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        printf("This is the child process\n");
        return 12;
    } else {
        printf("This is the parent process\n");
        int status;
        waitpid(pid, &status, 0);
        printf("Child exited with status code: %d\n", WEXITSTATUS(status));
        return 0;
    }
}
```

when compiled and run prints either

    This is the parent process
    This is the child process
    Child exited with status code: 12

or

    This is the child process
    This is the parent process
    Child exited with status code: 12

depending on if the parent or child gets to its first `printf` first.

The `status` integer also has other information, such as if it was terminated with a signal and if so which signal, which can be accessed with other macros instead of `WEXITSTATUS`.
{/}

# Using `pthreads`

The POSIX standard defined a set of interfaces for handling threads,
which are implemented as a native threading mechanism on Linux and OS X
and as an optional mechanism on Windows.
POSIX threads, often abbreviated pthreads,
are complicated enough to deserve [their own writeup](pthreads.html)
The POSIX Thread Library (often PThreads) 

