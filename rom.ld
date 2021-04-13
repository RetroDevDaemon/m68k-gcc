MEMORY
{
  ROM (rx) : ORIGIN = 0x200, LENGTH = 128K
  RAM (!rx): org = 0xFF0000, l = 0xffff
}
SECTIONS
{
  .text : {
    *(.text)
  } > ROM 
  
  . = 0xFF0000;
  .bss : { 
    *(.bss) 
  } > RAM
  .data : AT (ADDR(.text) + SIZEOF(.text) + SIZEOF(.bss)){ 
    *(.data)
    *(.rodata) 
  } > RAM 
}
OUTPUT_FORMAT(binary)