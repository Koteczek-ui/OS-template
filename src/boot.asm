[ORG 0x7c00]
KERNEL_OFFSET equ 0x1000

mov [BOOT_DRIVE], dl 

mov bp, 0x9000
mov sp, bp

call load_kernel
call switch_to_pm
jmp $

[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET
    mov al, 32         
    mov ch, 0x00       
    mov dh, 0x00       
    mov cl, 0x02       
    mov dl, [BOOT_DRIVE]
    mov ah, 0x02       
    int 0x13
    jc disk_err
    
    cmp al, 32
    jne disk_err
    ret

disk_err:
    mov si, disk_err_MSG
    call print_err
    jmp $

print_err:
    mov ah, 0x0e
.loop:
    lodsb
    cmp al, 0
    je .done
    mov bl, 0x04
    int 0x10
    jmp .loop
.done:
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
    
    call KERNEL_OFFSET 
    jmp $

BOOT_DRIVE db 0
disk_err_MSG db "Disk Read Error!", 0

times 510-($-$$) db 0
dw 0xaa55

