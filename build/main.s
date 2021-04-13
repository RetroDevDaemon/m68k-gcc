#NO_APP
	.file	"main.c"
	.text
	.align	2
	.globl	_start
	.type	_start, @function
_start:
	link.w %fp,#-4
	move.l %d7,-(%sp)
#APP
| 23 "src/bentgen.h" 1
	tst.w 0x00A10008
	bne __resetOK
	tst.w 0x00A1000C
	bne __resetOK
	__resetOK:
| 0 "" 2
| 29 "src/bentgen.h" 1
	move.l #0x00000000, %d0
	move.l #0x00000000, %a0
	move.l #0x00003FFF, %d1
	__Clearram:
	move.l %d0, -(%a0)
	dbra %d1, __Clearram
| 0 "" 2
| 37 "src/bentgen.h" 1
	move.b 0x00A10001, %d0
	andi.b #0x0f, %d0
	beq __Skiptmss
	move.l #0x53454741, 0x00A14000
	__Skiptmss:
| 0 "" 2
| 44 "src/bentgen.h" 1
	move.w #0x0100, 0x00a11100
	move.w #0x0100, 0x00a11200
	__initwait:
	btst #0, 0x00a11100
	bne __initwait
	move.l __Z80Data, %a0
	move.l #0x00a00000, %a1
	move.l #0x29, %d0
	__copyz80init:
	move.b (%a0)+,(%a1)+
	dbra %d0, __copyz80init
	move.w #0, 0x00a11200
	move.w #0, 0x00a11100
	jra __InitPSG
	__Z80Data:
	.word 0xaf01
	.word 0xd91f
	.word 0x1127
	.word 0x0021
	.word 0x2600
	.word 0xf977
	.word 0xedb0
	.word 0xdde1
	.word 0xfde1
	.word 0xed47
	.word 0xed4f
	.word 0xd1e1
	.word 0xf108
	.word 0xd9c1
	.word 0xd1e1
	.word 0xf1f9
	.word 0xf3ed
	.word 0x5636
	.word 0xe9e9
	.word 0x8104
	.word 0x8f01
	
| 0 "" 2
#NO_APP
	move.w #-24641,-4(%fp)
	move.w #-8193,-2(%fp)
	move.l %fp,%d1
	subq.l #4,%d1
#APP
| 73 "src/bentgen.h" 1
	__InitPSG:
	move.l %d1, %a0
	move.l #3, %d0
	__copypsgp: move.b (%a0)+, 0x00c00011
	dbra %d0, __copypsgp
| 0 "" 2
| 80 "src/bentgen.h" 1
	move.l __vdpreginitdata, %a0
	move.l #0x18, %d0
	move.l #0x00008000, %d1
	_vdprcpy: move.b (%a0)+,%d1
	move.w %d1, 0x00c00004
	add.w #0x0100, %d1
	dbra %d0, _vdprcpy
	jra __donevdpinit
	__vdpreginitdata: 
	.byte 4
	.byte 0x74
	.byte (0xc000>>10)
	.byte (0xf000>>10)
	.byte (0xe000>>13)
	.byte (0xd800>>9)
	.byte 0x00
	.byte 0x02
	.byte 0x00
	.byte 0x00
	.byte 0x01
	.byte 0x00
	.byte 0x81
	.byte (0xdc00>>10)
	.byte 0x00
	.byte 0x02
	.byte 0x01
	.byte 0x00
	.byte 0x00
	.byte 0x00
	.byte 0x00
	.byte 0x00
	.byte 0x00
	.byte 0x00
	__donevdpinit:
| 0 "" 2
| 105 "src/bentgen.h" 1
	move.b #0, 0x000a10009
	move.b #0, 0x000a1000b
	move.b #0, 0x000a1000d
| 0 "" 2
| 109 "src/bentgen.h" 1
	move.l #0, %a0
	movem.l (%a0), %d0-%d7/%a1-%a7
	move #0x2700, %SR
	
| 0 "" 2
#NO_APP
	jsr main
	nop
	move.l -8(%fp),%d7
	unlk %fp
	rts
	.size	_start, .-_start
	.globl	VDP_STATUSREG
	.data
	.align	2
	.type	VDP_STATUSREG, @object
	.size	VDP_STATUSREG, 4
VDP_STATUSREG:
	.long	12582916
	.text
	.align	2
	.globl	SetVDPAddress
	.type	SetVDPAddress, @function
SetVDPAddress:
	subq.l #8,%sp
	move.l %d2,-(%sp)
	move.l 16(%sp),%d0
	move.w %d0,6(%sp)
	moveq #0,%d0
	move.w 6(%sp),%d0
	swap %d0
	clr.w %d0
	and.l #1073676288,%d0
	move.l %d0,%d1
	add.l #1073741824,%d1
	move.w 6(%sp),%d0
	moveq #14,%d2
	lsr.w %d2,%d0
	move.w %d0,%d0
	and.l #65535,%d0
	add.l %d1,%d0
	move.l %d0,8(%sp)
#APP
| 176 "src/bentgen.h" 1
	move.l 8(%sp),(0xc00004).l
| 0 "" 2
#NO_APP
	nop
	move.l (%sp)+,%d2
	addq.l #8,%sp
	rts
	.size	SetVDPAddress, .-SetVDPAddress
	.align	2
	.globl	SetVDPPlaneAddress
	.type	SetVDPPlaneAddress, @function
SetVDPPlaneAddress:
	subq.l #8,%sp
	move.l 12(%sp),%d1
	move.l 16(%sp),%d0
	move.b %d1,2(%sp)
	move.w %d0,(%sp)
	clr.l 4(%sp)
	moveq #0,%d0
	move.b 2(%sp),%d0
	moveq #1,%d1
	cmp.l %d0,%d1
	jeq .L5
	moveq #1,%d1
	cmp.l %d0,%d1
	jlt .L6
	tst.l %d0
	jeq .L7
	jra .L4
.L6:
	moveq #2,%d1
	cmp.l %d0,%d1
	jeq .L8
	moveq #3,%d1
	cmp.l %d0,%d1
	jeq .L9
	jra .L4
.L7:
	move.w (%sp),%d0
	moveq #10,%d1
	lsr.w %d1,%d0
	move.w %d0,%d0
	and.l #65535,%d0
	add.l #33280,%d0
	move.l %d0,4(%sp)
	jra .L4
.L5:
	move.w (%sp),%d0
	moveq #13,%d1
	lsr.w %d1,%d0
	move.w %d0,%d0
	and.l #65535,%d0
	add.l #33792,%d0
	move.l %d0,4(%sp)
	jra .L4
.L8:
	move.w (%sp),%d0
	moveq #10,%d1
	lsr.w %d1,%d0
	move.w %d0,%d0
	and.l #65535,%d0
	add.l #33536,%d0
	move.l %d0,4(%sp)
	jra .L4
.L9:
	move.w (%sp),%d0
	moveq #9,%d1
	lsr.w %d1,%d0
	move.w %d0,%d0
	and.l #65535,%d0
	add.l #34048,%d0
	move.l %d0,4(%sp)
	nop
.L4:
#APP
| 199 "src/bentgen.h" 1
	move.w 4(%sp),(0xC00004).l
| 0 "" 2
#NO_APP
	nop
	addq.l #8,%sp
	rts
	.size	SetVDPPlaneAddress, .-SetVDPPlaneAddress
	.comm	frameCounter,4,2
	.comm	vdpstat,2,2
	.comm	hcount,4,2
	.globl	VDP_DATAREG
	.data
	.align	2
	.type	VDP_DATAREG, @object
	.size	VDP_DATAREG, 4
VDP_DATAREG:
	.long	12582912
	.globl	VDP_CTRLREG
	.align	2
	.type	VDP_CTRLREG, @object
	.size	VDP_CTRLREG, 4
VDP_CTRLREG:
	.long	12582916
	.globl	palette
	.section	.rodata
	.align	2
	.type	palette, @object
	.size	palette, 32
palette:
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.word	3822
	.text
	.align	2
	.globl	catch
	.type	catch, @function
catch:
	nop
	rte
	.size	catch, .-catch
	.align	2
	.globl	main
	.type	main, @function
main:
	subq.l #4,%sp
#APP
| 47 "src/main.c" 1
	move.w #36610,(0xC00004).l
| 0 "" 2
| 49 "src/main.c" 1
	move.l #0xf0000000, (0xc00004).l
	lea palette, %a0
	move.l #0x07, %d0
	mvploop: move.l (%a0)+, (0xc00000).l
	dbra %d0, mvploop
| 0 "" 2
| 55 "src/main.c" 1
	move.w #34568,(0xC00004).l
| 0 "" 2
#NO_APP
.L13:
	moveq #0,%d0
	move.b 3(%sp),%d0
	or.w #34560,%d0
#APP
| 59 "src/main.c" 1
	move.w %d0,(0xC00004).l
| 0 "" 2
#NO_APP
	move.b 3(%sp),%d0
	move.b %d0,%d1
	addq.b #1,%d1
	move.b %d1,3(%sp)
#APP
| 61 "src/main.c" 1
	VB181: move.w (0xc00004).l,%d0
	btst #3,%d0
	beq VB181
| 0 "" 2
| 61 "src/main.c" 1
	VB182: move.w (0xc00004).l,%d0
	btst #3,%d0
	bne VB182
| 0 "" 2
#NO_APP
	jra .L13
	.size	main, .-main
	.globl	ENDOFROM
	.section	.bss
	.type	ENDOFROM, @object
	.size	ENDOFROM, 1
ENDOFROM:
	.zero	1
	.ident	"GCC: (GNU) 6.3.0"
