BITS 64

SECTION .text

extern exception_dispatch
extern page_fault_handler

global isr0
global isr6
global isr8
global isr13
global isr14

; --------------------------------------------------
; Exceptions WITHOUT a CPU-pushed error code
; --------------------------------------------------

isr0:
    push qword 0             ; Synthetic error code
    push qword 0             ; Vector
    jmp exception_common

isr6:
    push qword 0             ; Synthetic error code
    push qword 6             ; Vector
    jmp exception_common

; --------------------------------------------------
; Exceptions WITH a CPU-pushed error code
; --------------------------------------------------

isr8:
    push qword 8             ; Vector; CPU already pushed error code
    jmp exception_common

isr13:
    push qword 13            ; Vector; CPU already pushed error code
    jmp exception_common

; --------------------------------------------------
; Common exception handler
;
; Stack at entry:
;   [rsp + 0]  = vector
;   [rsp + 8]  = error code
;   [rsp + 16] = RIP
; --------------------------------------------------

exception_common:
    cli
    cld

    ; Save general-purpose registers.
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; 15 registers = 120 bytes.
    ; Recover vector, error code, and RIP.
    mov rdi, [rsp + 120]
    mov rsi, [rsp + 128]
    mov rdx, [rsp + 136]

    ; Align stack for the SysV AMD64 C ABI.
    mov rbx, rsp
    and rsp, -16

    call exception_dispatch

    ; exception_dispatch must not return.
    mov rsp, rbx

.exception_halt:
    cli
    hlt
    jmp .exception_halt

; --------------------------------------------------
; Page fault (#PF), vector 14
; CPU pushes an error code automatically.
; --------------------------------------------------

isr14:
    cli
    cld

    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Error code is at offset 120 after saving registers.
    mov rdi, [rsp + 120]

    mov rbx, rsp
    and rsp, -16

    call page_fault_handler

    mov rsp, rbx

.page_fault_halt:
    cli
    hlt
    jmp .page_fault_halt
EOF