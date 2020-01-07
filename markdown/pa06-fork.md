---
title: Fork and run
...

Write and submit a single file `fork_run.c`
that defines the following two functions.
You should not submit any other files (i.e., no `.h` files, `Makefile`, etc).
You may include helper functions in the file,
but it should not contain `main`.

# `int my_system(const char *command)`

This is a simplified version of the `system` standard library function (see `man 3 system`{.bash} for details). You should handle all aspects of the function as described in the manual page **except**

1. you do not need to ignore or block any signals
1. you do not need to check "if a shell is available" for `NULL` commands

You must not use `system` in your implementation.
You should use `fork`; `wait` or `waitpid`; and either `execve` or one of its front-ends (`execl`, `execlp`, `execle`, `execv`, `execvp`, or `execvpe`).

Because `my_system` needs to be thread-safe, do not use any global variables.

{.example ...} The following main function

```c
int main(int argc, const char *argv[]) {
    int a1 = my_system("sleep 1; echo hi");
    int a2 = my_system("echo bye");
    int a3 = my_system("flibbertigibbet 23");
    printf("%d %d %d\n", 
        WEXITSTATUS(a1), WEXITSTATUS(a2), WEXITSTATUS(a3));
}
```

should print

    hi
    bye
    sh: 1: flibbertigibbet: not found
    0 0 127

(note: the details of the "not found" line will vary based on the version of `sh` installed on the computer)
{/}

{.example ...} The following main function

```c
int main(int argc, const char *argv[]) {
    my_system("echo -n 'type something: ';"
        " read got;"
        " echo Thanks for typing \\\"\"$got\"\\\"");
}
```

should prompt for user input, wait until it is provided, and then repeat what they typed, as e.g.

    type something: this is a test
    Thanks for typing "this is a test"
{/}


# `char *getoutput(const char *command)`

This should behave something like `system`, except that instead of letting the child print to stdout, it should collect what the child prints and return it as a `malloc`-allocated `char *`.

I am not aware of a standard library function that does this, but if you find one do not use it; do this by forking, execing, and piping yourself.

{.example ...} The following main function

```c
int main() {
    printf("Hi!\n");
    printf("Text: [[[%s]]]\n", 
        getoutput("echo 1 2 3; sleep 2; echo 5 5"));
    printf("Bye!\n");
}
```

Should print

    Hi!

then wait for 2 seconds before printing

    Text: [[[1 2 3
    5 5
    ]]]
    Bye!

Note that this `main` also has a memory leak: `my_getoutput` invokes `malloc` and `main` is not invoking `free`.
{/}

To do this, use the following outline.

1. Create a **pipe**.
    A pipe looks like a pair of file decriptors,
    one opened for reading and the other for writing,
    and is a tool used extensively to help processes talk to each other.
    See `man 2 pipe`{.bash} for details.
    
    Make sure you invoke `pipe` *before* you invoke `fork` so that both processes have access to the same pipe.

2. In the child, 
    
    a. replace stdout with the write-end of the pipe.
        The `dup2` command is used for this,
        copying one file descriptor with a new number.
        You want to copy the write-end of the pipe to `1`, stdout.
    b. close both of the pipe file descriptors.
        You don't need the read end in the child,
        and the write-end is now duplicated as stdout.
    c. exec, etc, as you did for `my_system`.

3. In the parent,
    
    a. close the write end of the pipe -- only the child needs that.
    b. read all contents from the read end of the pipe,
        `malloc`ing enough space to store it all.
    c. close the read end of the pipe when you are done reading.
    d. `waitpid` on the child *after* reading everything.
        This is not strictly necessary (you can wait first and it should work),
        but waiting after reading lets the OS
        send information to the parent as it arrives
        instead of buffering it all in kernel memory first.

You may assume that the command exists and executes normally; no need to add any error-handling logic.

As a tip, one of the easiest ways to read everything there is to read
is to use `getdelim`{.c} with the delimiter `'\0'`{.c}.
`getdelim`{.c} wants a `FILE *`{.c}, not a file descriptor;
see `fdopen`{.c} for how to wrap a file descriptor in a `FILE *`.
