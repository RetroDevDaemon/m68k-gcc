MEMORY
{
  ROM (rx) : ORIGIN = 0x0, LENGTH = 0x1fffff
  RAM (!rx): org = 0xFF0000, l = 0xffff
}
SECTIONS
{
  header 0x0 :
   {
     *(.header)
   }
  . = 0x200;
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