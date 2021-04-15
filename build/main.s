#NO_APP
	.file	"main.c"
	.text
	.align	2
	.globl	catch
	.type	catch, @function
catch:
#APP
| 130 "src/bentgen.h" 1
	jsr exceptionDump
| 0 "" 2
#NO_APP
	nop
	rte
	.size	catch, .-catch
	.align	2
	.globl	HBlank
	.type	HBlank, @function
HBlank:
	nop
	rte
	.size	HBlank, .-HBlank
	.align	2
	.globl	VBlank
	.type	VBlank, @function
VBlank:
	movem.l #49344,-(%sp)
	jsr GAME_DRAW
	nop
	movem.l (%sp)+,#771
	rte
	.size	VBlank, .-VBlank
	.align	2
	.globl	_start
	.type	_start, @function
_start:
#APP
| 149 "src/bentgen.h" 1
	move.w #1312,sr
| 0 "" 2
#NO_APP
	jsr main
	nop
	rts
	.size	_start, .-_start
	.align	2
	.globl	SetVRAMWriteAddress
	.type	SetVRAMWriteAddress, @function
SetVRAMWriteAddress:
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
| 156 "src/bentgen.h" 1
	move.l 8(%sp),(0xc00004).l
| 0 "" 2
#NO_APP
	nop
	move.l (%sp)+,%d2
	addq.l #8,%sp
	rts
	.size	SetVRAMWriteAddress, .-SetVRAMWriteAddress
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
	jeq .L11
	moveq #1,%d1
	cmp.l %d0,%d1
	jlt .L12
	tst.l %d0
	jeq .L13
	jra .L10
.L12:
	moveq #2,%d1
	cmp.l %d0,%d1
	jeq .L14
	moveq #3,%d1
	cmp.l %d0,%d1
	jeq .L15
	jra .L10
.L13:
	move.w (%sp),%d0
	moveq #10,%d1
	lsr.w %d1,%d0
	move.w %d0,%d0
	and.l #65535,%d0
	add.l #33280,%d0
	move.l %d0,4(%sp)
	jra .L10
.L11:
	move.w (%sp),%d0
	moveq #13,%d1
	lsr.w %d1,%d0
	move.w %d0,%d0
	and.l #65535,%d0
	add.l #33792,%d0
	move.l %d0,4(%sp)
	jra .L10
.L14:
	move.w (%sp),%d0
	moveq #10,%d1
	lsr.w %d1,%d0
	move.w %d0,%d0
	and.l #65535,%d0
	add.l #33536,%d0
	move.l %d0,4(%sp)
	jra .L10
.L15:
	move.w (%sp),%d0
	moveq #9,%d1
	lsr.w %d1,%d0
	move.w %d0,%d0
	and.l #65535,%d0
	add.l #34048,%d0
	move.l %d0,4(%sp)
	nop
.L10:
#APP
| 178 "src/bentgen.h" 1
	move.w 4(%sp),(0xC00004).l
| 0 "" 2
#NO_APP
	nop
	addq.l #8,%sp
	rts
	.size	SetVDPPlaneAddress, .-SetVDPPlaneAddress
	.align	2
	.globl	LoadPalette
	.type	LoadPalette, @function
LoadPalette:
	move.l 8(%sp),%a0
	move.b 7(%sp),%d0
#APP
| 187 "src/bentgen.h" 1
	move.w #36610,(0xC00004).l
| 0 "" 2
#NO_APP
	cmp.b #1,%d0
	jeq .L18
	jcs .L19
	cmp.b #2,%d0
	jeq .L20
	cmp.b #3,%d0
	jeq .L21
.L25:
	moveq #0,%d0
.L22:
#APP
| 203 "src/bentgen.h" 1
	move.w (%a0,%d0.l),(0xc00000).l
| 0 "" 2
#NO_APP
	addq.l #2,%d0
	moveq #32,%d1
	cmp.l %d0,%d1
	jne .L22
	rts
.L19:
#APP
| 190 "src/bentgen.h" 1
	move.l #-1073741824,(0xc00004).l
| 0 "" 2
#NO_APP
	jra .L25
.L18:
#APP
| 193 "src/bentgen.h" 1
	move.l #-1071644672,(0xc00004).l
| 0 "" 2
#NO_APP
	jra .L25
.L20:
#APP
| 196 "src/bentgen.h" 1
	move.l #-1069547520,(0xc00004).l
| 0 "" 2
#NO_APP
	jra .L25
.L21:
#APP
| 199 "src/bentgen.h" 1
	move.l #-1067450368,(0xc00004).l
| 0 "" 2
#NO_APP
	jra .L25
	.size	LoadPalette, .-LoadPalette
	.section	.rodata
	.align	2
	.type	palette, @object
	.size	palette, 32
palette:
	.word	0
	.word	14
	.word	224
	.word	3584
	.word	0
	.word	3822
	.word	238
	.word	142
	.word	3598
	.word	2056
	.word	1092
	.word	2184
	.word	3808
	.word	10
	.word	1536
	.word	96
	.align	2
	.type	palette2, @object
	.size	palette2, 32
palette2:
	.word	0
	.word	14
	.word	224
	.word	3584
	.word	0
	.word	3822
	.word	238
	.word	142
	.word	3598
	.word	2056
	.word	1092
	.word	2184
	.word	3808
	.word	10
	.word	1536
	.word	96
	.align	2
	.type	palette3, @object
	.size	palette3, 32
palette3:
	.word	0
	.word	14
	.word	224
	.word	3584
	.word	0
	.word	3822
	.word	238
	.word	142
	.word	3598
	.word	2056
	.word	1092
	.word	2184
	.word	3808
	.word	10
	.word	1536
	.word	96
	.align	2
	.type	palette4, @object
	.size	palette4, 32
palette4:
	.word	0
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
	.word	3818
	.word	3822
	.word	3822
	.align	2
	.type	spritetest, @object
	.size	spritetest, 32
spritetest:
	.long	19088743
	.long	305419896
	.long	591751049
	.long	878082202
	.long	1164413355
	.long	1450744508
	.long	1737075661
	.long	2023406814
	.align	2
	.type	tile_0, @object
	.size	tile_0, 32
tile_0:
	.zero	32
	.align	2
	.type	tile_1, @object
	.size	tile_1, 32
tile_1:
	.long	0
	.long	15728640
	.long	15859712
	.long	15859712
	.long	15859712
	.long	131072
	.long	15728640
	.long	131072
	.align	2
	.type	tile_2, @object
	.size	tile_2, 32
tile_2:
	.long	0
	.long	0
	.long	15790080
	.long	15921664
	.long	131584
	.long	0
	.long	0
	.long	0
	.align	2
	.type	tile_3, @object
	.size	tile_3, 32
tile_3:
	.long	0
	.long	15732480
	.long	268435440
	.long	15871778
	.long	15863584
	.long	268435440
	.long	15871778
	.long	131104
	.align	2
	.type	tile_4, @object
	.size	tile_4, 32
tile_4:
	.long	0
	.long	1044480
	.long	15740672
	.long	252702960
	.long	254800626
	.long	15740674
	.long	1044512
	.long	8704
	.align	2
	.type	tile_5, @object
	.size	tile_5, 32
tile_5:
	.long	0
	.long	267387120
	.long	267521826
	.long	2290208
	.long	991744
	.long	15871984
	.long	253890546
	.long	2097186
	.align	2
	.type	tile_6, @object
	.size	tile_6, 32
tile_6:
	.long	0
	.long	983040
	.long	15921152
	.long	991744
	.long	15921392
	.long	253890338
	.long	16773872
	.long	139778
	.align	2
	.type	tile_7, @object
	.size	tile_7, 32
tile_7:
	.long	0
	.long	0
	.long	983040
	.long	15867904
	.long	131072
	.long	0
	.long	0
	.long	0
	.align	2
	.type	tile_8, @object
	.size	tile_8, 32
tile_8:
	.long	0
	.long	3840
	.long	61984
	.long	61952
	.long	61952
	.long	61952
	.long	3840
	.long	32
	.align	2
	.type	tile_9, @object
	.size	tile_9, 32
tile_9:
	.long	0
	.long	15728640
	.long	983040
	.long	991232
	.long	991232
	.long	991232
	.long	15867904
	.long	131072
	.align	2
	.type	tile_10, @object
	.size	tile_10, 32
tile_10:
	.long	0
	.long	0
	.long	15790080
	.long	983552
	.long	268435200
	.long	3088928
	.long	15790080
	.long	131584
	.align	2
	.type	tile_11, @object
	.size	tile_11, 32
tile_11:
	.long	0
	.long	983040
	.long	991232
	.long	268435200
	.long	3088928
	.long	991232
	.long	8192
	.long	0
	.align	2
	.type	tile_12, @object
	.size	tile_12, 32
tile_12:
	.long	0
	.long	0
	.long	0
	.long	0
	.long	15728640
	.long	15728640
	.long	251658240
	.long	2097152
	.align	2
	.type	tile_13, @object
	.size	tile_13, 32
tile_13:
	.long	0
	.long	0
	.long	0
	.long	268435200
	.long	2236960
	.long	0
	.long	0
	.long	0
	.align	2
	.type	tile_14, @object
	.size	tile_14, 32
tile_14:
	.long	0
	.long	0
	.long	0
	.long	0
	.long	16711680
	.long	16719872
	.long	139264
	.long	0
	.align	2
	.type	tile_15, @object
	.size	tile_15, 32
tile_15:
	.long	0
	.long	240
	.long	3874
	.long	61984
	.long	991744
	.long	15867904
	.long	253886464
	.long	2097152
	.align	2
	.type	tile_16, @object
	.size	tile_16, 32
tile_16:
	.long	0
	.long	16776960
	.long	253898736
	.long	253817074
	.long	254739186
	.long	267395314
	.long	16776994
	.long	139808
	.align	2
	.type	tile_17, @object
	.size	tile_17, 32
tile_17:
	.long	0
	.long	1044480
	.long	15921664
	.long	193024
	.long	61952
	.long	61952
	.long	16777200
	.long	139810
	.align	2
	.type	tile_18, @object
	.size	tile_18, 32
tile_18:
	.long	0
	.long	16776960
	.long	253895408
	.long	2097394
	.long	16776994
	.long	253895200
	.long	268435440
	.long	2236962
	.align	2
	.type	tile_19, @object
	.size	tile_19, 32
tile_19:
	.long	0
	.long	16776960
	.long	253895408
	.long	3145506
	.long	8944
	.long	251658482
	.long	16776994
	.long	139808
	.align	2
	.type	tile_20, @object
	.size	tile_20, 32
tile_20:
	.long	0
	.long	61440
	.long	1044992
	.long	15921664
	.long	253948416
	.long	268435440
	.long	2290210
	.long	512
	.align	2
	.type	tile_21, @object
	.size	tile_21, 32
tile_21:
	.long	0
	.long	268435440
	.long	253895202
	.long	268435200
	.long	2237168
	.long	251658482
	.long	16776994
	.long	139808
	.align	2
	.type	tile_22, @object
	.size	tile_22, 32
tile_22:
	.long	0
	.long	16776960
	.long	253895200
	.long	268435200
	.long	253895408
	.long	253755634
	.long	16776994
	.long	139808
	.align	2
	.type	tile_23, @object
	.size	tile_23, 32
tile_23:
	.long	0
	.long	268435440
	.long	2237170
	.long	3874
	.long	61984
	.long	991744
	.long	991232
	.long	8192
	.align	2
	.type	tile_24, @object
	.size	tile_24, 32
tile_24:
	.long	0
	.long	16776960
	.long	253895408
	.long	16776994
	.long	253895408
	.long	253755634
	.long	16776994
	.long	139808
	.align	2
	.type	tile_25, @object
	.size	tile_25, 32
tile_25:
	.long	0
	.long	16776960
	.long	253895408
	.long	253755634
	.long	16777202
	.long	140018
	.long	16776994
	.long	139808
	.align	2
	.type	tile_26, @object
	.size	tile_26, 32
tile_26:
	.long	0
	.long	0
	.long	15728640
	.long	131072
	.long	0
	.long	15728640
	.long	131072
	.long	0
	.align	2
	.type	tile_27, @object
	.size	tile_27, 32
tile_27:
	.long	0
	.long	15728640
	.long	131072
	.long	0
	.long	15728640
	.long	15859712
	.long	253886464
	.long	2097152
	.align	2
	.type	tile_28, @object
	.size	tile_28, 32
tile_28:
	.long	0
	.long	61440
	.long	991744
	.long	15867904
	.long	983040
	.long	61440
	.long	512
	.long	0
	.align	2
	.type	tile_29, @object
	.size	tile_29, 32
tile_29:
	.long	0
	.long	0
	.long	268435200
	.long	2236960
	.long	268435200
	.long	2236960
	.long	0
	.long	0
	.align	2
	.type	tile_30, @object
	.size	tile_30, 32
tile_30:
	.long	0
	.long	983040
	.long	61440
	.long	3840
	.long	61984
	.long	991744
	.long	8192
	.long	0
	.align	2
	.type	tile_31, @object
	.size	tile_31, 32
tile_31:
	.long	0
	.long	16776960
	.long	253895408
	.long	2101026
	.long	61984
	.long	512
	.long	61440
	.long	512
	.align	2
	.type	tile_32, @object
	.size	tile_32, 32
tile_32:
	.long	0
	.long	16776960
	.long	253948656
	.long	254750706
	.long	254803954
	.long	253764130
	.long	16773120
	.long	139776
	.align	2
	.type	tile_33, @object
	.size	tile_33, 32
tile_33:
	.long	0
	.long	16776960
	.long	253895408
	.long	253755634
	.long	268435442
	.long	253895410
	.long	253755634
	.long	2097154
	.align	2
	.type	tile_34, @object
	.size	tile_34, 32
tile_34:
	.long	0
	.long	268435200
	.long	253895408
	.long	268435202
	.long	253895408
	.long	253755634
	.long	268435202
	.long	2236960
	.align	2
	.type	tile_35, @object
	.size	tile_35, 32
tile_35:
	.long	0
	.long	16776960
	.long	251798256
	.long	253755394
	.long	253755392
	.long	253755632
	.long	16776962
	.long	139808
	.align	2
	.type	tile_36, @object
	.size	tile_36, 32
tile_36:
	.long	0
	.long	268431360
	.long	253898496
	.long	253755632
	.long	253755634
	.long	253759234
	.long	268431392
	.long	2236928
	.align	2
	.type	tile_37, @object
	.size	tile_37, 32
tile_37:
	.long	0
	.long	268435440
	.long	253895202
	.long	268435200
	.long	253895200
	.long	253755392
	.long	268435440
	.long	2236962
	.align	2
	.type	tile_38, @object
	.size	tile_38, 32
tile_38:
	.long	0
	.long	268435440
	.long	253895202
	.long	268435200
	.long	253895200
	.long	253755392
	.long	253755392
	.long	2097152
	.align	2
	.type	tile_39, @object
	.size	tile_39, 32
tile_39:
	.long	0
	.long	16776960
	.long	253895408
	.long	253755394
	.long	253820912
	.long	253756146
	.long	16776994
	.long	139808
	.align	2
	.type	tile_40, @object
	.size	tile_40, 32
tile_40:
	.long	0
	.long	251658480
	.long	253755634
	.long	268435442
	.long	253895410
	.long	253755634
	.long	253755634
	.long	2097154
	.align	2
	.type	tile_41, @object
	.size	tile_41, 32
tile_41:
	.long	0
	.long	268435200
	.long	3088928
	.long	991232
	.long	991232
	.long	991232
	.long	268435200
	.long	2236960
	.align	2
	.type	tile_42, @object
	.size	tile_42, 32
tile_42:
	.long	0
	.long	240
	.long	242
	.long	242
	.long	242
	.long	251658482
	.long	16776994
	.long	139808
	.align	2
	.type	tile_43, @object
	.size	tile_43, 32
tile_43:
	.long	0
	.long	251662080
	.long	253817376
	.long	268378624
	.long	253947904
	.long	253759232
	.long	253755632
	.long	2097154
	.align	2
	.type	tile_44, @object
	.size	tile_44, 32
tile_44:
	.long	0
	.long	251658240
	.long	253755392
	.long	253755392
	.long	253755392
	.long	253755392
	.long	268435440
	.long	2236962
	.align	2
	.type	tile_45, @object
	.size	tile_45, 32
tile_45:
	.long	0
	.long	251658480
	.long	267390962
	.long	254800626
	.long	253764338
	.long	253755634
	.long	253755634
	.long	2097154
	.align	2
	.type	tile_46, @object
	.size	tile_46, 32
tile_46:
	.long	0
	.long	251658480
	.long	267387122
	.long	254738674
	.long	253817074
	.long	253759474
	.long	253755634
	.long	2097154
	.align	2
	.type	tile_47, @object
	.size	tile_47, 32
tile_47:
	.long	0
	.long	16776960
	.long	253895408
	.long	253755634
	.long	253755634
	.long	253755634
	.long	16776994
	.long	139808
	.align	2
	.type	tile_48, @object
	.size	tile_48, 32
tile_48:
	.long	0
	.long	268435200
	.long	253895408
	.long	253755634
	.long	268435234
	.long	253895200
	.long	253755392
	.long	2097152
	.align	2
	.type	tile_49, @object
	.size	tile_49, 32
tile_49:
	.long	0
	.long	16776960
	.long	253895408
	.long	253755634
	.long	254738674
	.long	253817074
	.long	16776994
	.long	139808
	.align	2
	.type	tile_50, @object
	.size	tile_50, 32
tile_50:
	.long	0
	.long	268435200
	.long	253895408
	.long	253755634
	.long	268435234
	.long	253898528
	.long	253755632
	.long	2097154
	.align	2
	.type	tile_51, @object
	.size	tile_51, 32
tile_51:
	.long	0
	.long	16776960
	.long	253895200
	.long	16776960
	.long	140016
	.long	251658482
	.long	16776994
	.long	139808
	.align	2
	.type	tile_52, @object
	.size	tile_52, 32
tile_52:
	.long	0
	.long	-16
	.long	36643362
	.long	991232
	.long	991232
	.long	991232
	.long	991232
	.long	8192
	.align	2
	.type	tile_53, @object
	.size	tile_53, 32
tile_53:
	.long	0
	.long	251658480
	.long	253755634
	.long	253755634
	.long	253755634
	.long	253755634
	.long	16776994
	.long	139808
	.align	2
	.type	tile_54, @object
	.size	tile_54, 32
tile_54:
	.long	0
	.long	251658480
	.long	253755634
	.long	253755634
	.long	253755634
	.long	15732514
	.long	1045024
	.long	8704
	.align	2
	.type	tile_55, @object
	.size	tile_55, 32
tile_55:
	.long	0
	.long	251658480
	.long	253755634
	.long	253755634
	.long	253755634
	.long	254800114
	.long	15871778
	.long	131104
	.align	2
	.type	tile_56, @object
	.size	tile_56, 32
tile_56:
	.long	0
	.long	251658480
	.long	15732514
	.long	1045024
	.long	1044992
	.long	15871744
	.long	253886704
	.long	2097154
	.align	2
	.type	tile_57, @object
	.size	tile_57, 32
tile_57:
	.long	0
	.long	-268435216
	.long	251662114
	.long	15790624
	.long	991744
	.long	991232
	.long	991232
	.long	8192
	.align	2
	.type	tile_58, @object
	.size	tile_58, 32
tile_58:
	.long	0
	.long	268435440
	.long	2240290
	.long	61984
	.long	991744
	.long	15867904
	.long	268435440
	.long	2236962
	.align	2
	.type	tile_59, @object
	.size	tile_59, 32
tile_59:
	.long	0
	.long	16776960
	.long	15868448
	.long	15859712
	.long	15859712
	.long	15859712
	.long	16776960
	.long	139808
	.align	2
	.type	tile_60, @object
	.size	tile_60, 32
tile_60:
	.long	0
	.long	251658240
	.long	15728640
	.long	983040
	.long	61440
	.long	3840
	.long	240
	.long	2
	.align	2
	.type	tile_61, @object
	.size	tile_61, 32
tile_61:
	.long	0
	.long	16776960
	.long	143136
	.long	3872
	.long	3872
	.long	3872
	.long	16776992
	.long	139808
	.align	2
	.type	tile_62, @object
	.size	tile_62, 32
tile_62:
	.long	0
	.long	983040
	.long	16773120
	.long	254750464
	.long	3088416
	.long	991232
	.long	991232
	.long	8192
	.align	2
	.type	tile_63, @object
	.size	tile_63, 32
tile_63:
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	-16
	.long	0
	.align	2
	.type	tile_64, @object
	.size	tile_64, 32
tile_64:
	.long	0
	.long	0
	.long	0
	.long	16711920
	.long	251854594
	.long	2097696
	.long	0
	.long	0
	.align	2
	.type	tile_65, @object
	.size	tile_65, 32
tile_65:
	.long	0
	.long	0
	.long	16773120
	.long	143104
	.long	16776992
	.long	253898528
	.long	16776992
	.long	139808
	.align	2
	.type	tile_66, @object
	.size	tile_66, 32
tile_66:
	.long	0
	.long	15728640
	.long	15859712
	.long	16776960
	.long	15868656
	.long	15859954
	.long	16776994
	.long	139808
	.align	2
	.type	tile_67, @object
	.size	tile_67, 32
tile_67:
	.long	0
	.long	0
	.long	1048320
	.long	15868448
	.long	15859712
	.long	15859712
	.long	1048320
	.long	8736
	.align	2
	.type	tile_68, @object
	.size	tile_68, 32
tile_68:
	.long	0
	.long	3840
	.long	3872
	.long	16776992
	.long	253898528
	.long	253759264
	.long	16776992
	.long	139808
	.align	2
	.type	tile_69, @object
	.size	tile_69, 32
tile_69:
	.long	0
	.long	0
	.long	16773120
	.long	253898496
	.long	268431904
	.long	253895168
	.long	16776960
	.long	139808
	.align	2
	.type	tile_70, @object
	.size	tile_70, 32
tile_70:
	.long	0
	.long	65280
	.long	991776
	.long	1044480
	.long	991744
	.long	991232
	.long	991232
	.long	8192
	.align	2
	.type	tile_71, @object
	.size	tile_71, 32
tile_71:
	.long	0
	.long	0
	.long	16776960
	.long	253898528
	.long	251662112
	.long	268435232
	.long	2240288
	.long	268431904
	.align	2
	.type	tile_72, @object
	.size	tile_72, 32
tile_72:
	.long	0
	.long	251658240
	.long	253755392
	.long	268431360
	.long	253898496
	.long	253759264
	.long	253759264
	.long	2097184
	.align	2
	.type	tile_73, @object
	.size	tile_73, 32
tile_73:
	.long	0
	.long	983040
	.long	8192
	.long	16711680
	.long	991232
	.long	991232
	.long	16773120
	.long	139776
	.align	2
	.type	tile_74, @object
	.size	tile_74, 32
tile_74:
	.long	0
	.long	61440
	.long	512
	.long	61440
	.long	61952
	.long	61952
	.long	251720192
	.long	16720384
	.align	2
	.type	tile_75, @object
	.size	tile_75, 32
tile_75:
	.long	0
	.long	15728640
	.long	15921152
	.long	16720384
	.long	16719872
	.long	15921152
	.long	15863552
	.long	131104
	.align	2
	.type	tile_76, @object
	.size	tile_76, 32
tile_76:
	.long	0
	.long	983040
	.long	991232
	.long	991232
	.long	991232
	.long	991232
	.long	65280
	.long	544
	.align	2
	.type	tile_77, @object
	.size	tile_77, 32
tile_77:
	.long	0
	.long	0
	.long	267448320
	.long	254750464
	.long	254750496
	.long	254750496
	.long	254750496
	.long	2105376
	.align	2
	.type	tile_78, @object
	.size	tile_78, 32
tile_78:
	.long	0
	.long	0
	.long	268431360
	.long	253898496
	.long	253759264
	.long	253759264
	.long	253759264
	.long	2097184
	.align	2
	.type	tile_79, @object
	.size	tile_79, 32
tile_79:
	.long	0
	.long	0
	.long	16773120
	.long	253898528
	.long	253759264
	.long	253759264
	.long	16773664
	.long	139776
	.align	2
	.type	tile_80, @object
	.size	tile_80, 32
tile_80:
	.long	0
	.long	0
	.long	16776960
	.long	15868656
	.long	15859954
	.long	16776994
	.long	15868448
	.long	15859712
	.align	2
	.type	tile_81, @object
	.size	tile_81, 32
tile_81:
	.long	0
	.long	0
	.long	16776960
	.long	253898528
	.long	253759264
	.long	16776992
	.long	143136
	.long	4080
	.align	2
	.type	tile_82, @object
	.size	tile_82, 32
tile_82:
	.long	0
	.long	0
	.long	1048320
	.long	15868448
	.long	15859712
	.long	15859712
	.long	15859712
	.long	131072
	.align	2
	.type	tile_83, @object
	.size	tile_83, 32
tile_83:
	.long	0
	.long	0
	.long	16773120
	.long	253895168
	.long	16773120
	.long	143104
	.long	268431904
	.long	2236928
	.align	2
	.type	tile_84, @object
	.size	tile_84, 32
tile_84:
	.long	0
	.long	983040
	.long	16773120
	.long	991744
	.long	991232
	.long	991232
	.long	65280
	.long	544
	.align	2
	.type	tile_85, @object
	.size	tile_85, 32
tile_85:
	.long	0
	.long	0
	.long	251662080
	.long	253759264
	.long	253759264
	.long	253759264
	.long	16773664
	.long	139776
	.align	2
	.type	tile_86, @object
	.size	tile_86, 32
tile_86:
	.long	0
	.long	0
	.long	251662080
	.long	253759264
	.long	15790624
	.long	15921664
	.long	991744
	.long	8192
	.align	2
	.type	tile_87, @object
	.size	tile_87, 32
tile_87:
	.long	0
	.long	0
	.long	251662080
	.long	253759264
	.long	254742304
	.long	254750496
	.long	15921696
	.long	131584
	.align	2
	.type	tile_88, @object
	.size	tile_88, 32
tile_88:
	.long	0
	.long	0
	.long	251662080
	.long	15790624
	.long	991744
	.long	15921152
	.long	253890304
	.long	2097184
	.align	2
	.type	tile_89, @object
	.size	tile_89, 32
tile_89:
	.long	0
	.long	0
	.long	251662080
	.long	253759264
	.long	253759264
	.long	16776992
	.long	143136
	.long	16773664
	.align	2
	.type	tile_90, @object
	.size	tile_90, 32
tile_90:
	.long	0
	.long	0
	.long	268435200
	.long	2290208
	.long	991744
	.long	15867904
	.long	268435200
	.long	2236960
	.align	2
	.type	tile_91, @object
	.size	tile_91, 32
tile_91:
	.zero	32
	.align	2
	.type	tile_92, @object
	.size	tile_92, 32
tile_92:
	.zero	32
	.align	2
	.type	tile_93, @object
	.size	tile_93, 32
tile_93:
	.zero	32
	.align	2
	.type	tile_94, @object
	.size	tile_94, 32
tile_94:
	.zero	32
	.align	2
	.type	tile_95, @object
	.size	tile_95, 32
tile_95:
	.zero	32
	.local	pals
	.comm	pals,16,2
	.comm	fq,1,1
	.comm	frameCounter,4,2
	.comm	vdpstat,2,2
	.comm	hcount,4,2
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	lea (-24,%sp),%sp
	pea palette
	clr.l -(%sp)
	jsr LoadPalette
	addq.l #8,%sp
	pea palette4
	pea 1.w
	jsr LoadPalette
	addq.l #8,%sp
	move.l #palette,pals
	move.l #palette4,pals+4
	move.l pals,%d0
	move.l %d0,-(%sp)
	pea 2.w
	jsr LoadPalette
	addq.l #8,%sp
	move.l pals+4,%d0
	move.l %d0,-(%sp)
	pea 3.w
	jsr LoadPalette
	addq.l #8,%sp
	clr.l 18(%sp)
	pea 1024.w
	jsr SetVRAMWriteAddress
	addq.l #4,%sp
	clr.l 18(%sp)
	jra .L27
.L28:
	move.l #tile_0,14(%sp)
	move.l 18(%sp),%d0
	add.l %d0,%d0
	add.l %d0,%d0
	add.l %d0,14(%sp)
	move.l 14(%sp),%a0
	move.l (%a0),%d0
#APP
| 43 "src/main.c" 1
	move.l %d0,(0xc00000).l
| 0 "" 2
#NO_APP
	addq.l #1,18(%sp)
.L27:
	cmp.l #767,18(%sp)
	jle .L28
	move.l #49152,-(%sp)
	jsr SetVRAMWriteAddress
	addq.l #4,%sp
	lea (2,%sp),%a0
	move.l #1214606444,(%a0)
	addq.l #4,%a0
	move.l #1864390511,(%a0)
	addq.l #4,%a0
	move.l #1919706145,(%a0)
	addq.l #4,%a0
	clr.b 23(%sp)
	jra .L29
.L30:
	moveq #0,%d0
	move.b 23(%sp),%d0
	move.b 2(%sp,%d0.l),%d0
#APP
| 49 "src/main.c" 1
	move.w %d0,(0xc00000).l
| 0 "" 2
#NO_APP
	move.b 23(%sp),%d0
	move.b %d0,%d1
	addq.b #1,%d1
	move.b %d1,23(%sp)
.L29:
	cmp.b #11,23(%sp)
	jls .L30
#APP
| 52 "src/main.c" 1
	move.w #33124,(0xC00004).l
| 0 "" 2
#NO_APP
.L31:
	jra .L31
	.size	main, .-main
	.align	2
	.globl	GAME_DRAW
	.type	GAME_DRAW, @function
GAME_DRAW:
	move.b fq,%d0
	move.b %d0,%d0
	and.l #255,%d0
	or.w #34560,%d0
#APP
| 68 "src/main.c" 1
	move.w %d0,(0xC00004).l
| 0 "" 2
#NO_APP
	move.b fq,%d0
	addq.b #1,%d0
	move.b %d0,fq
	move.b fq,%d0
	cmp.b #63,%d0
	jls .L34
	clr.b fq
.L34:
	nop
	rts
	.size	GAME_DRAW, .-GAME_DRAW
	.ident	"GCC: (GNU) 6.3.0"
