/*Define memory map*/
MEMORY
{
  FLASH (rx)   : ORIGIN = 0x00000000,  LENGTH = 128K
  RAM  (rwx)   : ORIGIN = 0x1FFFF000,  LENGTH = 16K
}

/*Libraries
 *Baremetal does not use C libraries; however,
 *libgcc is needed when using C features that do not map to
 *instructions in the ARM M0+ set, such as divide
 */
GROUP(libgcc.a)


/*This defines the entry point of the system*/
ENTRY(_reset_init)

/*Map sections to memory*/
SECTIONS
{
	.text : {
		KEEP(*(.interrupt_vectors))
		KEEP(*(.system_startup_code))
		__startup_end = .;
		. = 0x400;
		KEEP(*(.flash_configuration_field))
		. = ALIGN(4);
		*(.text*)
		*(.rodata*)
		. = ALIGN(4);
		__etext = .;
    } > FLASH

        
    .data :
    {
        __data = .;
        *(.data*)
        . = ALIGN(4);
        __edata = .;
    } > RAM AT >FLASH

    .bss :
    {
        __bss_start = .;
        *(.bss*)
    } > RAM
    
    __bss_end = .;
    __end = .;
    /* Set stack top to end of RAM */
    __stacktop = ORIGIN(RAM) + LENGTH(RAM);
    PROVIDE(__stack = __stacktop);
    /*Sanity Checks*/
    
}
    ASSERT( __startup_end <= 0x0400 , "Startup code size too big"); 
