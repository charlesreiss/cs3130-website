---
title: Cache lab
author: Charles Reiss
...


1.  For this lab, you will need to answer several questions about *set-associative* caches. Normally, we would have consistently covered this in lecture before the lab,
    but we had some disruptions last week, so you can find some description:

    *  in the "briefly about set associative caches" section below;
    *  in the [reading on caches](../readings/caches.html), including some examples that are similar to lab exercises
    *  the 26 Feb lecture 9am recording from 24:20 through around 55:00;

1.  Answer the questions at the [lab interface](https://kytos02.cs.virginia.edu/cs3130-spring2025/cachelab/cachelab.php).

    Each person should answer an individual set of questions, but you may work with others to figure how to do
    so.

    (If you have correctly completed the exercises on the lab interface, that counts
    as submitting the lab and you do not need to get checked off.)


# Briefly about set-associative caches

In a set-associative cache, unlike a direct mapped cache, each set ("row") of the cache contains several blocks. If the cache has K blocks per set, then we say
it is a K-"way" set-associative cache. You can see an example of this [starting from slide 22 (PDF page 67) of the caching slides](https://www.cs.virginia.edu/~cr4bd/3130/S2025/slides/caching.pdf#page=67): essentially, we stapled two direct-mapped caches together to get a 2-way set associative cache.

The process for looking up a value in a *K*-way set-associative cache is mostly the same as for a direct-mapped cache:

*  we split the address into a tag, index, and offset, like with a direct-mapped cache. The number of set index bits is determined by the number of sets, the number of offset bits by the size of blocks.
*  we look at the blocks in set with the corresponding index. Unlike, the direct-mapped cache, we will have *K* candidates to check.
*  if any of our candidate blocks are valid and have a matching tag, we have a cache hit
*  otherwise, we have a miss. We will read in the block that was accessed into the set, replacing one of the *K* blocks we checked. For this lab, we will always replace
   the block that was least recently accessed (or, if possible, never accessed).

For determining the tag, index, and offset bits of a set-associative cache, the procedures are basically the same as for a direct-mapped cache:

*  the number of index bits is equal to log base 2 of the number of sets (rows)
*  the number of offset bits is equal to log base 2 of the block size

Where calculations may be different than a direct-mapped cache is when you're using the overall cache size to compute things like the number of sets: with a direct-mapped cache, computing the number of
blocks gives you the number of sets (so, e.g. a 65536B direct-mapped cache with 64B blocks has 1024 blocks and therefore 1024 sets). With a *K*-way set associative cache, once you compute
the number of blocks, you need to divide by the number of ways (blocks per set) to get the numbers of sets (so, e.g., a 65536B 4-way cache with 64B has 1024 blcoks and therefore 1024/4=256 sets). This is reflected on the formulas on [slide 28 (PDF page 67) of the caching slides](https://www.cs.virginia.edu/~cr4bd/3130/S2025/slides/caching.pdf#page=67).

