ENTRY (start)

SECTIONS{
    . =0x00100000;

    .multiboot ALIGN(4) : {
          *(.multiboot)
    }

    .text ALIGN(4K) : {
          *(.text)
    }

    .data ALIGN(4K) : {
     *(.data*)     
    }

    .bss ALIGN(4K) : {
        *(.bss)         
    }

    /DISCARD/ : {          
        *(.comment)
        *(.note.gnu.property)
    }
}
