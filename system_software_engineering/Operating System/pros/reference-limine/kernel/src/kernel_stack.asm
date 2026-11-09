BITS 64

global kernel_stack_switch_and_call

section .text

; void kernel_stack_switch_and_call(
;     uint64_t stack_top,
;     void (*continuation)(void)
; );
;
; RDI = new stack top
; RSI = continuation function
;
; This function never returns to the old stack.

kernel_stack_switch_and_call:

    ; Switch to the PrOS-owned stack.
    mov rsp, rdi

    ; SysV x86-64 ABI:
    ; Before a normal CALL, RSP should be 16-byte aligned.
    and rsp, -16

    ; We are jumping rather than calling the continuation,
    ; therefore manufacture the stack state that the called
    ; function expects.
    sub rsp, 8

    jmp rsi
