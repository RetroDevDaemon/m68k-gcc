MEMORY
{
  ROM (rx) : ORIGIN = 0x0, LENGTH = 0x1fffff
  RAM (!rx): org = 0xFF0000, l = 0xffff
}
SECTIONS
{

  .text : {
    *(.text)
    *(.rodata) 
  } > ROM 
  . = 0xFF0000;
  .data : AT (ADDR(.text) + SIZEOF(.text)){
    *(.data)
  } > RAM
  .bss : { 
    *(.bss)
    *(COMMON)
  } > RAM
     
}
OUTPUT_FORMAT(binary)