.text

*-------------------------------------------------------
*
*       Sega startup code for the GNU Assembler
*       Translated from:
*       Sega startup code for the Sozobon C compiler
*       Written by Paul W. Lee
*       Modified by Charles Coty
*       Modified by Stephane Dallongeville
*       Modified by Chris McClelland
*		Modified by Chris Smith: 02/12/2015.
*       Modified by Ben Ferguson 4/15/2021.
*
*-------------------------------------------------------
        .org    0x00000000

_Start_Of_Rom:
_Vectors_68K:
        dc.l    0x00FFFE00              /* Stack address */
        dc.l    _Entry_Point            /* Program start address */
        dc.l    _Bus_Error
        dc.l    _Address_Error
        dc.l    _Illegal_Instruction
        dc.l    _Zero_Divide
        dc.l    _Chk_Instruction
        dc.l    _Trapv_Instruction
        dc.l    _Privilege_Violation
        dc.l    _Trace
        dc.l    _Line_1010_Emulation
        dc.l    _Line_1111_Emulation
        dc.l     catch, catch, catch, catch
        dc.l     catch, catch, catch, catch
        dc.l     catch, catch, catch, catch
        dc.l    catch, _INT, _EXTINT, _INT
        dc.l    HBlank                                                  /* LEVEL4 */
        dc.l    _INT
        dc.l    VBlank                                                  /* LEVEL6 */
        dc.l    _INT
        dc.l    _Reset,_INT,_INT,_INT,_INT,_INT,_INT,_INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT

	.ascii	"SEGA MEGA DRIVE " 					/* Console Name (16) */
	.ascii	"(C)2021         "					/* Copyright Information (16) */
	.ascii	"Star Thrall                                     "	/* Domestic Name (48) */
	.ascii	"Star Thrall                                     "	/* Domestic Name (48) */
	.ascii	"GM 00000000-00"					/* Serial Number (2, 14) */
	dc.w	0x0000							/* Checksum (2) */
	.ascii	"J               "					/* I/O Support (16) */
	dc.l	0x00000000						/* ROM Start Address (4) */
	dc.l	_end_rom						/* ROM End Address (4)  (20kh = 128kb) */
	dc.l	0x00FF0000						/* Start of RAM (4) */
	dc.l	0x00FFFFFF						/* End of RAM (4) */
	.ascii	"                        "				/* Modem Support (12) */
	.ascii	"                                        "		/* Memo (40) */
	.ascii	"JUE             "					/* Country Support (16) */

_Entry_Point:
        move    #0x2700,%sr                                             /* Disable interrupts 7 and below */ 
	tst.l   0xa10008
        bne.s   SkipJoyDetect
        tst.w   0xa1000c
SkipJoyDetect:
        bne.s   SkipSetup

        lea     Table,%a5
        movem.w (%a5)+,%d5-%d7
        movem.l (%a5)+,%a0-%a4
* Check Version Number
        move.b  -0x10ff(%a1),%d0
        andi.b  #0x0f,%d0
        beq.s   WrongVersion
* Sega Security Code (SEGA)
        move.l  #0x53454741,(0xa14000).l
WrongVersion:
        move.w  (%a4),%d0
        moveq   #0x00,%d0
        movea.l %d0,%a6
        move    %a6,%usp
        move.w  %d7,(%a1)
        move.w  %d7,(%a2)
        jmp     Continue

Table:
        dc.w    0x8000,0x3fff,0x0100
        dc.l    0xA00000,0xA11100,0xA11200,0xC00000,0xC00004

SkipSetup:
        move.w  #0,%a7
        jmp     _Reset

Continue:
VDP_Init:
	    move.w  #0,%a7
	    lea	SetupValues(pc),a5
	    movem.w	(a5)+,d5-d7
	    movem.l	(a5)+,a0-a4
	    moveq	#0x17,d1
VDPInitLoop:
	    move.b	(a5)+,d5		/*	add $8000 to value			*/
	    move.w	d5,(a4)			/*	move value to	VDP register		*/
	    add.w	d7,d5			/*	next register				*/
	    dbf	d1,VDPInitLoop

ClearWRAM_Init:
            lea     0xff0000,%a0
            move.L   #0x0,%d0
            move.w  #0x3FFF,%d1
ClearWRAM:
            move.l  %d0,(%a0)+
            dbra    %d1,ClearWRAM

ClearVRAM_Init:
            move.L  #0x0,%d0
            move.w  #0x3FFF,%d1
	    move.W	#0x8F02,0x00C00004			/*	Set auto-increment to 2 Bytes.	*/
	    move.L	#0x40000003,(vdp_control_port).L	/*	Set VRAM to start address.	*/
ClearVRAM:
	    move.L	d0,(vdp_data_port).L
	    dbra	%d1,ClearVRAM

* ***********************************************
* copy initialized variables from ROM to Work RAM
* ***********************************************
        lea     _stext,%a0
        lea     0xFF0000,%a1
        move.l  #_sdata,%d0
        lsr.l   #1,%d0
        beq     NoCopy
        subq.w  #1,%d0
CopyVar:
        move.w  (%a0)+,(%a1)+
        dbra    %d0,CopyVar
NoCopy:

* Jump to initialisation process...
.even
	jmp	_start 

*------------------------------------------------
*
*       interrupt functions
*
*------------------------------------------------

registersDump:
        rts

busAddressErrorDump:
        jmp registersDump

exception4WDump:
        jmp registersDump

exceptionDump:
        jmp registersDump


_Bus_Error:
        jsr busAddressErrorDump
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte

_Address_Error:
        jsr busAddressErrorDump
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte

_Illegal_Instruction:
        jsr exception4WDump
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte

_Zero_Divide:
        jsr exceptionDump
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte

_Chk_Instruction:
        jsr exception4WDump
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte

_Trapv_Instruction:
        jsr exception4WDump
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte
	

_Reset:
	    move.w	#0x2700, %sr
	    reset
	    lea	0x430000, a0
	    lea	0x480000, a1


_Privilege_Violation:
        jsr exceptionDump
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte

_Trace:
        jsr exceptionDump
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte

_Line_1010_Emulation:
_Line_1111_Emulation:
        jsr exceptionDump
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte

_Error_Exception:
        jsr exceptionDump
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte

_INT:
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte

_EXTINT:
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte

_HINT:
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte

_VINT:
        movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr    (%a0)
        movem.l (%sp)+,%d0-%d1/%a0-%a1
        rte


*.data
.even
SetupValues:	
.word		0x8000			/* VDP register start number*/
.word		0x3FFF			/* size of RAM/4*/
.word		0x100			/* VDP register diff*/

.long		z80_ram			/* start	of Z80 RAM*/
.long		z80_bus_request		/* Z80 bus request*/
.long		z80_reset		/* Z80 reset*/
.long		vdp_data_port		/* VDP data*/
.long		vdp_control_port	/* VDP control*/

.byte		0x04			/* VDP 0x80 - 8-colour mode*/
.byte		0x44			/* VDP 0x81 - Megadrive mode, DMA enable		*/
.byte		(0xC000>>10)		/* VDP 0x82 - foreground nametable address		*/
.byte		(0xF000>>10)		/* VDP 0x83 - window nametable address		*/
.byte		(0xE000>>13)		/* VDP 0x84 - background nametable address		*/
.byte		(0xD800>>9)		/* VDP 0x85 - sprite table address		*/
.byte		0			/* VDP 0x86 - unused		*/
.byte		0			/*VDP 0x87 - background colour		*/
.byte		0			/* VDP 0x88 - unused		*/
.byte		0			/* VDP 0x89 - unused		*/
.byte		255			/* VDP 0x8A - HBlank register		*/
.byte		0			/* VDP 0x8B - full screen scroll		*/
.byte		0x81			/* VDP 0x8C - 40 cell display		*/
.byte		(0xFC00>>10)		/* VDP 0x8D - hscroll table address		*/
.byte		0			/* VDP 0x8E - unused		*/
.byte		1			/* VDP 0x8F - VDP increment		*/
.byte		1			/* VDP 0x90 - 64 cell hscroll size		*/
.byte		0			/* VDP 0x91 - window h position		*/
.byte		0			/* VDP 0x92 - window v position		*/
.word		0xFFFF			/* VDP 0x93/94 - DMA length		*/
.word		0			/* VDP 0x95/96 - DMA source		*/
.byte		0x80			/* VDP 0x97 - DMA fill VRAM		*/
.long		0x40000080		/* VRAM address 0		*/
.even
.byte		0xAF			/* xor	a		*/
.byte		0x01, 0xD9, 0x1F	/* ld	bc,1fd9h		*/
.byte		0x11, 0x27, 0x00	/* ld	de,0027h		*/
.byte		0x21, 0x26, 0x00	/* ld	hl,0026h		*/
.byte		0xF9			/* ld	sp,hl		*/
.byte		0x77			/* ld	(hl),a		*/
.byte		0xED, 0xB0		/* ldir		*/
.byte		0xDD, 0xE1		/* pop	ix		*/
.byte		0xFD, 0xE1		/* pop	iy		*/
.byte		0xED, 0x47		/* ld	i,a		*/
.byte		0xED, 0x4F		/* ld	r,a		*/
.byte		0xD1			/*pop	de		*/
.byte		0xE1			/* pop	hl		*/
.byte		0xF1			/* pop	af		*/
.byte		0x08			/* ex	af,af'		*/
.byte		0xD9			/* exx		*/
.byte		0xC1			/* pop	bc		*/
.byte		0xD1			/* pop	de		*/
.byte		0xE1			/* pop	hl		*/
.byte		0xF1			/* pop	af		*/
.byte		0xF9			/* ld	sp,hl		*/
.byte		0xF3			/* di		*/
.byte		0xED, 0x56		/* im1		*/
.byte		0x36, 0xE9		/* ld	(hl),e9h		*/
.byte		0xE9			/* jp	(hl)		*/

.word		0x8104			/* VDP display mode		*/
.word		0x8F02			/* VDP increment		*/
.long		0xC0000000		/* CRAM write mode		*/
.long		0x40000010		/* VSRAM address 0		*/

.byte		0x9F, 0xBF, 0xDF, 0xFF	/* values for PSG channel volumes		*/
*.equ	vdp_data_port,		0xC00000
*z80_ram:		equ $A00000	; start of Z80 RAM
.equ	z80_ram,0xA00000	/*	start of Z80 RAM.	*/
*z80_bus_request:	equ $A11100
.equ	z80_bus_request,0xA11100
*z80_reset:		equ $A11200
.equ	z80_reset,0xA11200
*************************************************
**		[VDP]
*************************************************
.equ	vdp_data_port,0xC00000
.equ	vdp_control_port,0xC00004
.equ 	vdp_counter,0xC00008