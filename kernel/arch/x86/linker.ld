/*
  Bootloader looks at this image and starts
  at the symbol marked as the entrypoint.
*/
OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    /*
      Begin putting sections at 1 MiB, a conventional place for
      kernels to be loaded by the bootloader.
    */
    . = 1M;
    __link_mem_begin = .;

    /* Next we'll put the .text section. */
    .text : ALIGN(4K)
    {
        __link_mem_code = .;
        *(.multiboot)
        *(.text)
    }

    /* Read-only data. */
    .rodata : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Read-write data (initialized) */
    .data : ALIGN(4K)
    {
        *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    .bss : ALIGN(4K)
    {
        __link_mem_bss = .;
        *(COMMON)
        *(.bss)
        *(.stack)
    }

    /* Extras */
    .extra : ALIGN(4K)
    {
        __link_mem_extra = .;
        *(.note.gnu.build-id)
        *(.comment)
    }

    __link_mem_end = .;
}
