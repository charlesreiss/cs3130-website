#ifndef MLPT_H_
#define MLPT_H_

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
 * Allocates and maps the virtual page which starts at virtual address `start_va`
 * (if it is not allocated ready).
 *
 * If `start_va` is not the address at the start of a page, returns `-1`.
 * If `start_va` is the address at the start of a page, but the
 * page is already allocated, returns `0`; otherwise, returns `1`.
 *
 * Any data pages and page tables not yet allocated will be
 * allocated using `posix_memalign`.
 * (Any data pages or page tables already created, such as by a prior
 * call to `allocate_page` will be reused. If the mapping is already entirely
 * setup, the function should do nothing.)
 */
int allocate_page(size_t start_va);
#endif
