---
title: Kernels -- Software for the Hardware
...


# Kernel Mode vs. User Mode

All multi-purposed chips today have (at least) two modes in which they can operate:
user mode and kernel mode.
Thus far you have probably only written user-mode software, and most of you will never write kernel-mode software during your entire software development careers.

## Motivation

Some activities the computer does can only be done by kernel-mode software;
if user-mode code wants to do them, it has to do so by asking the kernel.
This restriction provides several advantages.

### Limiting Bugs' Potential for Evil

One reason to not run in kernel mode is to limit the scope of mistakes a software bug can have.
Inside the computer is all of the code and data that handles drawing the screen and tracking the mouse and reacting to keys and so on; I'd rather not be able to make a mistake when writing my Hello World program and accidentally mess up those core functionalities.
By running in user mode, if my code tries to touch any of that it will be blocked from doing so: my program will crash, but the computer will keep running.

### Each Program Thinks it is In Control

Physical computers have limited physical resources.
One of the benefits of running in user mode is that the particular limitations are hidden from you by the kernel and hardware working together.
For example, even if you only have 8GB of memory you can still use addresses larger than that limit: the kernel and hardware will work together to provide the illusion that there was enough memory even if there was not.

This illusion is even more important when you are running more than one process simultaneously.
Perhaps you have a terminal, an editor, and your developed code all running at once.
The kernel and hardware will work together to ensure that your code cannot even see, let alone modify, the memory being used by the terminal or editor.
Each program is given the illusion of being the only one running on an unlimited machine.

### Wrapper around Hardware

Each piece of hardware attached to a chip has its own protocols and nuances of operation.
Fortunately, only the kernel needs to know about those details.
If the mouse tries to talk to your program directly, the kernel intercepts the communication and handles it for you so you don't notice it happened unless you asked the kernel to inform you about it.
If you try to talk to the disk, the kernel likewise moderates that communication, keeping you from needing to know the specific protocol the currently-attached disk expects.

### Multiple Users

Because the kernel is an unavoidable intermediary for talking to hardware, it can chose to forbid some interactions.
One common use of this is to allow multiple user accounts.
Since the kernel is the one that controls access to the disk, for example, it can chose not to allow my processes to access the part of the disk it has assigned to you, not me.


## Implementation

### Protected Instructions and Memory

When in user mode, there are a set of instructions that cannot be executed
and segments of memory that cannot be accessed.
Examples of things you cannot directly access in user mode include

- the part of memory that stores information about which user account you have logged into
- instructions that send signals out of the chip to communicate with disks, networks, etc
- instructions that let user-mode code execute in kernel-mode

### Mode Bit

The simplest way to create two modes is to have a single-bit register somewhere on the chip
that indicates if the chip is currently executing in user mode or in kernel mode.
Each protected instruction then checks the contents of that register and, if it indicates user mode, causes an exception instead of executing the instruction.
A special protected instruction is added to change the contents of this mode register,
meaning kernel-mode code can become user-mode, but no vice versa.

Modern processors often have more than one operating mode with more than one level of privilege,
which (among other benefits) can help make virtualization efficient;
the details of these additional modes are beyond the scope of this course.

### Mode-switch via Exceptions

The core mechanic for becoming the kernel is a hardware exception.
An exception results in several functionally-simultaneous changes to processor state,
notably including both (a) changing to kernel mode and (b) jumping to code in kernel-only memory.
Thus, the only mechanisms that exist for entering kernel mode will running kernel code for the next instruction, preventing user code from running itself in kernel mode.

The nature of these hardware exceptions and the jump to kernel code that is associated with them is a large enough topic to deserve [it's own section](#exceptions).

# Exceptions

Hardware exceptions are examples of "exceptional control flow":
that is, the sequence of instructions being operated around an exception
is an exception to the usual rules of sequentiality and jumps.
They also tend to appear in exceptional (that is, unusual) situations.

## Kinds

There are several kinds of hardware exceptions.

### Interrupts

An interrupt occurs independently from the code being executed when it occurs.
It might result from the mouse moving, a network packet arriving, or the like.

Interrupts are typically handled by the kernel in a way that is invisible to the user code.
The user code is frozen, the interrupt handled, and then the user code resumed as if nothing had happened.

{.aside} Some sources call all exceptions "interrupts," calling the interrupting-type of exception an "asynchronous interrupt" instead.

### Faults

A fault is the result of an instruction failing to succeed in its execution.
Examples include dividing by zero, dereferencing a null pointer, or attempting to execute a protected instruction while in user mode.

There are two basic families of responses to faults:
either the kernel can fix the problem, then re-run the user code that generated the fault;
or it cannot fix the problem and kills the process that cause the fault instead.
In practice, fixable faults happen quite often, notably the [page fault](#pages) discussed later.
Even faults the kernel can't fix on its own are often handled by asking the user code if it knows how to fix them using a mechanism called [signals](#signals), though many programs do not chose to handle the signals so the end result is often still termination of the fault-generating process.

### Traps

A trap is an exception caused by a special instruction whose sole job is to generate exceptions.
Traps are the main mechanism for intentionally switching from user mode to kernel mode and are the core of all system calls.
System calls are used to interact with the file system, spawn and wait on threads, listen for key presses, and anything else that you cannot do with simple code alone.

## Handling

When an exception occurs, the processor switches to kernel mode and jumps to a special function in kernel code called an "exception handler."
Because interrupts exist, this can happen without any particular instruction causing the jump,
and thus might happen at any point during the code running.
In order for the handler to later resume user code, the exception handling process must also save the processor state before executing the handler.

### Save-Handle-Restore

The basic mechanism for any exception to be handled is

1. Save the current state of the processor (register contents, PC location, etc)
2. Enter kernel mode
3. Jump to code designed to react to the exception in question, called an **exception handler**
4. When the handler finishes, enter user mode and restore processor state (including register contents, PC location, etc)

These steps (except for the actual execution of the exception handler) are all done atomically by the hardware.

### Which Handler?

In general, there can be as many different exception handlers as there are exception causes.
To select which one to run, the hardware consults something called an **exception table**.
The exception table is just an array of code addresses; the index into that array is determined by the kind of exception generated (e.g., divide-by-zero uses index 0, invalid instructions use index 6, etc.)
The index is called an **exception number** or **vector number** and the array of code addresses is called the **exception table**.

{.aside ...} Switches

Having an array of code addresses is not unique to exception handlers; it is also present in C in the form of a `switch`{.c} statement.

For example, the following C

````c
switch(x) {
    case 0: f0();
    case 1: f1();
    case 2: f2();
    case 3: f3();
        break;
    case 4: f4();
    case 5: f5();
    case 6: f6();
}
````

compiles down to the following code

````asm
    cmpl    $6, %eax
    ja      AfterSwitch
    jmpq    * Table(,%rax,8)
Case0:
    callq   f0
Case1:
    callq   f1
Case2:
    callq   f2
Case3:
    callq   f3
    jmp     AfterSwitch
Case4:
    callq   f4
Case5:
    callq   f5
Case6:
    callq   f6
AfterSwitch:
````

supported by the following jump table

````asm
    .section    .rodata,"a",@progbits
Table:
    .quad   Case0
    .quad   Case1
    .quad   Case2
    .quad   Case3
    .quad   Case4
    .quad   Case5
    .quad   Case6
````

Exception tables are just one particular use of this array-of-code-addresses idea.
{/}

### After the Handler

Handlers may either abort the user code or resume its operation.
Aborting is primarily used when there is no obvious way to recover from the cause of the exception.

When resuming, the handler can either re-run the instruction that was running when it was generated
or continue with the next instruction instead.
A trap, for example, is similar to a `callq` in its behavior
and thus resumes with the subsequent instruction.
A fault handler, on the other hand, is supposed to remove the barrier to success that caused the fault
and thus generally re-runs the faulting instruction.


{.aside ...} Aborts

We [classified exceptions by cause](#kinds), but some texts classify them by result instead.
If classified by what happens after the handler, there is a fourth class: aborts, which never return.

Fault
:   re-runs triggering instruction

Trap
:   runs instruction after triggering instruction

Interrupt
:   runs each instruction once (has no triggering instruction)

Abort
:   stops running instructions

The abort result may be triggered by any cause:
if a memory access detects memory inconsistency we have an aborting fault;
an exit system call is an aborting trap;
and integral peripherals like RAM can send aborting interrupts if they notice unrecoverable problems.
{/}



### Example: Linux system calls

In Linux, almost all communications from user- to kernel-code are handled by a trap with exception number 128.
Instead of using exception number to select the particular action the user is requesting of the kernel, that action is put in a number in `%rax`;
this decision allows Linux to have space for billions of different system calls instead of just the 200 or so that would fit in an exception number and are not reserved by the hardware.

To see a list of Linux system calls, see `man 2 syscall`{.bash}.
Most of these are wrapped in their own library function, listed in `man 2 syscalls`{.bash}

{.example ...}
Consider the C library function `socket`.
The assembly for this function (as compiled into `libc.so`) begins

````asm
socket:
    endbr64 
    mov    $0x29,%eax
    syscall 
````

Let's walk through this a bit at a time:

`endbr64`{.asm}
:   This is part of the [control-flow enforcement](https://software.intel.com/sites/default/files/managed/4d/2a/control-flow-enforcement-technology-preview.pdf) elements of x86-64.
    It has no outwardly visible impact on program functionality,
    but it does add some security enforcement,
    making it harder for some classes of malicious code
    to access this function.
    
    Omitting some (important but tedious to describe) details,
    because this instruction is present in the function
    you can't jump into the code except at that line.
    That means malicious code can't jump a bit later into this function
    with the goal of executing the `syscall` instruction
    without first going through the `%rax`-setting instruction.

`mov $0x29,%eax`{.asm}
:   Places 29~16~ (41~10~) into `%rax`. 
    Per `/usr/include/sys/syscal.h`, 41 is the number of the "socket" syscall.

`syscall`{.asm}
:   A trap, generating exception number 128.
    This means the hardware saves processor state,
    then jumps to the address stored in the kernel's `exception_handler[128]`.
    
    The exception handler at index 128 checks that `%rax` contains a valid system call number (which 41 is),
    after which it jumps to the kernel's `system_call_handler[41]`, the address of the socket call implementation.
    
    The kernel's system calling convention,
    which has the same first three arguments as C's calling convention,
    so it has access to the arguments from the `int socket(int domain, int type, int protocol)`{.c} invocation.
    It uses them to do whatever work is needed to create a socket,
    placing its file descriptor in `%rax` to be a return value.
    
    The handler ends by executing a protected return-from-handler instruction
    that 
    
    - restores processor state (but leaves `%rax` alone as a return value),
    - returns to user mode, and
    - jumps back to user code
    
After that is some error checking code, and then the function returns. The whole function's only 11 instructions (24 bytes) long.
{/}

## Exception-Like Constructs

### Signals

One view of exceptions is that they enable communication from user code to the kernel.
Signals permit the opposite direction of communication.

                     → User code     → Kernel code   → Hardware
------------------- --------------  --------------- -----------------------
User code →         ---             Trap            _via kernel_
Kernel code →       **Signal**      ---             protected instructions
Hardware →          _via kernel_    Interrupt       ---

Signals are roughly the kernel-to-user equivalent of an interrupt.
At any time, while executing any line of code,
a signal may appear.
It will suspend the currently executing code and see if you've told the kernel
about a signal handler, a special function you believe will handle that signal.
After running the handler, the interrupted code will resume.

Linux defines many different signals, listed in `man 7 signal`{.bash}.
Each of them has a default action if no handler is registered,
most commonly terminating the process.

{.example ...} Typing Ctrl+C on the command line causes the SIGINT signal to be generated.
If we want Ctrl+C to do something else, we have to handle that signal:

````c
#include <stdio.h>
#include <signal.h>

static void handler(int signum) {
    printf("Provide end-of-file to end program.\n");
}

int main() {
    struct sigaction sa;       // how we tell the OS what we want to handle
    sa.sa_handler = handler;   // the function pointer to invoke
    sigemptyset(&sa.sa_mask);  // we'll use sigaction, not signal, so set not needed
    sa.sa_flags = SA_RESTART;  // restart functions that can recognize they were interrupted

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        fprintf(stderr, "unable to override ^C signal");
        return 1;
    }

    char buf[1024];
    while(scanf("%1023s", buf) && !feof(stdin)) {
        printf("token: %s\n", buf);
    }
}
````
{/}

{.example ...} Almost all signals can be overridden, but for many it is not wise to do so.
For example, this code:

````c
#include <stdio.h>
#include <signal.h>

static void handler(int signum) {
    printf("Ignoring segfault.\n");
}

int main() {
    struct sigaction sa;       // how we tell the OS what we want to handle
    sa.sa_handler = handler;   // the function pointer to invoke
    sigemptyset(&sa.sa_mask);  // we'll use sigaction, not signal, so set not needed
    sa.sa_flags = SA_RESTART;  // restart functions that can recognize they were interrupted

    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        fprintf(stderr, "unable to override segfault signal");
        return 1;
    }

    // let's make a segfault -- enters infinite handler loop
    char * buf;
    buf[1234567] = 89;
}
````

... will print "Ignoring segfault" repeatedly until you kill it with something like Ctrl+C.
The last line creates a segfault, which causes the OS to run the handler.
The handler returns normally, so the OS assumes the cause of the fault was fixed and re-runs the triggering code,
which generates another segfault.

Obviously, you should not do something like this in real code.
{/}

There is a system call named "kill" that asks the kernel to send a signal to a different process.
While this can be used for inter-process communication,
better systems (like sockets and pipes) exist for that if it is to be a major part of an application's design.

{.aside ...} Command line and signals

From a bash command line, you can send signals to running processes.

- Ctrl+C sends SIGINT, the "interrupt" signal that means "I want you to stop doing what you are doing."
- Ctrl+Z sends SIGSTOP, a signal that cannot be handled. It always causes the OS to suspend the process, freezing it in place until you ask for it to resume.
- `bg` causes a suspended process to resume running, but in the background so you can use the terminal for other purposes.
- `fg` causes a suspended or background process to resume running in the foreground, re-attaching the keyboard to `stdin`.
- `kill <pid>` sends SIGTERM, the "terminate" signal, to the process with process ID `<pid>`.
    The "terminate" signal that means "I want you to shut down now."
- `kill -9 <pid>` sends SIGKILL, the "kill" signal, to the process with process ID `<pid>`.
    The "kill" signal cannot be handled; it always causes the OS to terminate the program.
    
    Any other signal can be provided in similar fashion, either by number (SIGKILL is signal number 9)
    or name (e.g., `kill -KILL <pid>`)

There are other tools for sending signals, but the above are sufficient for most common use cases.
{/}

### `setjmp`{.c}, `longjmp`{.c}, and software exceptions

Portions of the save- and restore-state functionality used by exception handlers
is exposed by the C library functions `setjmp` and `longjmp`.

`setjump`, given a `jmp_buf` as an argument, fills that structure with information needed to resume execution at that location and then returns 0.
Thereafter `longjmp` can be called with that same `jmp_buf` as an argument;
`longjmp` never returns, instead "returning" from `setjmp` for a second time.
`longjmp` also provides an alternative return value for `setjmp`.

{.example ...} The following program

```c
#include <setjmp.h>
#include <stdio.h>

jmp_buf env;
int n = 0;

void b() {
    printf("b1: n = %d\n", n);
    n += 1;
    printf("b2: n = %d\n", n);
    longjmp(env, 1);
    printf("b3: n = %d\n", n);
    n += 1;
    printf("b4: n = %d\n", n);
}

void a() {
    printf("a1: n = %d\n", n);
    n += 1;
    printf("a2: n = %d\n", n);
    b();
    printf("a3: n = %d\n", n);
    n += 1;
    printf("a4: n = %d\n", n);
}

int main() {
    printf("main1: n = %d\n", n);
    n += 1;
    printf("main2: n = %d\n", n);
    int got = setjmp(env);
    if (got) {
        printf("main3: n = %d\n", n);
        n += 1;
        printf("main4: n = %d\n", n);
    } else {
        printf("main5: n = %d\n", n);
        n += 1;
        printf("main6: n = %d\n", n);
        a();
    }
    printf("end of main\n");
}
```

prints

    main1: n = 0
    main2: n = 1
    main5: n = 1
    main6: n = 2
    a1: n = 2
    a2: n = 3
    b1: n = 3
    b2: n = 4
    main3: n = 4
    main4: n = 5
    end of main

See also a [step-by-step simulation](longjmp.html) of this same process without the `printf`s.
{/}

There was a time when `setjmp`/`longjmp` were seen as effective ways of achieving
`try`/`catch`-like error recovery,
which cannot be handled with simple `goto` because it may skip multiple function call/returns.

+---------------------------------------+---------------------------------------+
| `try`/`catch` constructs              | `setjmp`/`longjmp` parallel           |
+=======================================+=======================================+
| ````java                              | ````c                                 |
| try {                                 | if (!setjmp(env)) {                   |
|     f();                              |     f();                              |
| } catch {                             | } else {                              |
|     g();                              |     g();                              |
| }                                     | }                                     |
| ````                                  | ````                                  |
+---------------------------------------+---------------------------------------+
| `throw new Exception3();`{.java}      | `longjmp(env, 3)`{.c}                 |
+---------------------------------------+---------------------------------------+

More efficient approaches to this have since been developed.
`setjmp` records much of the state of the program in advance,
which gives it a significant cost in time;
because `try` is generally assumed to be executed far more often than `throw`.
Most of the information stored by `setjmp` into the `jmp_buf`
can be found somewhere in the `setjmp`-invoking function's stack frame,
which many languages maintain with sufficient discipline
that it can be "unwound" to restore a previous state upon a `throw`.


# Virtual Memory

One of the most important functions provided by a collaboration
between the hardware and the operating system
is virtual memory.
Virtual memory provides

a. each process with the illusion that it is the only process running.
a. each process with the illusion that all possible addresses exist, no matter how much physical memory is available.
a. memory protection such that user-mode code cannot access kernel memory.
a. an efficient mechanism for communicating with processes

## Processes

A **process** is a software-only operating systems construct
that roughly parallels the end-user idea of a running program.
Each process has its own virtual address space.
The hardware knows the difference between user and kernel mode,
but not the difference between a web browser and a text editor.

The operating system maintains a variety of data structures
inside kernel-only memory to help track different processes.
Portions of those structures relating to one process at a time^[or one per processor depending on the multiprocessor design]
are loaded into hardware-accessible registers and memory
so that the hardware mechanisms will cause running code to interact with the appropriate process's memory
and other state.

Changing which process's state is currently loaded is called a **context switch**.
All major user operating systems (but not all embedded-system operating systems)
use a hardware timer to create an exception every few dozen milliseconds
to enabled automated context switching and facilitate the illusion that
more processes are running at a time than there are processors in the computer.

## Regions of Memory

To provide process separation,
each memory access needs to be checked to ensure it is permitted.
Specific sets of permissions may vary by system, but a common set includes

- Read-only vs. read-write memory (e.g., is `mov %rax, address`{.asm} allowed?) 
- Kernel-only vs. user-mode memory (e.g., is `mov  address, %rax`{.asm} allowed if not in kernel mode?) 
- Executable vs. data-only memory (e.g., is `jmp address` allowed?)
- Valid vs. invalid addresses (e.g., no code can use address 0)

Because most instructions would need to check at least one of these constraints,
there is reason to encode them very efficiently.
Because each process may access a very large number of memory addresses,
they also need to be efficient in space,
in practice meaning that permissions are applied to large contiguous regions of memory.

### Segments revisited

A common operating system internal representation of memory regions
is a list if **segements**,
pairs of starting- and stopping-addresses and a set of permissions for the intermediate addresses.

Segments are never directly visible to the hardware:
instead, the operating system uses the segments
to create initialize hardware-visible [pages table entries](Pages)
and to react to hardware-generated page-related [faults](Faults)
and potentially convert them into [signals](Signals)
to convey to the user process.

{.example ...}
Because segments are a software-only construct,
each operating system can store them in a different way.
One simple example might be the following,

```c
struct segment_node {
    void *low_address;
    void *high_address;
    unsigned int permissions_flags;
    struct segment_node *next;
}
```
{/}


{.aside ...}
It is common to refer to the crashing of a program
that tries to access an invalid address
a "segfault", short for "segmentation fault."
This is technically a misnomer;
the fault is generated by hardware, which does not even know about segments;
the generating fault is either a page fault or a protection fault.
The operating system may react to that fault by generating
a "segmentation violation signal",
often abbreviated as SEGV or SIGSEGV.

Many people who use "segfault"
are aware it is technically incorrect;
it is arguably an example of a computing colloquialism.
{/}



POSIX-compatible operating systems
provide library functions `mmap` and `munmap`
for requesting the operating system adjust its segments.
Typical user applications use higher-level wrappers such as `malloc` instead.

### Pages

Hardware access to memory protections
is designed to be very simple so that it can be very rapid.
Each address is split into two parts:
a **page number** made up of the high-order bits
and a **page offset** made up of the low-order bits.

Each time memory is accessed in any way---including in fetching the instruction itself,
meaning at least once per instruction---the address is **translated**.
Translation converts an address as it appears in code
(called a **virtual address**)
into an address that can be used to access the memory chip
(called a **physical address**).
This translation involves the following steps:

1. The virtual page number is used as a key or index
    into a data structure called a *page table*
    to retrieve a *page table entry*
    (see [Page Tables]).
    Conceptually this is what Java would call a `Map<VirtualPageNumber, PageTableEntry>`{.java}.
    It is stored in memory,
    using physical not virtual addresses
    and its location is stored in a register
    that is changed on each context switch
    so that the operating system maintain a different page table for each process.

2. The **page table entry**
    contains a variety of protection or flag bits.
    In addition to the various read/write/execute/kernel permission bits,
    it may also contain an "unmapped" flag
    indicating that this virtual page has no corresponding physical page.
    
    If the access cannot proceed for any reason, a fault is generated
    (a **page fault** if unmapped, otherwise a protection fault).
    The fault handler should either modify the page table
    so that the address will work on a second attempt
    or kill the program.

3. If the necessary permissions are present,
    the page table entry contains a physical page number.
    This physical page number, concatenated with the page offset
    from the virtual address, is the *physical address*.
    
Hardware uses various optimizations to speed up this process,
notably using the [Translation Lookaside Buffer].

### Protection... but side channels?

Address translation is designed so that the hardware guarantees that

- Only kernel-mode code can change the page table.
- All memory accesses are guarded by the page table.

Combined with careful coding of the operating system,
it is intended that these provide full separation of processes,
so that no two programs can see the contents of the other's memory
unless they have both asked the operating system to enable that.

Since 2017, it has become known that these protections
are not quite a complete as they were once thought.
Although these protections do ensure that one process cannot directly access another's memory,
they does not guarantee the absence of side-effects of memory access
that another process can detect.
For example, many hardware accelerations are based on the principle
that what happens once is likely to happen again,
so timing how long a memory access takes
can leak some information about what other memory accesses other processes may have made.

This is one example of how a side channel might exist,
and why it is challenging to reason about their absence.
Securing primary channels is easy to think about, even if not always easy to do,
because the data in question is known.
That the timing of individual memory accesses was information that needed protection
was not even known for decades after it became a potential side channel.

## Page Tables

A page table is conceptually just a map between virtual page numbers
(which are fixed-size integers)
and page table entries (which are each a physical page number and a set of Boolean flags).

### Single-level page tables

In the early days of virtual memory,
when the entire addressable space was not large,
a single array could suffice for implementing an efficient page table.
A special kernel-access-only register,
called the **page table base register** or PTBR,
stores the address of this array.

{.example ...}
Consider a system with 16-bit virtual addresses.
If that memory was broken into 2^7^ pages of 2^9^ bytes each,
and if each page table entry consists of 6 bits of flag bits
and a 10-bit physical page number,
then the single-level page table
is an array or 2^7^ 2-byte values (i.e., 2^8^ bytes in total).

If the virtual address is 1010 1111 0000 0101~2~
then the oage offset is 1 0000 0101~2~
and the virtual page number is 101 0111~2~.
Thus, the hardware accesses page table entry 101 0111~2~---that is,
physical address PTBR + 1010 1110~2~---to find
a page table entry.
If that page table entry's flags allow access
and it has the physical page number
11 0011 0011~2~,
then the resulting physical address is
the physical page number (11 0011 0011~2~)
concatenated with the page offset (1 0000 0101~2~)
for a physical address of 110 0110 0111 0000 0101~2~.

Note that this example maps 16-bit virtual addresses
to 19-bit physical addresses:
each *process* is limited to 64KB of addressable memory
but the hardware can support 512KB physical RAM.
{/}

Single-level page tables are inefficient for 32-bit addresses
and basically untenable for 64-bit addresses.

### Multi-level page tables

Multi-level page tables are a particular instance
of a linked data structure known as a high-arity (or wide node)
fixed-depth tree,
which is an efficient way for storing large, sparse arrays
particularly when entries are clustered.
Since this is not a data structure that is generally taught in data structure courses,
this section provides a full overview of the data structure itself
before discussing how they are used for virutal memory.

#### Fixed-depth high-arity trees

A tree is a linked data structure
where each node contains an array of pointers to additional nodes.
The best-known tree types
use arity-2 (also known as binary or width-2) trees,
where the arrays are all 2 elements long,
but trees with higher-arity are common when hardware is involved.

Many trees have variable depth:
the number of steps from the root to the node containing the desired data
may be different for different data.
That design allows the tree to scale in size,
but also mean that conditional checks are needed at each node.
Alternatively, a tree can be designed with a fixed depth,
with all data stored the same number of steps from the root.
Fixed-depth trees typically store all data in the leaves of the tree,
with separate data design for the internal nodes
and the leaf nodes.

{.example ...} The following code defines a tree
with arity 16 and three levels of intermediate nodes.
It has a fixed height of (depending on how you count) 3 or 4.

```c
#define WIDTH 16

struct leaf {
    PAYLOAD data[WIDTH];
};
struct height1 {
    struct leaf *kids[WIDTH];
};
struct height2 {
    struct height1 *kids[WIDTH];
};
struct height3 {
    struct height2 *kids[WIDTH];
};

/* ... */

struct height3 *root = /* ... */
```

This tree has the ability to store 16^4^ = 2^16^ = 65,536 `PAYLOAD` values
using a total of 16^3^ + 1 = 2^12^ + 1 = 4,097 pointers,
including the `root` pointer.
{/}

Fixed-depth fixed-width trees can be accessed using bit masks
to treat like an array.
If the width of each node is a power of two,
this is done by splitting up the bits of the index,
using the high-order bits for the first index
and lower-order bits as the tree is navigated.

{.example ...} In the following figure,
which uses width 8 instead of 16 to avoid over-cluttering the figure,
the value 23 has index 100 111 011 100~2~.

<svg version="1.1" viewBox="0 0 203.2 137.16" xmlns="http://www.w3.org/2000/svg" style="max-width:24em; margin:auto; display:table;">
<defs>
<marker id="DotM" overflow="visible" orient="auto">
<path transform="matrix(.4 0 0 .4 2.96 .4)" d="m-2.5-1c0 2.76-2.24 5-5 5s-5-2.24-5-5 2.24-5 5-5 5 2.24 5 5z" fill-rule="evenodd" stroke="#000" stroke-width="1pt"/>
</marker>
<marker id="Arrow2Mend" overflow="visible" orient="auto">
<path transform="scale(-.6)" d="m8.71859 4.03374-10.9259-4.01772 10.9259-4.01772c-1.7455 2.37206-1.73544 5.61745-6e-7 8.03544z" fill-rule="evenodd" stroke="#000" stroke-linejoin="round" stroke-width=".625"/>
</marker>
</defs>
<g transform="translate(0,-159.84)">
<g fill="none" stroke="#000">
<g stroke-width=".264583px">
<path d="m10.16 185.24v15.24h20.32v-15.24h20.32v15.24h20.32v-15.24h20.32v15.24h20.32v-15.24h20.32v15.24h20.32v-15.24h20.32v15.24h-162.56v-15.24h162.56"/>
<path d="m25.4 215.72v15.24h20.32v-15.24h20.32v15.24h20.32v-15.24h20.32v15.24h20.32v-15.24h20.32v15.24h20.32v-15.24h20.32v15.24h-162.56v-15.24h162.56"/>
<path d="m20.32 246.2v15.24h20.32v-15.24h20.32v15.24h20.32v-15.24h20.32v15.24h20.32v-15.24h20.32v15.24h20.32v-15.24h20.32v15.24h-162.56v-15.24h162.56"/>
<path d="m35.56 276.68v15.24h20.32v-15.24h20.32v15.24h20.32v-15.24h20.32v15.24h20.32v-15.24h20.32v15.24h20.32v-15.24h20.32v15.24h-162.56v-15.24h162.56"/>
</g>
<g stroke-width=".5">
<path d="m10.16 175.08-1e-5 10.16" marker-end="url(#Arrow2Mend)" marker-start="url(#DotM)"/>
<path d="m35.56 225.88c0 20.32-15.24 0-15.24 20.32" marker-end="url(#Arrow2Mend)" marker-start="url(#DotM)"/>
<path d="m132.08 256.36c0 20.32-96.52 0-96.52 20.32" marker-end="url(#Arrow2Mend)" marker-start="url(#DotM)"/>
</g>
</g>
<g font-size="8px" text-anchor="middle" word-spacing="0px">
<text x="106.67999" y="286.83997" text-align="center"><tspan x="106.67999" y="286.83997">23</tspan></text>
<text x="81.279991" y="185.23994" text-align="center"><tspan x="81.279991" y="185.23994">100</tspan></text>
<text x="35.559986" y="215.71996" text-align="center"><tspan x="35.559986" y="215.71996">111</tspan></text>
<text x="132.07999" y="246.19995" text-align="center"><tspan x="132.07999" y="246.19995">011</tspan></text>
<text x="106.67999" y="276.67993" text-align="center"><tspan x="106.67999" y="276.67993">100</tspan></text>
<text x="10.474821" y="169.95105" text-align="center"><tspan x="10.474821" y="169.95105">root</tspan></text>
</g>
<path d="m81.28 195.4c0 20.32-55.88 0-55.88 20.32" fill="none" marker-end="url(#Arrow2Mend)" marker-start="url(#DotM)" stroke="#000" stroke-width=".5"/>
</g>
</svg>
{/}

{.example ...} Continuing the previous code example,
each tree can be treated as if it were an array of 65,536 entries:

```c
PAYLOAD arr(struct height3 *root, unsigned short index) {
    struct height2 *child1 = root  [(index>>12)&(WIDTH-1)];
    struct height1 *child2 = child1[(index>> 8)&(WIDTH-1)];
    struct leaf    *child3 = child2[(index>> 4)&(WIDTH-1)];
    return child3[index&(WIDTH-1)];
}
```
{/}

Such a structure is significantly less efficient than a simple array:
it requires more memory than a simple array
because of the additional pointers;
it requires more time because it requires multiple memory accesses per element access.
However, it can save significant space if the majority of the indexes have no value, as entire subtrees can be omitted.

{.example ...} By adding intermediate null checks,
most of the tree can be omitted if only a few indices are needed:

```c
PAYLOAD *arr(struct height3 *root, unsigned short index) {
    if (!root) return NULL;
    struct height2 *child1 = root  [(index>>12)&(WIDTH-1)];
    if (!child1) return NULL;
    struct height1 *child2 = child1[(index>> 8)&(WIDTH-1)];
    if (!child2) return NULL;
    struct leaf    *child3 = child2[(index>> 4)&(WIDTH-1)];
    if (!child3) return NULL;
    return &child3[index&(WIDTH-1)];
}
```

While this code is even less efficient than the previous code in runtime,
it is significantly more space efficient if only a few values are in the "array".
For example, if only indices 0--10 (0x0000--0x000A) and 60,000--60,050 (0xEA60--0zEA92) are used,
the only non-NULL pointers are to:

- In the root, 0x0 and 0xE
    - in child1 0x0: 0x0
        - in child2 0x0: 0x0,
            - in child3 0x0: 0x0 through 0xA
    - in child1 0xE: 0xA
        - in child2 0xA: 0x6 through 0x9
            - in child3 0x6: 0x0 through 0xF
            - in child3 0x7: 0x0 through 0xF
            - in child3 0x8: 0x0 through 0xF
            - in child3 0x9: 0x0 through 0x2

Thus, only ten intermediate nodes and 5 × 16 = 80 `PAYLAOD` values are allocated
rather than the 65,535 `PAYLOAD` values that would have been allocated for a simple array.
{/}

#### Multi-level page table implementation

Most programs access only a few megabytes of memory,
allocated in a few contiguous chunks,
out of an address space of 2^64^ = 18,446,744,073,709,551,616 possible addresses.
This is, in many ways, and ideal scenario for using fixed-depth high-arity trees
to store a very sparse array of page table entries.

x86-64-compatible processors handle 64-bit addresses as follows:

- They completely ignore the top 16 bits of each virtual address.
    2^48^ = 281,474,976,710,656 is seen as more than a large enough address space for the current generation of software applications.

- They use 64-bit page table entries, with larger physical page numbers than virtual page numbers.
    This does not mean that all physical page numbers are used:
    it is the OS's job, when creating new page table entries,
    to ensure that the physical page number picked
    corresponds to an actual address in available RAM.

- They pick a page size
    such that every page table node exactly fits in one page.
    
    The most common such page size is 4KB pages
    with 2^9^ PTE per node (9+9+9+9+12 = 48).
    Other page sizes also work; for example 256K pages
    with 2^15^ PTE per node (15+15+18 = 48) fits perfectly,
    and at a slight loss of space efficiency other sizes can be used as well.
    
{.aside ...} Page size selection is a trade-off.
The smaller the pages, the fewer bytes of memory are wasted because of partially-used pages,
but the more time and space is devoted to the page table.

At one extreme, 16-byte pages (2 PTE per node)
means almost no unused allocated memory
but also means 44 intermediate page table nodes separate the PTBR and the PTE.

At the other extreme, 2^26^-byte pages (a single-level page table
filling half a page with 2^22^ PTE)
means that the smallest possible program needs at least two pages
(128MB of memory) to run: one for the page table and one for the code,
and a more realistic minimal program
with at least 1 page of code, 1 page of read-only globals,
1 page of read.write globals, 1 page of heap, 1 page of stack,
1 page of shared library functions, and 1 page of kernel memory,
plus the page table itself,
requires 512MB.

4KB pages are generally seen as a nice intermediate point
between tiny and huge pages, though their correctness
depends in large part on the application domain.

The trade-off between big and small pages is more significant
the larger the address space becomes.
Trying to mitigate this trade-off is part of the reason why
x86-64 only actually uses 48 bits of its 64-bit addresses.

x86-64 also allows mixed-size-pages,
where most pages are 4KB but a few are 2MB.
These "big pages" are only used for data, not page table nodes,
meaning some paths down the page table tree pass through one fewer node
(9+9+9+21 = 48).
This adds some complexity to the page table traversal process,
but allows the operating system more control over how pages are distributed.
{/}

- Each address translation that the [TLB](#translation-lookaside-buffer) does not optimize runs a process similar to the following:

    ````c
    size_t vpn[4] = split_bits(va>>12, 9);
    size_t ppn = PTBR;
    for(int i=0; i<4; i+=1) {
        PTE pte = ram[(ppn<<12) | (vpn[i]<<3)];
        if (pte.unmapped) page fault;
        if (pte.flags & current_usage) protection fault;
        ppn = pte.ppn;
    }
    pa = (ppn<<12) | (va & ((1<<12)-1));
    ````
    
All of the above is performed by hardware, having been compiled down to
actual physical transistors. It is not programmable.

The programmable parts are the fault handlers.
A page fault handler (written in the OS software)
might do something like

````c
handle_page_fault(size_t va, int access_type) {
    int flags = permitted_actions(va, segment_list)
    if ((access_type & flags) != access_type))
        send_signal(SIGSEGV);
    ssize_t ppn = get_unused_physical_page();
    if (ppn < 0) ppn = swap_page();
    create_page_table_entry(va, ppn, flags);
}
````

The above pseudocode uses several function stubs:

`permitted_actions`
:   Check the OS list of [segments](#segments-revisited)
    to see what kinds of access (reading, writing, executing, etc) 
    the segment that contains this address allows.
    If this address is not in a segment, returns that no actions are allowed.

`get_unused_physical_page`
:   The OS maintains a list of all of the physical pages in RAM
    and which process is using each.
    The idea here is to pull one page from the "unused" list
    onto the "used by current process" list.

`swap_page`
:   In the event that `get_unused_physical_page` fails to find an unused page,
    the OS pick a page currently being used by some process
    and "swaps it out":
    that is,
    
    1. Picks a physical page
    2. Writes its contents onto a region of disk known as the "swap space"
    3. Changes the PTE for that page such that
        a. the unmapped bit is set
        b. the rest of the bits tell where on disk the contents are located
    4. Change that page to be owned by the current process
    5. Return the newly-freed physical page number
    
    See [Page Swapping] for more on swapping.

`create_page_table_entry`
:   Ensures that the page table entry is in the page table
    (possibly creating intermediate nodes in the tree)
    and sets the appropriate permissions and physical page number
    in the PTE.
    If the PTE already existed and noted where in the swap space the old
    contents of this page were located, loads the swapped-out contents back from disk into memory too.

An important thing to note about the above:
software (the operating system) writes the page table
in a format that the hardware understands and can read.

### Translation Lookaside Buffer

Address translation needs to happen for every instruction,
at least once to fetch the instruction
and a second time if the instruction contains a memory access,
so making it fast is vital to keeping the computer efficient.

Modern processors use a special cache
called the **translation lookaside buffer** (or TBL)
to keep the most-used address translations in high-speed memory.

Conceptually, the TLB is used as follows:

````c
size_t vpn_full = va>>12;
if (tlb_has(vpn_full)) {
    pte = tlb_lookup(vpn_full);
    if (pte.flags & current_usage) protection fault;
    ppn = pte.ppn;
} else {
    size_t vpn[4] = split_bits(vpn_full, 9);
    size_t ppn = PTBR;
    for(int i=0; i<4; i+=1) {
        pte = ram[(ppn<<12) | (vpn[i]<<3)];
        if (pte.unmapped) page fault;
        if (pte.flags & current_usage) protection fault;
        ppn = pte.ppn;
    }
    tlb_store(vpn_full, pte);
}
pa = (ppn<<12) | (va & ((1<<12)-1));
````

For more details on how caches like the TLB work,
see [Caching](caching.html).

## Usage

Various uses of virtual memory are alluded to above
to motivate the various features of virtual memory;
this section focuses on those uses.

### Page Swapping

Because addresses in code can be mapped to other addresses,
with operating system intervention if needed,
virtual memory lets code act beyond the constraints of available memory.
This is traditionally handled by the operating system
storing some pages on the disk instead of in memory.
moving these pages between disk and memory is called "swapping".

Because disks are *vastly* slower than memory,
there is a desire to make this happen rarely,
meaning that operating systems sometimes devote significant effort
to selecting pages to swap out of memory
that it believes are likely not to be swapped back in again anytime soon.
Predicting which pages will be needed next is undecidable[^theory],
so this is always at best a heuristic and sometimes incorrect.
If it is consistently incorrect (or there is not enough physical memory available for accuracy to matter)
and the process begins spending more time
swapping pages in and out than it does doing actual work,
it is said that the process is **thrashing**.

[^theory]: In case you have not yet had the pleasure
    of studying computational theory---or
    (even more sad to contemplate!) have forgotten
    important bits of it---"undecidable" basically means
    "only solvable in special cases, not in general."
    
    In the happier case where you do remember computational theory
    and were hoping to find a proof here,
    Rice's Theorem is sufficient to demonstrate the undecidability
    of determining what pages will be needed in what order.

As memory has become less expensive and more plentiful,
swap is arguably[^arguably] less important than it once was,
but it is still a part of every major end-user OS.

[^arguably]: Arguably as in, I have heard friendly arguments
    between people who know about this than I on this topic.

### Shared Memory

One of the goals of virtual memory is to allow each process to have its own address space,
but because this is done by having a mapping between virtual and physical pages,
it is quite possible to map the same physical page
into multiple different processes' virtual address space---potentially
even to have the same physical page mapped to different virtual pages
in each process.

Shared memory, as this is called, is used for several purposes,
notably including the following.

Shared libraries.
:   If several programs are going to load in the same shared library code,
    it can be more efficient to have its code in just one physical page
    shared in each process's virtual address space.
    
    Not all libraries are linked in as shared memory.
    because shared memory must come in in full page chunks,
    shared memory library linkage can be inefficient if library code is small
    or not shared by several running processes.
    Additionally, sharing library code requires all processes that use it
    to use the exact same version of the code;
    as library code is frequently updated,
    this can itself be a challenge to engineer around.
    
    Static library code is linked during compilation,
    while shared (also called "dynamic") library code is linked by the loader
    or in some cases even part-way through execution.

Kernel memory.
:   Consider the page of memory that contains the instruction
    that changes the PTBR.
    For the next instruction to be fetched properly,
    that instruction's virtual page needs to map to the same physical page
    in both the old and new page table.
    
    Different operating systems handle this differently;
    in Linux, for example, half of the entire address space
    (all with addresses 0x800000000000 or above)
    is reserved for the OS,
    and is mapped to the same set of physical pages
    in every process's page table.

Shared-memory communication.
:   Processes can communicate with one another
    by each reading what others have written into a shared region of memory.
    This can be a very efficient way of sharing large quantities of data
    but does not, by itself, provide mechanisms for efficiently
    awaiting a response.

### Process Switching

Switching between processes happens in the operating system's code.
When the OS was invoked (in an [Exception Handler](#handling))
the state of the original process was stored somewhere;
when changing processes 

- the saved user state is moved into an OS data structure to track the not-currently-running processes.
- the PTBR is changed to the page table of the new process and the user page table entries in the translation lookaside buffer are marked as invalid.
- the saved state from the OS data structure is moved back into the saved user state location to be restored when the exception handler returns.

As this happens in operating system software,
other computation is often attached to it.
It is not uncommon for the operating system
to perform many different bookkeeping and other operations
each time a context switch is performed.

### Direct Memory Access

Physical memory pages can be given to systems other than software processes.
It is common for various input/output operations
to be performed by allocating a page (or more) of memory
to be accessed directly by the I/O systems.
This direct memory access model allows I/O to happen at the slower pace of I/O systems
concurrently with the processor doing other work,
and then once the transfer into or out of memory is completed
the processor to access it at the higher speed of memory access.
This model is called **direct memory access** (DMA).

Technically, DMA does not require virtual memory---disks
could be given access to physical memory
even if code access memory with physical addresses---but
it is generally implemented as an extension of the virtual memory system.
