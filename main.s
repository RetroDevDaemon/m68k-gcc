#NO_APP
	.file	"main.c"
	.text
	.align	2
	.globl	_start
	.type	_start, @function
_start:
	link.w %fp,#0
	jsr main
	nop
	unlk %fp
	rts
	.size	_start, .-_start
	.align	2
	.type	WriteVDPRegister, @function
WriteVDPRegister:
	link.w %fp,#0
#APP
| 14 "main.c" 1
	move.w 8(%fp),(0xC00004).l
| 0 "" 2
#NO_APP
	nop
	unlk %fp
	rts
	.size	WriteVDPRegister, .-WriteVDPRegister
	.align	2
	.globl	main
	.type	main, @function
main:
	link.w %fp,#0
	move.l #32768,-(%sp)
	jsr WriteVDPRegister
	addq.l #4,%sp
.L4:
	jra .L4
	.size	main, .-main
	.ident	"GCC: (GNU) 6.3.0"
