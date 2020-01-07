---
title: Caches -- Keep a finger in it
...

# A thought experiment

Suppose I handed you a book and asked you to tell me the 211^th^ word on the 83^rd^ page,
then timed how long it took you to reply.
Let's call that time $t_1$.

Suppose I handed you another copy of the same book and asked you to tell me the 211^th^ word on the 83^rd^ page,
then timed how long it took you to reply.
Let's call that time $t_2$.

We might reasonably assume that $t_2$ would be dramatically smaller that $t_1$.
In fact, I'd expect $t_2$ to not even include the time needed to open the book.
$t_1$ is an estimate of the time needed to find a particular word in a book;
$t_2$ is an estimate of the time needed to recognize that the new book is the same as the old and repeat what you just said.

Now suppose that in-between the first and second request for the 211^th^ word on the 83^rd^ page of this book
I gave you a hundred other books, pages, and words to tell me.
There's still some chance that you'd remember the first word
when the same book, page, and word appeared a second time,
but more likely you'd need to look it up again,
though perhaps once you got to the page you'd remember where on the page the word was located.

Caching is a computer hardware (and sometimes also software) technique
for mimicking this faster-because-we-just-did-this-and-still-remember optimization.
Simple single-level caching either remembers a result or does not,
but multi-level caching can also have that partially-remembered characteristic
that lets a forgotten detail be recovered with only partial work.

Note that in this example, it is important to store more than just the word that is read;
you also have to remember the address (book, page, and word) at which it was found.
In caching terminology, that "where we found it" data
is known as the **tag**;
the thing being remembered is the **block**;
and the tag + block pair is known as a **cache line**.

# Locality

Locality is a pseudo-technical term in computing
with two subtypes:

Spatial locality
:   Refers to two related ideas:
    
    a. having only a small difference in numerical address
    b. programs where most addresses accessed are spatially local to the address accessed before them.

Temporal locality
:   Refers to the accesses to a single address
    being clustered in the same portion of the overall runtime of the program.

Most programs, even if not specifically designed to do so,
exhibit both spatial and temporal locality.
Caches are designed to increase memory access speeds
when either form of locality is present.

## Cache line

When caching memory, it is typical to remember not just the exact memory accesses,
but also the memory nearby, on the assumption (based on spatial locality)
that if one address is accessed nearby addresses will soon be accessed as well.

The most common way to do this is that when address `x` is accessed,
all addresses with the same high-order bits are loaded into the cache;
the data thus fetched is called a *block*.
The *tag* of a block is not the full address, just the higher-order bits that all bytes in the block share.

{.example ...}
Suppose a cache is designed with 64-byte blocks.
When accessing address `0x12345678`,
all bytes with addresses between `0x12345640`
and `0x1234567F`
(i.e., between `0x12345678 & ~0x3F`{.c} and `0x12345678 | 0x3F`{.c})
are loaded into the block
and the tag is `0x48d159` (i.e., `0x12345678 >> 6`{.c})
{/}

The low-order bits of the original address indicate where
the addressed information is stored within the block
and is known as the **block offset** or simply the offset.

{.aside ...} Comparison to Virtual Memory

There are several parallels between a cache line and a virtual memory page.
The common use of low-order bits as an offset into a contiguous chunk of memory is one such parallel;
we'll see later that most caches also break the high-order bits into something used to locate a line and something not so used,
like virtual memory's virtual page numbers and unused bits.

Virtual memory uses these tools to *translate* an address;
if there is no translation, there is no data at that address and a fault occurs.
Caching uses it to *accelerate* an access;
if the cache doesn't find it, RAM will, and there is no true failure possible.

Virtual memory provides new capability to a system via the cooperation of the hardware and OS.
Caching is managed entirely by the hardware and is only designed to increase speed, not add features.
{/}

# Three types of caches

A cache is a finite-size memory that stores a subset of recent results
of whatever it is caching.
If "cache" is used without other context,
it is generally safe to assume that what is being cached is memory accesses,
but most of the concepts of caching are independent of that detail.

There are several designs of a cache,
varying based on how they decide which old cache line is forgotten
to make room for a new cache line.

## Fully-associative

A **fully-associative cache** stores a set of cache lines.
Using an unordered set instead of an array gives maximal ability to pick and choose
which line is removed to make room for the next line;
but that freedom comes at a cost.

First, on each cache access,
a fully-associative cache needs to check the accessed address
against the tag of every single cache line in the cache,
as the line could be stored anywhere within the cache.
Secondly, it is believed that the best rule about which line to replace
is the **least-recently used** (LRU) rule:
that is, replace the cache line that has gone the longest since it was last accessed.
Computing that requires some additional bookkeeping,
needing both more storage and more computation.

### Capacity miss

A **cache miss** is when a cache is accessed but the line desired is not located in it.
Some misses are inevitable, not matter the cache design,
such as the **cold miss** when the address is accessed for the very first time.
Other misses are caused by the specific design of the cache.

A fully-associative cache can suffer from a **capacity miss**.
A capacity miss occurs when every line in the cache has been replaced
since the last access to the line being requested.

## Direct-mapped

A **direct-mapped cache** stores an array of cache lines.
As it is an array, an index is needed to know which line is being accessed;
this index is taken from a part of the address.

To maximize the benefits accruing from spatial locality,
the index is taken from the bits of the address
immediately above the offset,
leaving the bits above that as the tag;
this is illustrated in [Fig 1](#fig1).

<figure>
<svg style="max-width:22em; display:table; margin:auto;" version="1.1" viewBox="0 0 172.72 25.4" xmlns="http://www.w3.org/2000/svg">
<g transform="translate(0 -271.6)">
<rect x="5.08" y="276.68" width="162.56" height="15.24" fill="none" stroke="#000" stroke-width=".5"/>
<rect x="96.52" y="276.68" width="40.64" height="15.24" fill="none" stroke="#000" stroke-width=".5"/>
<g font-size="8px" text-anchor="middle">
<text x="116.84" y="286.84003" text-align="center"><tspan x="116.84" y="286.84003" stroke-width=".264583">index</tspan></text>
<text x="152.39999" y="286.84003" text-align="center"><tspan x="152.39999" y="286.84003" stroke-width=".264583">offset</tspan></text>
<text x="50.799999" y="286.84003" text-align="center"><tspan x="50.799999" y="286.84003" stroke-width=".264583">tag</tspan></text>
</g>
</g>
</svg>
<figcaption id="fig1"><strong>Fig 1</strong>: Breakdown of direct-mapped address.
The $\log_2($bytes per block$)$ low-order bits are the block offset;
the $\log_2($lines in cache$)$ next lowest are the index;
and the remaining higher-order bits are the tag.</figcaption>
</figure>

Direct mapped caches are very efficient, and can be made far larger than fully-associative caches can.
However, they are far less flexible and often have a higher miss-rate per unit capacity.

### Conflict miss

A direct-mapped cache can suffer from a **conflict miss**
(and the cold miss that any cache can experience).
A conflict miss occurs when a different line has been loaded into the same index
since a requested line was last accessed.
While conflict misses are more likely when a cache is small than when it is large,
in a direct mapped cache of any size a conflict miss can occur with as few as three memory accesses.

{.example ...}
Suppose a direct-mapped cache has 16-byte blocks
and 256 lines.
Then the addresses `0xbf1234`, `0xbf1238` and `0xbf9230`
all have the same index (`0x23`).
The first two also have the same tag (`0xbf1`)
and hence are not in conflict with one another;
the third has a different tag (`0xbf9`).

If the following memory addresses are accessed in order,
the following notes if it is a hit or miss and if a miss, what type.

1. address `0xbf1234` -- cold miss
2. address `0xbf1238` -- hit
3. address `0xbf9230` -- cold miss
4. address `0xbf1234` -- conflict miss
{/}

## Set-associative

A set-associative cache
is a compromise
between the flexibility at cost of a fully-associative cache
and the scale and efficiency of a direct-mapped cache.
Structurally, it is an array of sets of lines.
Set associative caches dominate hardware,
and if a cache with more than a few dozen lines is mentioned without specifying type
it is generally safe to assume it is set-associative.

Addresses are processed by set-associative caches
exactly the same way they are for direct-mapped caches:
a tag, index, and block offset (see [Fig 1](#fig1)).
The index, however, instead of identifying a single line
identifies an entire set of lines.
As such, it is traditionally called a **set index** instead of simply an index.
The size of the sets are referred to using the term
"$n$-way set-associative cache" meaning each set contains $n$ lines.

Neither capacity nor conflict misses are a perfect match for a set-associative cache,
though the terms are sometimes used loosely to distinguish between
misses that could have been avoided with a different cache design (conflict)
and misses that could only have been avoided by having a larger cache (capacity).

# Common memory caches

As of 2019, it is common for end-user computers to have a variety of caches.

L1
:   The L1 cache is the smallest and fastest cache.
    It is virtually always placed directly on the processor chip itself
    and sized so that an access to L1 takes only a single cycle^[At least in a [pipelined](processors.html) sense. The goal is that an L1 hit does not slow down processing at all.].
    It is often designed to work on virtual addresses rather than physical,
    or to have the set index be entirely within the page offset so that TLB lookup
    can be performed concurrently with L1 lookup.

TLB
:   The TLB tends to be a small set-associative cache with fairly large sets
    that holds a single [page table entry](kernel.html#page-tables) per block.
    TLBs only hold the final PTE encountered in translating an address,
    the one that has the PPN of the final memory page.
    
    The TLB is often a read-only cache.
    If the page table is modified, the TLB may need to be partly or fully **flushed**:
    that is, (some of) its entries are marked as invalid,
    requiring full page table lookups upon next access.

L2--L*n*
:   The L2 cache is larger than the L1
    which means both that it is more likely that any given access will hit in the L2
    and that each access takes a bit longer.
    The L3 is bigger (and thus slower) than the L2, and so on.
    The number of caches in this cache hierarchy increases every few years
    as processors continue to get faster than memory
    and more intermediate levels make sense.

If a block of data is stored in the L$i$ cache, it is also stored in the L$i+1$ cache and so on back up to RAM.

Each cache also has a policy for how memory writes are sent to larger components of the cache hierarchy.
The two most common such policies are

Write back
:   An edit is placed only in the cache itself
    and not sent to the larger layer beyond it until the cache line is going to be evicted.
    Write back policies typically use a "dirty bit" in each cache line
    to mark which lines have been edited (are "dirty") and need to be written upon eviction
    and which were read without being modified.

Write through
:   Each edit is immediately sent to the next larger layer of the cache hierarchy.
    Typically this means there is some kind of finite-size write queue
    between the cache layers and that if it is full
    the write happens at the speed of the slower cache instead of the faster,
    but it also means that evicting a cache line does not itself slow down processing.

There is no need for all of the cache layers to use the same write policy;
a write-back L1 coupled with a write-though L2 is a perfectly doable configuration.

# Cache-efficient code

As guiding principles, code is more cache-efficient if

- It performs all accesses to one region of memory at the same time
- It uses memory efficiently
- Addresses are aligned so no single access spans cache lines
- When multiple different regions of memory are being accessed, they differ in set index not just tag

Writing cache efficient code is a fairly involved topic,
well beyond the scope of this course.
It often involves counter-intuitive decisions such as adding more loops to code
and using data structures with worse asymptotic behavior but better cache locality.
A common trend is these make code harder for humans to read, understand, and adjust,
so they should generally be used only when the speed increases they provide are actually needed.
If you end up  writing high-performance code at some point,
these will be techniques to learn.
