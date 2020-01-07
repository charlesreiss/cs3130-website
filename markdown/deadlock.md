---
title: Deadlock
...

# Definition

A system is in *deadlock* if there exists a cycle of agents,
each currently holding a resource
and each waiting for another processor in the cycle to free a resource
before freeing the resources it holds.

A system is in *livelock* if the agents switch between one state and another without making progress.
For example, this could happen if they detect deadlock, back off, try again, and end up in the same deadlock state again.

# Necessary conditions

Deadlock can only occur if all four of the following occur:

1. Mutual Exclusion -- at least resource may exclude some agents from access because other agents are using it.

2. Hold-and-Wait -- at least one agent must be able to acquire one resource and retain ownership of it while waiting for another to become available.

3. No Preemption -- agents cannot forcibly take resources that other agents are currently holding.

4. Circular Wait -- it must be possible for agent A to be waiting for a resource that agent B won't release until after agent A releases a different resource.

Several comments about these:

- "Mutual Exclusion" is meant in a more general sense than the synchronization primitive of the same name.
- "Preemption" is meant in a more general sense that "preemptive concurrency"; preemptive concurrency refers only to preemption of processor resource, where the condition also refers to preemption of locks, etc.
- These are sometimes called "the Coffman conditions" after one author of the 1971 article that introduced them^[Coffman, Edward G., Jr.; Elphick, Michael J.; Shoshani, Arie (1971). "System Deadlocks" (PDF). *ACM Computing Surveys*. 3 (2): 67–78. [doi:10.1145/356586.356588](https://doi.org/10.1145%2F356586.356588).].
- These are necessary, not sufficient, conditions. They can all exist and no deadlock occur, but deadlock cannot occur unless they all exist.

# Detection

## Exact detection

Deadlock detection is usually defined in terms of a **resource allocation graph**, a directed graph with two node types:

- An *agent* node has an outgoing edge to each resource it is waiting for
- A *resource* node has an outgoing edge to each agent that is holding it, and a count of how many outgoing edges it can have.

If all resource counts are 1, there is deadlock if this graph contains a cycle.
If resource counts are higher, the detection is a bit more complicated
but approximately involves finding a subgraph where all resources have an outgoing degree equal to their counts and all agents have at least one outgoing edge.

Whatever process allocates resources (e.g., the OS for pthreads) can construct and check the current resource allocation graph each time a resource is allocated to check for deadlock.

## Heuristic detection

Various heuristics can be used to detect likely deadlock.
One simple heuristic is "if all threads are suspended waiting on a resource, there is likely deadlock."
Only *likely* deadlock, though, because there could have been timeouts, scheduled signals to change conditions, resources that are released by other processes, etc.

Heuristics are all that is available if no process has the information needed to construct a resource allocation graph.
Even when one can, heuristics are often simpler to implement and test.

# Recovery

Once deadlock is detected, there is often no attractive solution.

Picking some thread and removing its held resources can remove deadlock, but also removes the guarantees of synchronization primitives and can create race conditions.

Picking a thread and rewinding it to some state from before it obtained resources requires bookkeeping to make the threads so rewindable, and prevents doing irreversible actions like writing files.
Efforts to do this well often result in something similar to optimistic transactions -- irreversible work is staged in a temporary area and committed in bulk if everything went well.
They differ primarily in what "went well" means:
for optimistic transactions it means "nothing we depended on changed"
while for deadlock recovery it means "we did not encounter a deadlock".

Picking a thread and killing it completely is somewhat harsh, but can work;
it is similar to the "throw an exception" mentality that says
"we find the problem and stop working; it's up to whoever asked us to work to handle that".

# Prevention

In general, it is preferred to prevent the possibility of deadlock entirely.

## Static approaches

A *static* approach to deadlock prevention is to create code that cannot deadlock.
Your solutions to PA05 are examples of static deadlock prevention.
In generally it is assumed that if you write good concurrent code you have incorporated static deadlock prevention into your code.

Many modern languages provide specialized syntax-level constructs like Java's `synchronized` keyword to make some kinds of deadlock (such as return without unlocking) impossible.
These do not prevent all forms of deadlock, however.

Some libraries for concurrency intentionally design an API where either (a) no combination of API calls can create deadlock or (b) common use-cases cannot create deadlock.
A common family of such APIs are called "message-passing" approaches,
as opposed to "shared memory" approaches;
the "messages" are often implemented under the hood as lock-free concurrent queues,
stored in shared memory that the API does not expose to the programmer.

Message-passing is also convenient in that is scales up naturally to non-shared-memory systems such as supercomputers.

There have been some research papers (but so far as I know no production toolchains) that prove the absence of deadlock at compile time and give compiler errors if those proofs fail.

## Dynamic approaches

There are various ways to augment a threading library so that it prevents deadlock
without removing shared memory and similar features from user view.
All of these require additional information to be provided (accurately) by the programmer using the library.

The best known of these is the **Banker's Algorithm**.
It works as follows:

1. Before you can request any resources, you must state the maximum number of resources you'll ever need.
    That number cannot be increased while you are holding any resources.
1. If you try to request more resources than your stated maximum, you get an error, not the resource.
1. When you request a resource, the library first ensures that 
    - *if* they give you the resource, there are enough resources left that
    - *some* thread will be able to get their maximum number of resources 
    - and if that thread does and completes, *another* thread will be able to do the same,
    - and so on for all threads
    
    If these conditions are not met, the request blocks until they are.

This all assumes that each thread is able to complete work and release resources in a timely manner;
can be complicated to implement if the number of threads or resources are variable;
and can limit some kinds of algorithms where small maximums are not readily computable in advance.

I am unaware of a single use of the Banker's Algorithm or any related approach in modern software.
Static approaches are far more common, and when those cannot be relied on for OS code detect-kill-and-restart is more common.
