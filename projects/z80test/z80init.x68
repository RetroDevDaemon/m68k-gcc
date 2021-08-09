; Working z80 program init

PRGSIZE equ _ENDZ80-Z80PRG 

        nop 
        ; Request z80 bus
	move.w	$100,(z80_bus_request).l
        ; Enable reset
	move.w	$100,(z80_reset).l
; COPY z80prg[] INTO Z80 MEMORY (A00000+)
	movea.l Z80PRG,a0
	movea.l $a00000,a1
	move.l  PRGSIZE,d1
    .Z80COPYLOOP:
	move.b  (a0)+,d0
	move.b  d0,(a1)+
	subq 	1,d1
	bne     .Z80COPYLOOP	
; // reset, start z80
	move.w  0,(z80_reset).l 
	nop 
	nop 
	nop 
	nop 
	move.w  $100,(z80_reset).l 
	move.w  0,(z80_bus_request).l

Z80PRG:
        defb [...]
_ENDZ80