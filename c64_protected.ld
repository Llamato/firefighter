/* C64 Memory Map with Hardware Protection
   Target: llvm-mos (mos-c64-clang)
   Excludes: Hardware registers ($D000-$DFFF), Bitmap memory ($2000-$3FFF), Sprite memory blocks
*/

MEMORY
{
    /* System ROM - Read Only */
    ROM (rwx)  : ORIGIN = 0xA000, LENGTH = 0x6000   /* $A000-$FFFF - BASIC/KERNAL ROM */
    
    /* System RAM - Available for program */
    RAM (rwx) : ORIGIN = 0x0801, LENGTH = 0x07FF   /* $0801-$0FFF - BASIC program area */
    
    /* Additional RAM but careful with stack */
    RAM_HIGH (rwx) : ORIGIN = 0x1000, LENGTH = 0x1000  /* $1000-$1FFF - Free RAM */
    
    /* PROTECTED AREAS - NOT AVAILABLE TO LINKER */
    /* $0200-$02FF - KERNAL/BASIC variables */
    /* $0300-$03FF - KERNAL/BASIC vectors */
    /* $0400-$07FF - Screen memory (text mode) */
    
    /* $2000-$3FFF - BITMAP MEMORY (320x200 high-res) - PROTECTED */
    /* $D000-$DFFF - VIC-II HARDWARE REGISTERS - PROTECTED */
    /* $DC00-$DCFF - CIA #1 - PROTECTED */
    /* $DD00-$DDFF - CIA #2 - PROTECTED */
    /* $D800-$DBFF - COLOR RAM - PROTECTED */
}

SECTIONS
{
    /* Standard sections */
    .text : {
        *(.text)
        *(.text.*)
    } > RAM
    
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    } > RAM
    
    .data : {
        *(.data)
        *(.data.*)
    } > RAM
    
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    } > RAM
    
    /* Stack configuration */
    .stack (NOLOAD) : {
        . = . + 0x0100;  /* Reserve 256 bytes for stack */
    } > RAM_HIGH
    
    /* Ensure alignment for stack */
    . = ALIGN(0x0100);
    
    /* Note: The following areas are NOT defined in MEMORY
       so the linker will NOT place anything here:
       
       $0000-$00FF: Zero page (use -mreserve-zp=N to reserve)
       $0100-$01FF: Stack (handled by compiler)
       $0200-$03FF: System variables
       $0400-$07FF: Screen memory (text mode)
       $2000-$3FFF: BITMAP MEMORY - PROTECTED
       $4000-$7FFF: Free RAM (if you want to use this, add to MEMORY)
       $8000-$9FFF: Cartridge ROM (if applicable)
       $A000-$FFFF: ROM/System
       $D000-$DFFF: VIC-II HARDWARE - PROTECTED
       $D800-$DBFF: COLOR RAM - PROTECTED
       $DC00-$DDFF: CIA #1/#2 - PROTECTED
    */
}

/* Optional: Define symbols for debugging */
__RAM_START = ORIGIN(RAM);
__RAM_END = ORIGIN(RAM) + LENGTH(RAM);
__STACK_TOP = ORIGIN(RAM_HIGH) + LENGTH(RAM_HIGH);