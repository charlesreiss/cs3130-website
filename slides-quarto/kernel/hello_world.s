.globl _start
.data
hello_str: .asciz "Hello, World!\n"
.text
_start:
  movq $1, %rax # 1 = "write"
  movq $1, %rdi # file descriptor 1 = stdout
  movq $hello_str, %rsi 
  movq $15, %rdx # 15 = strlen("Hello, World!\n")
  syscall
    
  movq $60, %rax # 60 = exit
  movq $0, %rdi
  syscall
