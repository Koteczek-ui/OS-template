[ORG 0x7c00]
KERNEL_OFFSET equ 0x1000

mov [BOOT_DRIVE], dl ; BIOS sets DL to the drive number 

mov bp, 0x9000
mov sp, bp

call load_kernel
call switch_to_pm
jmp $

[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET
    mov al, 32         ; Number of sectors to read 
    mov ch, 0x00       ; Cylinder 0
    mov dh, 0x00       ; Head 0 
    mov cl, 0x02       ; Start reading from second sector 
    mov dl, [BOOT_DRIVE]
    mov ah, 0x02       ; BIOS read sector function 
    int 0x13
    ret

[bits 16]
switch_to_pm:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp 0x08:init_pm 

gdt_start:
    dq 0x0
gdt_code:
    dw 0xffff, 0x0
    db 0x0, 10011010b, 11001111b, 0x0 
gdt_data:
    dw 0xffff, 0x0
    db 0x0, 10010010b, 11001111b, 0x0 
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start 

[bits 32]
init_pm:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax 
    mov ebp, 0x90000
    mov esp, ebp
    
    call KERNEL_OFFSET ; Jump to the start of the C kernel 
    jmp $

BOOT_DRIVE db 0

; Boot sector padding and magic number
times 510-($-$$) db 0
dw 0xaa55 

; --- INCLUDE THE KERNEL ---
; This places the compiled C code exactly at the 512-byte mark
incbin "bin/kernel.bin"

