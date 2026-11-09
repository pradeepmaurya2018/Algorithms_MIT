BITS 64

global context_switch
global thread_bootstrap

extern thread_exit

section .text

context_switch:

    ; Save callee-saved registers.

    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    ; Save current thread's stack pointer.

    mov [rdi], rsp

    ; Switch to next thread's stack.

    mov rsp, rsi

    ; Restore next thread's registers.

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp

    ; Return into the restored RIP.

    ret


thread_bootstrap:

    ; R12 contains the thread entry function.

    call r12

    ; The thread function returned.

    call thread_exit

    ; thread_exit() must never return.

    cli

.halt:
    hlt
    jmp .halt