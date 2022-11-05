---
title: Synchronization primitives
...

# Day-to-day life

When you make a comment in a conversation, it is (usually) treated as an [atomic operation](#atomic-operation): you can divide the conversation into what happened before and what happened after your comment, but during it nothing else occurred. This is not true in most text-based chats, though, where it is common for multiple comments to be created concurrently.

Study rooms, lavatory stalls, TV remotes, and many other resources are handled as if protected by a [mutex](#mutex): anyone can use them, but by using them you ar excluding others from using them at the same time. [Semaphores](#Semaphore) and [monitors](#monitor) have some special properties, but are roughly variations on the [mutex](#mutex) theme.

One person walking along a sidewalk does not prevent others from entering it as well, but one construction team repairing the sidewalk does because maintenance staff tend to use a [reader-writer lock](#reader-writer-lock) to access sidewalks and many other parts of our lived environment: they may either be used by a single repair team (a "writer") or any number of pedestrians ("readers") but not both at the same time.

If you've ever told a group a friends "we'll meet up by the statue and then go out from there" you've implemented a [barrier](#barrier): everyone arriving at the statue waits there until everyone arrives, and then all move forward together from there.

When you go to a store, a purchase only occurs if both you give them money and they give you the product. If either step fails, neither step occurs. This is an example of a [transaction](#transaction): multiple steps, between which other things can occur, but guaranteed to either all happen or none happen.

# Atomic Operation

An **atomic operation** is an action or set of actions are made to appear to happen all at once, with no ability for any other action to occur in the middle.
For non-parallel concurrency, this can be guaranteed by the OS refusing to respond to interrupts until the atomic operation is completed.
When there is parallelism, this generally requires extra support by the hardware.
Either way, atomic operations are generally slower than their non-atomic counterparts
and only a limited number of them are provided on any given system.

Atomic operations are used to implement all of the other synchronization tools on this page.

# Semaphore

A variable indicating how many (if a counting semaphore) or if any (if a boolean semaphore) resources are available. Because simple variables are not modified atomically, semaphores are typically implemented as an abstract datatype with three methods for modifying them:

Acquire or wait
:   Attempt to acquire a resource, decrementing the count or setting the boolean to false.
    If the count is 0, suspend operation until it becomes 1 again.
    
    This operation is sometimes called **P**, **wait**, **down**, **acquire**, **pend**, **procure**, or **decrement**.

Acquire if possible
:   Attempt to acquire a resource, decrementing the count or setting the boolean to false,
    and return a success code if successful.
    If the count was already 0, instead do nothing to the semaphore and return a failure code.
    
    This operation is present in many semaphore libraries, but rarely has its own name. Generally it is achieved by some kind of "don't block" flag given to the same function that performed "acquire or wait".

Release
:   Increment the count, possibly causing a waiting process to become active.

    This operation is sometimes called **V**, **signal**, **up**, **release**, **post**, **vacate**, or **increment**.

The naming of these operations is complicated by having first been defined by a dutch team (led by Edgar Dijkstra)
who gave different words for **P** and **V** to stand for in different reports;
for a brief summary of names, see <https://en.wikipedia.org/wiki/Semaphore_(programming)#Operation_names>.

Although I described counting semaphores as a count of how many resources are available, that can be adjusted; some systems use a negative count to indicate how many processes are waiting for a resource, for example. While these changes do not impact the overall functionality of a semaphore, they can impact initialization state and so on for a particular library, so be sure to read the manual for any library you use if the specific numbers are meaningful in your application.

# Mutex / Lock      {#mutex}

A **mutex** (from "mutual exclusion") or **lock** is any device that ensures one and only one thread or process has access to some resource at a time.

Mutexes can be implemented as a binary [semaphore](#semaphore), provided that an additional limitation is imposed where only the thread that has acquired and not yet released the semaphore releases it.
More commonly, mutexes are built as more complicated constructs to provide extra features, including enforcing the "only the acquirer can release" property, automatic release if the acquirer terminates without releasing, etc.

Operations on mutexes use similar vocabulary to the corresponding operations on [semaphores](#semaphore).

# Condition Variable

A **condition variable**, despite having "variable" in its name, is not a variable nor like one in any meaningful way.
Instead, it is composed of at least two fairly complicated constructs:

- An **assertion**^[Note that this is not the usual programming definition of "assertion"; in code, an assertion is a function that checks a Boolean value and terminates the program with an error message if it is false.]; that is, a condition that must be satisfied before operation continues.
    
    Some implementations no not actually store the assertion in any way,
    instead assuming that it is handled elsewhere in the code
    and that the associated [monitor](#monitor) will invoke *wait* only if it was false
    and *signal* any time it becomes true.

- A **wait-queue**; that is, a collection (not necessarily a queue) of suspended threads awaiting the moment when the assertion will become true.

- Optionally, a **ready queue**; that is, a collection (not necessarily a queue) of suspended threads where the assertion is true and the thread is ready to run, but has not yet been scheduled to do so.

Condition variables are almost always discussed coupled with a [mutex](#mutex), the pair forming a [monitor](#monitor).
Because condition variables are so often used in monitors, I have sometimes seen a monitor called a "condition variable" colloquially.


# Monitor

A **monitor** is a paired [mutex](#mutex) and [condition variable](#condition-variable) with the following operations defined upon the pair:

wait
:   Used to wait until the condition variable's assertion becomes true.

    Suspend operation of the current thread, leaving the mutex unlocked;
    place the thread into the condition variable's wait-queue.

signal or notify
:   Used to indicate that the condition variable's assertion became true, and that a waiting thread may thus execute.

    Pick one thread from the condition variable's wait-queue and resume it, having it lock the mutex before it resumes operation.

broadcast or notifyAll
:   Used to tell all sleeping threads to wake up.
    Often used if the assertion of the condition variable was implicitly defined
    and the broadcasting code is unsure which, if any, sleeping threads are now ready to operate.
    
    Since each waking thread is supposed to acquire the mutex associated with the monitor, monitors that have a broadcast function can benefit from a ready queue to store the set of threads that are ready to run but are waiting for the mutex.
    Depending on the implementation of the mutex, that ready queue may be implicitly handled by the mutex instead.

Monitors are sometimes implemented at a programming language level as the most user-friendly technique for achieving mutual exclusion. Java, for example, decided that every object would be a monitor and includes the `synchronized`{.java} keyword and associated control constructs for automatically waiting on and signaling that monitor.

# Reader-writer lock

A reader-writer lock is used in two modes.
If used by a writer, it is a [mutex](#mutex);
if used by a reader, it is not.
In particular, is has the following four operations

Begin read
:   If there are no writers, increment the number of readers.
    Otherwise, wait for the writer to finish.

End read
:   Decrement the number of readers.

Begin write
:   If there are neither readers nor writers, set the number of writers to 1.
    Otherwise, wait for the number of readers to become 0.

End write
:   Set the number of writers to 0.

There are implementations of this using multiple [mutexes](#mutex)
or a [monitor](#monitor), both augmented with a few extra variables.
Several designs exist in part because of the multiple possible answers to the following question:

- If a writer is waiting for all the readers to be done, and a new reader arrives, should that reader be allowed in or made to wait for the writer to finish first?

Both read-preferring and write-preferring reader-writer locks have pros and cons, and both are commonly implemented.

# Barrier

A **barrier** is somewhat similar to a counting semaphore,
in that it can be thought of as a simple integer.
However, it needs to be configured in advance with the number of processis it is to wait for
and once initialized it has just has one action:

arrive
:   Increment the counter.

    If that increment caused the counter to reach the preconfigured number of processes this barrier is to wait for,
    resume all processes suspended at this barrier and reset the counter to 0.
    Otherwise, suspend the arriving process.

In practice, somewhat more complicated structures are used
to help this design scale and to ensure
both that the increment-and-check happen atomically
and that all waiting processes pass the barrier before others arrive.

# Transaction

A **transaction**, or more formally an **atomic transaction**,
is a group of operations that are guaranteed to occur as if they were a single [atomic operation](#atomic-operation).
Appropriate use of [mutexes](#mutex) [monitors](#monitor) can be used to implement transactions,
but often when transactions are identified by name
they are instead implemented "optimistically" using something liek the read-copy-update mechanism.

The principle behind read-copy-update is that a process checks to make sure none of the inputs of a computed value have changed before placing that value into memory.

{.example ...} Suppose you wanted to compute `x = a*x*x + b*x + c` as a transaction. You would

1. make a copy of the current values of `a`, `b`, `c`, and `x` as `A`, `B`, `C`, and `X`.
2. compute the new value of `x` as `x_new = A*X*X + B*X + C`.
3. use a mutex to ensure you are the only one accessing memory while
    a. verifying that the value now in `a` is still `A`, and similarly for `b` and `B`, `c` and `C`, and `x` and `X`.
    b. if so, writing `x = x_new` and release the mutex.
    c. if not, release the mutex and return to step 1 to try again.
{/}

