|Vector Table - start of ROM
    .file   "header.s"
    .text
    .align 2

    .dc.l 0xfffffe00     
|stack pointer start location, usually $FFFFFE00
    .dc.l 0               
|program start location
    .dc.l 0               
|bus error
    .dc.l 0               
|address error
    .dc.l 0               
|illegal instruction
    .dc.l 0               
|divide by zero
    .dc.l 0               
| CHK exception
    .dc.l 0               
| TRAPV
    .dc.l 0               
| privilege violation
    .dc.l 0               
| TRACE exception
    .dc.l 0               
| Line-A emu
    .dc.l 0               
| Line-F emu
    .dc.l 0               
| unused 0
    .dc.l 0               
| unused 1
    .dc.l 0               
| unused 2
    .dc.l 0               
| unused 3
    .dc.l 0               
| unused 4
    .dc.l 0               
| unused 5
    .dc.l 0               
| unused 6
    .dc.l 0               
| unused 7
    .dc.l 0               
| unused 8
    .dc.l 0               
| unused 9
    .dc.l 0               
| unused 10
    .dc.l 0               
| unused 11
    .dc.l 0               
| spurious exception
    .dc.l 0               
| IRQ level 1
    .dc.l 0               
| IRQ level 2
    .dc.l 0               
| IRQ level 3
    .dc.l 0               
| IRQ level 4 (HBLank)
    .dc.l 0               
| IRQ level 5
    .dc.l 0               
| IRQ level 6 (VBlank)
    .dc.l 0               
| IRQ level 7
    .dc.l 0               
| TRAP 00 exception
    .dc.l 0               
| TRAP 01 exception
    .dc.l 0               
| TRAP 02 exception
    .dc.l 0               
| TRAP 03 exception
    .dc.l 0               
| TRAP 04 exception
    .dc.l 0               
| TRAP 05 exception
    .dc.l 0               
| TRAP 06 exception
    .dc.l 0               
| TRAP 07 exception
    .dc.l 0               
| TRAP 08 exception
    .dc.l 0               
| TRAP 09 exception
    .dc.l 0               
| TRAP 10 exception
    .dc.l 0               
| TRAP 11 exception
    .dc.l 0               
| TRAP 12 exception
    .dc.l 0               
| TRAP 13 exception
    .dc.l 0               
| TRAP 14 exception
    .dc.l 0               
| TRAP 15 exception
    .dc.l 0               
| unused 12
    .dc.l 0               
| unused 13
    .dc.l 0               
| unused 14
    .dc.l 0               
| unused 15
    .dc.l 0               
| unused 16
    .dc.l 0               
| unused 17
    .dc.l 0               
| unused 18
    .dc.l 0               
| unused 19
    .dc.l 0               
| unused 20
    .dc.l 0               
| unused 21
    .dc.l 0               
| unused 22
    .dc.l 0               
| unused 23
    .dc.l 0               
| unused 24
    .dc.l 0               
| unused 25
    .dc.l 0               
| unused 26
    .dc.l 0               
| unused 27
    .ascii "SEGA MEGA DRIVE " 
| console ID
    .ascii "(C)SEGA 1991.APR" 
| sonic
    .ascii "SONIC THE               HEDGEHOG                " 
| Domestic name
    .ascii "SONIC THE               HEDGEHOG                " 
| International name
    .ascii "GM 00001009-00"   
| Serial/version number (Rev 0)
    .dc.w 0x0
    .ascii "J               " 
| I/O support
    .dc.l 0x1000		
| Start address of ROM
    .dc.l 0x1000-1		
| End address of ROM
    .dc.l 0xFF0000		
| Start address of RAM
    .dc.l 0xFFFFFF		
| End address of RAM
|if EnableSRAM=1
|		dc.b 0x52, 0x41, 0xA0+(BackupSRAM<<6)+(AddressSRAM<<3), 0x20
|else
	.dc.l 0x20202020
|endc
    .dc.l 0x20202020		
| SRAM start ($200001)
    .dc.l 0x20202020		
| SRAM end ($20xxxx)
    .ascii "                                                    " 
| Notes (unused, anything can be put in this space, but it has to be 52 bytes.)
    .ascii "JUE             " 
| Region (Country code)
| (0x1ff)
