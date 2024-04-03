---
title: OOO Exercise
...


# Hypothetical OOO processor

For this assignment, we will consider an out-of-order processor which has two execution units for
arithmetic instructions, and one execution unit for memory instructions.

### This Processor's Pipeline 
Instructions in this processor are executed as follows:

*  first, they go through a set of in-order pipeline stages, two a time:

    *  fetch, where instructions are read from the pipeline stage. For this exercise, assume branch prediction is perfect so two new instructions
       are fetched every cycle
    *  decode, where the machine is interpreted
    *  rename, where register renaming takes place and instructions are placed into an instruction queue, available ot be issued as early
       as the next cycle

*  then, each cycle instructions are issued from the instruction queue:
    
    *  each cycle, the instruction queue is scanned for instructions whose operands are ready. (For this exercise, we'll only account for their register operands
        being ready and assume that we do not need to worry about data memory-related dependencies.)
        *  operands for an instruction are considered ready during the cycle in which they finish execution
    *  the first memory instruction available and first two non-memory instructions available are selected to be issued. (If fewer instructions are available, as many are issued as possible.)
    *  during the cycle an instruction is issued, its register value inputs are read or forwarded

*  when an instruction is issued, in the following cycles it is executed on an execution unit
    *  for memory instructions, this execution unit is a pipelined data cache. It receives one instruction per cycle
        and produces the result after three cycles. To simplfy this exercise, we will assume no cache misses.
    *  for non-memory instructions, this execution unit is one of two arithmetic logic unit. It receives one instruction per
        cycle and produces the result near the end of this cycle. 
    *  the first available memory instruction (if any) is sent to a data cache, which is pipelined. The data cache accepts one instruction per cycle
        and takes three cycles to produce the result of the read or write
    *  the first two available non-memory instructions are sent to one of two arithmetic execution units. These execution units each take one cycle
        to compute their result

*  in the cycle after an instruction is executed it is written back to the register file in one cycle
*  finally, up to two instructions are *committed* each cycle using information placed in a reorder buffer during the issue stage.
    An instruction is only committed if all instructions fetched before have also committed.

Given these stages, an non-memory instruction will take 7 cycles to go through the pipeline if it does not need to wait for it operands
or wait for earlier instructions to commit (fetch, decode, rename, issue/register read, execute, writeback, commit).
A memory instruction will take 9 cycles (two extra cycles of `execution' because of the speed of the data cache).

When operands are not available, an instruction will spend extra cycles waiting in the instruction queue between its rename
and issue stage. When an instruction is written back but some prior instructions have not done so,
then the instruction will spend extra cycles waiting in the reorder buffer before it is committed.

### This processor's registers

The processor has 15 logical registers `%r01` through `%r15`, but these are implemented using 64 physical registers and register renaming. We call the physical registers `%x01` through `%x64`.

We will show instructions in a three-argument form like (**regardless of whether register renaming has taken place**):
    
    add %r01, %r02 -> %r03

this indicates to add %r01 and %r02 and put the result into %r03.

Answer the questions on the online answer sheet (link TBD) <!-- [this answer sheet](https://kytos02.cs.virginia.edu/cs3130-spring2024/ooo.php). -->

## Part 1: register renaming

Assume initially that `%r01` is assigned to `%x01`, `%r02` to `%x02`, and so on.
Registers `%x16` through `%x63` are available for register renaming.

Rewrite the following instructions using the `%x01` taking into account how register renaming
might occur that would allow all of these instructions to be placed in the instruction queue
at once:

    add %r01, %r01 -> %r01
    add %r02, %r01 -> %r01
    add %r03, %r01 -> %r01
    add %r04, %r01 -> %r01
    movq 0x1234(%r05) -> %r02
    imul %r02, %r03 -> %r03
    movq 0x1234(%r06) -> %r03
    imul %r02, %r04 -> %r04

Record your answer at the answer sheet linked above.

## Part 2: instruction dispatch

Suppose the instruction queue for this processor contains the following instructions
*after renaming*:

    A. add %x05, %x06 -> %x16
    B. sub %x16, %x07 -> %x17
    C. movq 0x100(%x05) -> %x18
    D. imul %x17, %x09 -> %x19
    E. add %x05, %x07 -> %x20
    F. sub %x18, %x07 -> %x21
    G. add %x20, %x19 -> %x22
    H. movq 0x200(%x22) -> %x23
    I. imul %x08, %x16 -> %x24

Initially the value of registers `%x01` through `%x15` is available.


Complete the timeline to show how all the instructions can be issued and executed:

                    cycle 1    2    3   4    5    6   7
    execution unit:       
    arithmetic 1          A
    arithmetic 2          E
    memory stage 1        C
    memory stage 2             C
    memory stage 3                  C
    
Record your answer at the answer sheet linked above.

## Part 3: pipeline diagram

Complete a pipeline diagram for the processor running the following instructions
(shown in the form from **before** register renaming):

    1. add %r01, %r01 -> %r01
    2. add %r02, %r03 -> %r03
    3. add %r04, %r05 -> %r05
    4. add %r05, %r01 -> %r01
    5. add %r01, %r01 -> %r01
    6. add %r04, %r05 -> %r05
    7. add %r02, %r04 -> %r04

Identify the stages as:

*  F for when the instruction is feteched
*  D for when an instruction is decoded
*  R for when an instruction is renamed and dispatched into an instruction queue
*  I for when an instruction is issued from an instruction queue and its registers are read
*  E for when an instruction is being executed (after it is issued)
*  W for when an instruction's results are written back
*  C for when an instruction is committed

Leave the specified stage blank when an instruction is not being processed in any of the above ways (such as when
the instruction is in the instruction queue waiting to be issued or any cycles between when an instruction is written back
and when it is committed).

The first three rows are:

    1 F D R I E W C
    2 F D R I E W C
    3   F D R I E W C

complete the remaining rows of the table at the answer sheet linked above.

You may assume that register values computed by previous instructions are available and that physical registers are free
such that stages will not be delayed for missing previous register values or lack of available physical registers.

