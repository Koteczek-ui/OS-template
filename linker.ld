ENTRY(main)
OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 0x1000;

    .text : AT(0x1000)
    {
        *(.entry)
        *(.text)
        *(.rodata*)
    }

    .data : 
    { 
        *(.data) 
    }

    .bss : 
    { 
        *(.bss) 
        *(COMMON) 
    }

    . = ALIGN(4096);
}

