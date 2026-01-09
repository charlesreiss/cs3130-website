// while A is working:
movq $99, %rax
movq %rax, 0x10000
// MEMORY[0x10000] <- RAX
...
