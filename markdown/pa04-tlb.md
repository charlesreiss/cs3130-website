---
title: PA04 -- Translation lookaside buffer
...

Implement a TLB wrapper around the page table interface from previous assignments.
Note you will *not* need to implement a page table for this assignment;
you'll merely use its API, as defined in `mlpt.h`

# Task

Implement a 4-way set-associative 16-set translation lookaside buffer with a single VPN → PA mapping per block^[You may hard-code 4 and 16 in your implementation, though a design that has these in `#define`s may actually help you keep your code organized.].
In addition to its block, each cache line should also store

- a valid bit
- necessary bookkeeping to implement LRU replacement within the set

You should design your own data structures, etc, to make this work properly.

The public API should be the following `tlb.h`

```c
#include "config.h" /* see PA02 for guidance on this file */
#include "mlpt.h"   /* see PA02 this file */

/** invalidate all cache lines in the TLB */
void tlb_clear();

/**
 * return 0 if this virtual address does not have a valid
 * mapping in the TLB. Otherwise, return its LRU status: 1
 * if it is the most-recently used, 2 if the next-to-most,
 * etc.
 */
int tlb_peek(size_t va);

/**
 * If this virtual address is in the TLB, return its
 * corresponding physical address. If not, use
 * `translate(va)` to find that address, store the result
 * in the TLB, and return it. In either case, make its
 * cache line the most-recently used in its set.
 *
 * As an exception, if translate(va) returns -1, do not
 * update the TLB: just return -1.
 */
size_t tlb_translate(size_t va);
```

Note that we do not have a `tlb_allocate`. This is by design: allocation is performed by software (a part of the operating system, accessed through a system call) and not by the hardware.

You should submit `tlb.h`, and other `.h` and `.c` files you create,
and a `Makefile` that depends on `mlpt.a` for its implementation of `translate` and produces, as its default target, `libtlb.a` with a definition of the three functions listed above.
And example target might look like

````makefile
libtlb.a: config.h $(objects)
	ar rcs libtlb.a $(objects)
````

# Separation of Concerns

Your TLB code **must not** depend on any implementation detail of your page tables.
It may use `translate`, plus the `#define`s in `config.h`,
and should append the page offset to the PPN it finds cached itself,
but should not invoke any code or use any variables defined in your implementations of PA02 or PA03 besides `translate`, `LEVELS`, and `POBITS`.
Each call to `translate` **must** be to a page-beginning address, and that only if it is not in the cache.

{.example ...}
If `POBITS` is 12 and `tlb_translate` is invoke with addresses `0x12345`, `0x12468`, and `0x13579` in that order,

- `tlb_translate(0x12345)` should invoke `translate(0x12000)`,
- `tlb_translate(0x12468)` should not invoke `translate` at all (it's a cache hit), and
- `tlb_translate(0x13579)` should invoke `translate(0x13000)`
{/}


# Tips

You can test this code without a working implementation of multi-level page tables,
possibly even more easily than you can with a working MLPT,
by creating a *stub*:
a special implementation of `translate` that returns specific values designed to let you test the `tlb_` functions. Even something as simple as

```c
/** stub for the purpose of testing tlb_* functions */
size_t translate(va) { return va; }
```

gives enough behavior to test most TLB behaviors; adding some logging code to the stub can help make sure that `translate` is only called as needed.

TLBs typically do not have a CPU-write-to-TLB functionality, so they do not need to be write-through or write-back.

# Examples

If you stub `translate(va)`{.c} as `return va < 0x1234000 ? va + 0x20000 : -1;`{.c}
then the following code should work

````c
tlb_clear();
assert(tlb_peek(0) == 0);
assert(tlb_translate(0) == 0x20000);
assert(tlb_peek(0) == 1);
assert(tlb_translate(0x200) == 0x20200);
assert(tlb_peek(0) == 1);
assert(tlb_peek(0x200) == 1);
assert(tlb_translate(0x1200) == 0x21200);
assert(tlb_translate(0x5200) == 0x25200);
assert(tlb_translate(0x8200) == 0x28200);
assert(tlb_translate(0x2200) == 0x22200);
assert(tlb_peek(0x1000) == 1);
assert(tlb_peek(0x5000) == 1);
assert(tlb_peek(0x8000) == 1);
assert(tlb_peek(0x2000) == 1);
assert(tlb_peek(0x0000) == 1);
assert(tlb_translate(0x101200) == 0x121200);
assert(tlb_translate(0x801200) == 0x821200);
assert(tlb_translate(0x301200) == 0x321200);
assert(tlb_translate(0x501200) == 0x521200);
assert(tlb_translate(0xA01200) == 0xA21200);
assert(tlb_translate(0xA0001200) == -1);
assert(tlb_peek(0x001200) == 0);
assert(tlb_peek(0x101200) == 0);
assert(tlb_peek(0x301200) == 3);
assert(tlb_peek(0x501200) == 2);
assert(tlb_peek(0x801200) == 4);
assert(tlb_peek(0xA01200) == 1);
assert(tlb_translate(0x301800) == 0x321800);
assert(tlb_peek(0x001000) == 0);
assert(tlb_peek(0x101000) == 0);
assert(tlb_peek(0x301000) == 1);
assert(tlb_peek(0x501000) == 3);
assert(tlb_peek(0x801000) == 4);
assert(tlb_peek(0xA01000) == 2);
````
