---
title: PA02 -- Page table
...

Implement a page table structure in C, with associated access functions.

# Specification

We supply two header files.

## `config.h`

`config.h` does nothing other than `#define`ing two constants.

```c
/** LEVELS = number of PTEs used to translate one address. */
#define LEVELS  4

/** POBITS = number of bits used for the page offset. */
#define POBITS  12
```

Your code must use these constants, must not redefine them,
and must continue to work even if we change their values.
For example, if we compile with a different `config.h` that `#define`s `LEVELS` as `3` instead of `4`,
your code should create a 3-level page table instead of a 4-level page table.
Your code should work for all integer values of `LEVELS` between `1` and `6` inclusive
and all integer values of `POBITS` between `4` and `18` inclusive.
As an exception, it does not need to (but may) support cases where 
(`POBITS` − 3) × (`LEVELS` + 1) > 60.

## `mlpt.h`

`mlpt.h` defines the public API your code will use.
You are welcome to edit it, provided such edits will not cause testing code to fail to compile.

```c
/**
 * Page table base register.
 * Declared here so tester code can look at it; because it is extern
 * you'll need to define it (without extern) in exactly one .c file.
 */
extern size_t ptbr;

/**
 * Given a virtual address, return the physical address.
 * Return a value consisting of all 1 bits
 * if this virtual address does not have a physical address.
 */
size_t translate(size_t va);

/**
 * Use posix_memalign to create page tables sufficient to have a mapping
 * between the given virtual address and some physical address. If there
 * already is such a page, does nothing.
 */
void page_allocate(size_t va);
```

## Behavior

Prior to the first invocation of `page_allocate`, `ptbr` should be `NULL`;
thereafter it should be a pointer to a heap-allocated array, cast as a `size_t`.
It should not change after the first `page_allocate` invocation.

If `ptbr` is not `NULL`, it should point to an array of `size_t`s
occupying 2^`POBITS`^ bytes.
Each such `size_t` should be interpreted as a page table entry.

{.note} Some texts refer to a PTBR containing a physical address; others to it containing a physical page number. The above definition asserts it contains a physical address, not page number. As such, it will always have 0 in its low-order `POBITS` bits.

Each page table entry should be either

0 in the low-order bit
:   There is no physical page.
    
    The remaining bits have no defined meaning; you may use them
    however you wish.

    {.example} PTE `0x1234567812345678` has a zero in the low-order bit, and thus indicates the absence of a physical page.

1 in the low-order bit
:   The bits above the `POBITS` low-order bits are the physical page number of either
    the next level page table or the physical page of memory.
    The `POBITS` − 1 low-order bits (excluding the 1 in the lowest-order bit)
    have no defined meaning; you may use them
    however you wish.

    {.example} PTE `0x1234567812345679` has a one in the low-order bit, and thus indicates the presence of a physical page or another level page table. If `POBITS` is 12, the physical page number (of the page or next page table level) is `0x1234567812345`.

No pages should be allocated unless requested by a call to `page_allocate`.

{.example ...} The comments in the following code correctly describe the number of pages that have ever been allocated
assuming that `LEVELS` is 4 and `POBITS` is 12.

```c

#include <stdio.h>
#include <assert.h>
#include "mlpt.h"
#include "config.h"

int main() {
    // 0 pages have been allocated
    assert(ptbr == 0);

    page_allocate(0x123456789abcdef);
    // 5 pages have been allocated: 4 page tables and 1 data
    assert(ptbr != 0);

    page_allocate(0x123456789abcd00);
    // no new pages allocated (still 5)
    
    int *p1 = (int *)translate(0x123456789abcd00);
    *p1 = 0xaabbccdd;
    short *p2 = (short *)translate(0x123456789abcd02);
    printf("%04hx\n", *p2); // prints "aabb\n"

    assert(translate(0x123456789ab0000) == 0xFFFFFFFFFFFFFFFF);
    
    page_allocate(0x123456789ab0000);
    // 1 new page allocated (now 6; 4 page table, 2 data)

    assert(translate(0x123456789ab0000) != 0xFFFFFFFFFFFFFFFF);
    
    page_allocate(0x123456780000000);
    // 2 new pages allocated (now 8; 5 page table, 3 data)
}
```
{/}

# Submission

You should submit code, with a Makefile, that

- Will work if I move the given `mlpt.h` and `config.h` into the directory with them, overwriting those you submit (if any)
- Meets all above specifications

Because we require no main function, you are welcome to have a target to build a program or not, at your preference.
We'll test by compiling adding our own tester program, though, so if you do have a `main`
make sure it is not in a file required to build your code (i.e., keep `main` in a different file than your implementations of `translate`, `page_allocate`, and `ptbr`).

# Tips

To test your code, you'll likely have to add extra code to let you measure things like the number of pages allocated.

A tip for good functional design: if you can describe what a non-trivial piece of code is doing, make it its own function with that action as its name. I'd encourage having names for all of the steps of address translation that we discussed in lectures.

Since we haven't used `posix_memalign`{.c} before and it's manual page is a bit confusing, the code I used to use it was

```c
void *ans;
posix_memalign(&ans, 1<<POBITS, 1<<POBITS);
```

You will likely find yourself casting between `size_t` and various pointer types multiple places in your code.
