---
title: Consistency
...

In practice, truly shared-memory concurrency (where there is only one piece of hardware that stores the bits of a particular memory location) is not very salable.
L1 caches routinely create separate copies for each core;
systems with multiple CPU chips may replicate across all cache layers;
supercomputers may have multiple copies of the entirety of RAM;
and distributed systems, where separate computers are communicating over a network,
virtually always replicate data at each computer.

Thus, we need to worry about the consistency of multiple copies.
Roughly speaking, by *consistent* we mean

> what one processor writes to an address, other processors see when they read that address

There are *many* more formal definitions of consistency^[See, e.g., the [wikipedia category](https://en.wikipedia.org/wiki/Category:Consistency_models) and [summary article](https://en.wikipedia.org/wiki/Consistency_model)].
This writeup mentions just a few of the most common.

# Strict consistency

> All accesses (read and write) are seen **instantly** by all processors.

This cannot be implemented
because information cannot travel faster than the speed of light^[At least it cannot without quantum entanglement. It is not yet clear if strict consistency will be achievable with quantum computing or not.].
But it's nice to have a name for the impossible ideal.

# Strong Consistency

> All accesses (read and write) are seen by all parallel processors **in the same sequential order**.

This is weaker than strict consistency in that it is only the *ordering*, not the *time*, or accesses that all processors agree upon.
I you programmed only with atomic operations, or with a memory barrier between ever memory access, you'd have strong consistency.

# Weak consistency

> All accesses (read and write) **to synchronization variables** are seen by all parallel processors in the same sequential order.

This is a relaxation of strong consistency to apply only to a few special memory locations.
It is what is provided by correct use of things like `pthread_mutex_lock(&m)`{.c} -- `m` is one such "synchronization variable".


# Sequential consistency

> The **result** of execution is the same as if each process was interleaved **in some order** on a single sequential processor.

This definition is similar to *strong* consistency, but is based on *result* not event.
Some people even use the terms interchangeably.

Sequential consistency, being about *the* result (instead of *each* result), requires [Eventual consistency](#eventual-consistency).

{.example ...}
Suppose process 1 performed operations `A; B; C;` in parallel with process 2 performing `x; y;`.
If the outcome is the same you'd see from one processor executing any one of the following:

- `A; B; C; x; y;`
- `A; B; x; C; y;`
- `A; B; x; y; C;`
- `A; x; B; C; y;`
- `A; x; B; y; C;`
- `A; x; y; B; C;`
- `x; A; B; C; y;`
- `x; A; B; y; C;`
- `x; A; y; B; C;`
- `x; y; A; B; C;`

then it is sequentially consistent. If there is any other outcome, it is not.
{/}

# Eventual consistency

> There **exists a time after** the last write to each address when all processors will receive the **same value** when reading from that address.

Note that this does not make any guarantees about *what* will be seen in that address, only that every process will agree.

{.example ...}
Suppose one processor executes `*x = 0x1234`{.c} and another executes `*x = 0x5678`{.c}
Eventual consistency can be satisfied even if

- there is a time when the two disagree about `*x`{.c}'s value
- they eventually converge on the value being `0x1278`{.c} or some other value neither of them tried to create.
{/}

Even providing eventual consistency requires some effort,
but it is achievable enough that many distributed systems guarantee it.
