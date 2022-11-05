---
title: Pipeline Diagrams
...

> For various reasons, the hoped-for C++11 lab never came to fruition. If you are interested, I recommend a slow and careful read of the [wikipedia article](https://en.wikipedia.org/wiki/C++11) on C++11, which explains many of the significant innovations of this release over previous C++ releases.
>
> This replacement lab is likely to be a bit less content-laden than some before it.
>
> — Luther Tychonievich

Due to scheduling constraints, we are running low on time to offer PA07. However, because the process of doing that is a beneficial learning exercise, and because I expect it will boost many of your GPAs to have it included, this lab will have you work with a partner on that PA.

# Pipeline diagrams

When considering pipelined execution, there are two related but not identical notions of "time":
instruction-order time and clock-cycle time.
A pipeline diagram is a graph of both kinds of time,
with instruction order on the vertical axis (future = down)
and clock cycle on the horizontal (future = right).

For example, we might diagram the execution of

```asm
movq (%rcx), %rax
addq $24, %rax
```

as

|      | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|:-----|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|`movq`| F | D | E | M | W |   |   |
|`addq`|   | F | D | D | E | M | W |

We can read a lot of information from this diagram; for example

- We have a five-stage pipeline (F, D, E, M, W)
- We ran two instructions (2 rows)
- They had a joint latency of 7 cycles (7 columns)
- The `addq` stalled while in the decode stage
    - meaning decode couldn't complete until some earlier instruction had done something
- During cycle 4, nothing was in the execute stage (it had been given a `nop` as part of the stall).
    If we looked at the stages then, we'd see
    
    | F | D | E | M | W |
    |:-:|:-:|:-:|:-:|:-:|
    |*next*|`addq`|*nop*|`movq`|*previous*|
    
    Contrast that with cycle 3, which looked like
    
    | F | D | E | M | W |
    |:-:|:-:|:-:|:-:|:-:|
    |*next*|`addq`|`movq`|*previous*|*instr before that*|
    
    This stage-style view can be created by taking a column and turning it sideways, then filling in missing stages with *nop*.
    

There are some things a pipeline diagram should never show:

- Never show the same stage on two rows of the same column.
    That would imply that two different instructions are using the same stage at the same time.
- Never show any row skipping a step.
    While it is possible to design pipelines that skip steps, if that level of work is intended then out-of-order makes more sense.
- Never show a gap in a row.
    An instruction might proceed normally, or stall by repeating a stage, or be cancelled altogether due to misprediction, but it can't vanish for a cycle and then come back later.

# Your Task

With a partner, visit the [pipline PA](pa08-pipeline.html) and do the following

1. Read and agree on the meaning of the bulleted list of assumptions
2. *After* you understand all of those items, each visit the online pipeline diagram tool for the PA and fill it in together

Your *lab* credit will be based on being present and engaged.
Your *PA* credit will be based on the correctness of your solution.
