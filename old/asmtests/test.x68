* test.x68
**********

	ORG 	$10000
	
sbss 	equ 	$1c000
ebss	equ 	$1c1ff

	move.l 	#sbss,d0
	move.l 	#ebss,d1
	sub.l 	d0,d1		;d1 has size 
	asr.w 	#2,d1		;divide by 4 (longword)
	movea.l	#sbss,a0 	
ClrLoop
	move.l	#$0,(a0)+
	dbra	d1,ClrLoop

	END	$10000

**********************
**	subq   	#1,d1
**	cmp	#-1,d1
**	bne	LOOP
*  is equivalent to 
**	dbra	d1,LOOP
*
