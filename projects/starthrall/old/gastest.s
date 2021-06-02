
.text
/*********************************************************************
 **	GNU Asm 68000
 *********************************************************************/

/*********************************************************************
**  Copyright (C) 2013 Christopher Smith
**  This program is not free software: you cannot redistribute it and/or-
**  modify or publish it without agreement from the original author-
**  expressly consenting for you to do so.
**********************************************************************/

/*********************************************************************
** Name          : md_basictest_gas.s
** Module        : none.
** Author        : C.Smith - MintyTheCat.
** Purpose       : Getting a simple MD demo up and running - firstly using-
**               : the sega.s file found all over the place.
**               : The other version will use AxLib2's sys.s' AX2_Sys_InitMD for the MD instead.
**               : I need to determine why my AX2_Sys_InitMD routine causes the MD's VDP to-
**               : become unstable.
** Date-First    : none.
** Date-Last     : none.
** Notes         : Tab Space = 8.
** Changes       : none.
**********************************************************************/

/*********************************************************************
**  [Linking/Addressing]
**********************************************************************/
.even

/*********************************************************************
**  [Source-Includes]
**********************************************************************/
.even

/*********************************************************************
**  [Binary-Includes]
**********************************************************************/
.even

/*********************************************************************
**  [Module]
**********************************************************************/
.even

/*********************************************************************
**  [Equates]
**********************************************************************/

*************************************************
**		[VDP]
*************************************************
.equ	vdp_data_port,		0xC00000
.equ	vdp_control_port,	0xC00004
.equ 	vdp_counter,		0xC00008



/*********************************************************************
**  [Constants]
**********************************************************************/
.even
Pal0:
.word	0x0000
.word	0x00CA
.word	0x00CA
.word	0x0008
.word	0x00C0
.word	0x0D00
.word	0x001F
.word	0x00DB
.word	0x0000
.word	0x0000
.word	0x0000
.word	0x0000
.word	0x0000
.word	0x0000
.word	0x0000
.word	0x0000

/*********************************************************************
**  [Short-Macros]
**********************************************************************/
.even

/*********************************************************************
**  [Macros]
**********************************************************************/
.even


/*********************************************************************
**  [Variables]
**********************************************************************/
.even

/*********************************************************************
**  [Entry]
**********************************************************************/

.global	EntryStart
.even
EntryStart:
	move.W	#0x8F02,(vdp_control_port).L			/*	Set auto-increment to 2 Bytes.*/
	lea	Pal0,a0
	move.l	#0x0,d0
	jsr	VDP_WritePalCRAM
**	move.W	#0x8703,(vdp_control_port).L			/*	Set background colour.*/
        jsr PalColCycle
	jsr	InfinNop

/*********************************************************************
**  [Routines]
**********************************************************************/

.even
InfinNop:
	nop
	bra	InfinNop
	rts


.even
VDP_WritePalCRAM:
VDP_WritePalCRAM_Init:
	movem.L	d1-d7/a1-a6,-(sp)			/*	Save the Regs.						*/
	move.L	#0x0,a1
VDP_WritePalCRAM_Exec:
	move.W	#0x8F02,vdp_control_port		/*	Auto-Increment =2B.TODO: shift it.			*/
/* Determine which Palette Slot to write to in CRAM:									*/
	lsl.L	#8,d0
	lsl.L	#8,d0
	lsl.L	#4,d0
	add.L	#0xC0000000,d0
	move.L	d0,(vdp_control_port).L			/*	Set the Palette to write to in CRAM.			*/
	move.L	a0,a1
	lea	(a1),a0					/*	Get Address of the Palette-Data.			*/
	move.L	#0x7,d2
VDP_WritePalCRAM_Loop:
	move.L	(a0)+,(vdp_data_port).L
	dbra	d2,VDP_WritePalCRAM_Loop
VDP_WritePalCRAM_Exit:
	movem.L	(sp)+,d1-d7/a1-a6			/*	Restore the Regs.					*/
	rts


.even
PalColCycle:
PalColCycle_Init:
	movem.L	d0-d7/a1-a6,-(sp)			/*	Save the Regs.						*/
PalColCycle_InitLoop:
        move.W 0,d0
        move.W #0,d0
        move.W #0,d2
        move.W #0x8,d2
PalColCycle_Loop:
        move.W 0,d1
        addi.W #1,d0
        move.W #0x8700,d1
        add.W d0,d1
**move.W	#0x8703,(vdp_control_port).L			/*	Set background colour.*/
        move.W	d1,(vdp_control_port).L			/*	Set background colour.*/

        jsr BigLoop
        
        jsr BigLoop

        dbra d2,PalColCycle_Loop
        bra PalColCycle_InitLoop
PalColCycle_Exit:
	movem.L	(sp)+,d0-d7/a1-a6			/*	Restore the Regs.					*/
        rts


.even
BigLoop:
BigLoop_Init:
	movem.L	d0-d7/a1-a6,-(sp)			/*	Save the Regs.						*/
BigLoop_NOPLoop_Init1:
        move.L 0,d3
        move.L #0xFF000000,d3
BigLoop_NOPLoop1:
        nop
        dbra d3,BigLoop_NOPLoop1
BigLoop_NOPLoop_Init2:
        move.L 0,d3
        move.L #0xFF000000,d3
BigLoop_NOPLoop2:
        nop
        dbra d3,BigLoop_NOPLoop2
BigLoop_NOPLoop_Init3:
        move.L 0,d3
        move.L #0xFF000000,d3
BigLoop_NOPLoop3:
        nop
        dbra d3,BigLoop_NOPLoop3
BigLoop_NOPLoop_Init4:
        move.L 0,d3
        move.L #0xFF000000,d3
BigLoop_NOPLoop4:
        nop
        dbra d3,BigLoop_NOPLoop4
BigLoop_Exit:
	movem.L	(sp)+,d0-d7/a1-a6			/*	Restore the Regs.					*/
        rts
.end
