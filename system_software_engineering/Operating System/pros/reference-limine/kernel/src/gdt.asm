BITS 64

global gdt_load
extern gdt

section .text

gdt_load:

    lgdt [rel gdt_ptr]

    ; Reload CS.
    ; CS cannot be loaded with MOV.
    ; A far return performs the required reload.

    push qword 0x08
    lea rax, [rel .reload_cs]
    push rax
    retfq

.reload_cs:

    ; Reload kernel data segments.
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; FS/GS are not used yet.
    xor eax, eax
    mov fs, ax
    mov gs, ax

    ret


section .data

align 8

gdt_ptr:
    dw 7 * 8 - 1
    dq gdt
