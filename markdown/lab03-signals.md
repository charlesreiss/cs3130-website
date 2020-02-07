---
title: Signals Lab
...

In this lab you'll write yet another chat application.
It will only work on a **single machine**^[If you are using ssh, this means you need to ssh into the same back-end server twice. To do this first SSH into portal.cs.virginia.edu; then run `uname -n` to find out what server you are on (portal## for some two-digit number ##) and have your second SSH go directly into that portal##.cs.virginia.edu.], not over a network file system or the Internet,
but it will also require significantly fewer system resources than our other chats.

{.exercise ...}
Do the following. We recommend doing them in order. Each is described in more detail below.

We strongly recommend working in pairs on this lab.

1. Make a program that prints its own PID and reads another PID from the user (store it in a global variable)
2. Create a signal handler for the SIGINT, SIGTERM, and SIGUSR1 signals
    - SIGTERM should invoke a cleanup function
    - SIGINT should  invoke a cleanup function and then sent SIGTERM to the other user
    - SIGUSR1 should display the contents of the inbox and then set its first character to be `'\0'`
3. Allocate some shared memory
    - An inbox, which this process will deallocate when it exits
    - An outbox, which it will not
3. Repeat until the end of file:
    - Read a typed line into the outbox (without overflow)
    - wait until the outbox is emptied
4. After the user stops typing (i.e., EOF), send SIGTERM to the other process

Also make a cleanup function which

1. detaches both inbox and outbox
1. destroys both inbox and outbox

The cleanup function should execute no matter how the program exists:
via end-of-input, SIGINT, or SIGTERM.
{/}

# PID

`getpid()` (from `unistd.h`) returns the PID of the current process as a `pid_t`, which can be treated like an `int` in most situations.
This can be printed (e.g., with `printf`) by one process and input to the other (e.g., with `scanf`)

# Signals

As shown in class, the key parts of signal handling are defining a handler function

```c
#include <signal.h>

static void your_handler_function_name_here(int signum) {
    if (signum == SIGINT) /* ... */
}
```

and the code in `main` to tell the OS to use it and for which signals:

```c
struct sigaction sa;
sa.sa_handler = handler;
sigemptyset(&sa.sa_mask);
sa.sa_flags = SA_RESTART;

sigaction(SIGINT, &sa, NULL);
sigaction(SIGTERM, &sa, NULL);
/* ... */
```

To send a signal, use `kill(pid_to_send_to, SIGTERM)` or the like.

# Shared memory

One of the advantages of virtual memory is the OS can map the same physical page to different virtual pages in different processes.
The processes can then directly communicate by loading what the other stored in that shared memory.

Because shared memory needs to be able to span processes, it is a bit more involved to create and destroy than ordinary memory is. You need a key, an identifier, and a pointer, as follows:

1. A key (type `key_t`) is generated via `ftok(path, num)` (from `sys/ipc.h`);
    we'll use `argv[0]` as the path and `1` and `2` as the `num`s:
    i.e., `ftok(argv[0], 1)`{.c} and `ftok(argv[0], 2)`{.c}.
    Box 1 will be the inbox of the smaller-PID program and the outbox of the larger-PID program;
    box 2 will be the inbox of the larger-PID program and the outbox of the smaller-PID program.

2. An identifier. The OS allocated a physical page and gives us an integer to identify it.
    
    To allocate, `inbox_id = shmget(inbox_key, box_size, 0666 | IPC_CREAT);`{.c}
    
    To deallocate, `shmctl(inbox_id, IPC_RMID, NULL);`{.c}
    
    Shared memory regions can persist even after the creating program terminates, so **always deallocate what you allocate**!
    Even if your program stops early (e.g., by SIGINT) you still need to deallocate.
    It does not hurt (though does set `errno`) to deallocate multiple times
    or from the wrong process.

3. A pointer. You attach a pointer to shared memory using `shmat` and detach it with `shmdt`:

    ````c
    inbox = (char*)shmat(in_seg, NULL, 0);
    // ...
    shmdt(inbox);
    ````

# Repeating and waiting

You've got two things to do: keyboard ↦ outbox and inbox ↦ screen.

## keyboard ↦ outbox

A loop in `main` is the way to go: `while(!feof(stdin))`{.c} you want to `fgets` directly into the outbox.
After data is in the outbox, send SIGUSR1 (using `kill`) to the other process then wait for `outbox[0]`{.c} to stop being `'\0'`{.c}
Don't just busy-wait though: check, then go to sleep for a bit, then check again.

```c
// wait for '\0' by checking 100 times a second
// 10,000 μs = 10 ms = 1/100 second
while(outbox[0]) { usleep(10000); }
```

## inbox ↦ screen

When you handle `SIGUSR1`

1. `fputs` the inbox so we can see what was sent;
2. `fflush(stdin)`{.c} to ensure we see it now, not later;
3. set the first character in the inbox to `'\0'`{.c} so the other process knows we're ready for it to send more

