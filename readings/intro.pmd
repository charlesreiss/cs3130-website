---
title: COA2 overview
author: Luther Tychnoviech, with modifications by Charles Reiss
...

In CSO1 you learned a simple version of how processors work, and used operating system provided functions as "black boxes": i.e., functions that do what they do, we know not how. In CSO2, we'll explore a more realistic model of processors, and how they and the operating system work together to provide various functionality we depend on as application programmers.

Each of the topics this term is intricate enough to require focused exploration, which may make it easy to loose sight of how they are all interrelated. The remainder of this section is designed to be an overview of each of those topics and how they fit together.

# Interrupts: safe communication with hardware

What happens when you press a key on the keyboard?
Clearly some electrical signal is sent to some part of the computer,
but how does the computer detect that it happened?
While it is possible to set up a *polling*-based system, where the program pauses every few milliseconds and checks to see if some key is being pressed or mouse moved or network packed collected,
it is more common to use interrupts.

Interrupts allow a signal on a wire entering the processor
to change what code the processor is running, suspending your program
and running an interrupt handler in the operating system.
There are many benefits to this design:
it removes the need to frequently check of new input,
ensures that only trusted code handle I/O devices,
and provides separation between peripheral drivers and the user code that interacts with them.

Interrupt handling is a special case of a more general tool known as *exception handling*.
It's not just I/O devices that might cause the operating system to take over:
it is also invoked if code does something impossible like dividing by zero
or something that requires fixing like trying to access a memory address that has no translation
or even just runs too long without letting another process have a turn.
Hardware exceptions, and related concepts like signals and software exceptions, are key parts of modern computer organization.

# Virtual Memory: the illusion of isolation

When writing assembly, we have not had to worry about which regions of memory other processes are using, even though in practice it is common for computers to be running hundreds of different processes concurrently, each with code and data in memory. This is not just a convenience: we don't want other processes to be able to see the contents of our process's memory or else they could use that visibility to read passwords or other private data from us.

This address-space separation is provided via a set of tools collectively called *virtual memory*. Conceptually this is just an extra level of indirection; instead of an instruction like

    movq 20(%eax), %ebx

being implemented as what we might write in C as

    ebx = memory[20 + eax]

it is instead implemented as

    ebx = memory[translation[20 + eax]]

where `translation` is an *address translation* data structure that is changed when changing processes, and that can only be changed by your operating system, not by normal programs. To achieve address translation, we need

a. a data structure that hardware can read.
a. special software (the operating system or *kernel*) that can modify memory regular user code cannot.
a. some optimization to make this close to as efficient as direct memory access.

Virtual memory is only one of several functionalities that will require close cooperation between special privileged software (the kernel) and hardware in order to achieve desirable behaviors.

# Networking: connecting to other computers

It is decreasingly common to have a program that does not interact with other computers in some way.
Networking is a larger topic than we'll be able to fully explore,
but there are a few concepts that are common enough to deserve our attention.
In particular, DNS and TCP/IP are so pervasive that it is not uncommon for people to assume they are being used any time networked communication is undertaken.

# Caching: avoiding repetitive work

If I ask you "What is thirty-seven times one hundred three?"
you'll require a few seconds to reply.
If I then immediately ask the same question a second time,
the second reply will come much more rapidly.
This is because your brain has cached the answer: stored it in a small slot of short-term memory
so it can produce the result without recomputation.

Caches are used throughout computing because work tends to repeat.
One of their most iconic uses is with memory.
It can take hundreds of nanoseconds to access a multi-gigabyte DRAM chip,
but only a few nanoseconds to access a few-megabyte SRAM chip.
Since programs often access the same address repeatedly,
significant speed increases can be created
by loading a copy of memory from the slower chip to the faster
and serving subsequent accesses from that faster cache.

Memory caching matters to ordinary programmers in at least two ways.
First, code with good locality (i.e., uses memory in a way the cache expected it to)
can be as much as an order of magnitude faster than code with poor locality.
Second, caching is emerging as a side channel,
a way that program security can be compromised by looking for side effects of computation
that remain visible in the cache.

We'll also discuss a bit about software caching and memoizing as a performance tool,
though that is not a major topic of this course.

# Synchronization: concurrency requires coordination

Sometimes you need to do several things at once.
Consider the common task of browsing with several browser tabs open at once.
You, as a user of software, expect all of the browser tabs to be loading all of the content of each page concurrently,
and the computer to react with no perceptible delay to every keystroke and mouse motion,
and music and videos to keep playing...

Concurrency is pervasive in computing.
Sometimes, it is truly parallel: multiple processor cores doing different tasks at the same time.
Often, it is merely the illusion of parallelism: a single processor jumps back and forth between several tasks many times a second so that they all appear to make progress together.
Either way, achieving concurrency is relatively easy
compared to the complexity of getting multiple concurrent actions to interact properly.

As a simple thought experiment, imagine that a teacher wrote a `0` on the board
and asked all the students to add their age to the number on the board and write the sum back up on the board.
Getting all the students to add their age to the number is easy;
getting them all the ability to write it back on the board is trickier;
getting them all to add their age to the total age so far is harder still,
and can seem to require that each student act one at a time.
There are approaches to this task where some parallelism is still used,
but they are far more complicated than a simple description of the task suggests.

This course will not spend too much time on how to design concurrent algorithms,
but will spend some time on how to negotiate coordination between concurrent threads and processes.
What does it mean to wait for another agent?
What happens if two agents are both waiting for the other to act first?
How does all of this manifest in hardware and software?
These and related topics will occupy a significant part of our attention this semester.

# Processors: idle = wasteful

A processor has many specialized subsystems: adders and movers and multipliers and so on. Any given instruction typically only uses a subset of these, meaning most are idle. Idle transistors still take up space on the chip, still draw some power and generate some heat: they have cost.

Additionally, some of the steps in executing a single instruction depend on other parts concluding: to determine the next instruction address, for example, requires first determining what the current instruction is, and then adding an appropriate offset to the current instruction's address. You can't start the addition until after you know the size of the instruction. That means that the instruction identifying hardware is idle while the adding is taking place, and the adder is idle (or, more commonly, doing work it will soon throw away) while the instruction identifying is taking place. And this waiting not only has power and heat costs, they also slow down the overall runtime of each instruction.

Henry Ford popularized a partial solution to these problems: the assembly line. After the machinery and workers who created car frames finished the first frame, they could pass it off to the next stage in car construction and begin a second frame. This meant more moving things around and could mean that each car took slightly longer to create from start to finish than if all the workers and tools were focused on one car at a time, but since many cars were in different stages of production at any given point in time, the total number of cars produced per month was greatly increased by the assembly line approach.

Processors make use of an assembly-line like decomposition of the work involved in each instruction. *Pipelining*, as this is called, increases the *throughput* of instructions (i.e., the number of instructions completed per microsecond) but also increases the *latency* (i.e., the number of picoseconds needed for one instruction to be completed). Since we are generally interested in throughput, this is a net win.

Pipelining has several complications that assembly lines do not because the operation of subsequent instructions may depend on the outcomes of previous instructions. Sometimes these can be solved by *forwarding*, taking information from an incomplete previous instruction to facilitate executing a following instruction. But not all information exists to be forwarded, so sometimes processors will perform *out-of-order execution*, picking a future instruction with fewer dependencies to execute first. When it is unclear which instruction comes next (as, for example, following a conditional branch) the processors will sometimes perform *speculative execution*, picking one of the instruction sequences that might be next to execute before knowing if they ought to be run, discarding their results if it turns out that the wrong option was picked.

Since 2017, security exploits have been known that depend on information leakage from speculatively-executed instructions. At the time of writing^[November 2019] it is not yet clear to what degree these newly-discovered attack vectors will influence the design of future processors.

