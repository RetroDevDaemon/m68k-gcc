MEMORY
{
  ROM (rx) : ORIGIN = 0x0, LENGTH = 0x1fffff
  RAM (!rx): org = 0xFF0000, l = 0xffff
}
SECTIONS
{
  .text : {
    . = ALIGN(2);
    _stext = .;
    *(.text)
    . = ALIGN(2);
    *(.rodata)
    . = ALIGN(2); 
  } > ROM 
  _end_rom = .;
  . = 0xFF0000;
  .data : AT (ADDR(.text) + SIZEOF(.text) + SIZEOF(.rodata)){
    . = ALIGN(2);
    _sdata = .;
    *(.data)
    . = ALIGN(2);
  } > RAM
  .bss : { 
    . = ALIGN(2);
    *(.bss)
    . = ALIGN(2);
    *(COMMON)
    . = ALIGN(2);
  } > RAM
  . = 0xff500;
}
OUTPUT_FORMAT(binary)
