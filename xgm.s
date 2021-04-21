#NO_APP
	.file	"xgm.c"
	.text
	.align	2
	.globl	XGM_setPCMFast
	.type	XGM_setPCMFast, @function
XGM_setPCMFast:
	move.l %d2,-(%sp)
	move.l 12(%sp),%d2
	move.l 16(%sp),%d1
	moveq #0,%d0
	move.b 11(%sp),%d0
	move.l %d0,%a0
	add.l #2623232,%a0
	add.l %a0,%a0
	add.l %a0,%a0
	move.l %d2,%d0
	lsr.l #8,%d0
	move.b %d0,(%a0)
	clr.w %d2
	swap %d2
	move.b %d2,1(%a0)
	move.l %d1,%d0
	lsr.l #8,%d0
	move.b %d0,2(%a0)
	clr.w %d1
	swap %d1
	move.b %d1,3(%a0)
	move.l (%sp)+,%d2
	rts
	.size	XGM_setPCMFast, .-XGM_setPCMFast
	.align	2
	.globl	XGM_getManualSync
	.type	XGM_getManualSync, @function
XGM_getManualSync:
	move.w driverFlags,%d0
	and.w #1,%d0
	rts
	.size	XGM_getManualSync, .-XGM_getManualSync
	.align	2
	.globl	XGM_setManualSync
	.type	XGM_setManualSync, @function
XGM_setManualSync:
	move.w 6(%sp),%d0
	cmp.w #5,currentDriver.l
	jne .L3
	tst.w %d0
	jeq .L5
	or.w #1,driverFlags
	move.w VBlankProcess,%d0
	and.w #-9,%d0
.L6:
	move.w %d0,VBlankProcess
.L3:
	rts
.L5:
	and.w #-2,driverFlags
	move.w VBlankProcess,%d0
	or.w #8,%d0
	jra .L6
	.size	XGM_setManualSync, .-XGM_setManualSync
	.align	2
	.globl	XGM_getForceDelayDMA
	.type	XGM_getForceDelayDMA, @function
XGM_getForceDelayDMA:
	move.w driverFlags,%d0
	and.w #2,%d0
	rts
	.size	XGM_getForceDelayDMA, .-XGM_getForceDelayDMA
	.align	2
	.globl	XGM_setForceDelayDMA
	.type	XGM_setForceDelayDMA, @function
XGM_setForceDelayDMA:
	move.w 6(%sp),%d0
	cmp.w #5,currentDriver.l
	jne .L8
	tst.w %d0
	jeq .L10
	or.w #2,driverFlags
	rts
.L10:
	and.w #-3,driverFlags
.L8:
	rts
	.size	XGM_setForceDelayDMA, .-XGM_setForceDelayDMA
	.align	2
	.globl	XGM_getMusicTempo
	.type	XGM_getMusicTempo, @function
XGM_getMusicTempo:
	move.w xgmTempo,%d0
	rts
	.size	XGM_getMusicTempo, .-XGM_getMusicTempo
	.align	2
	.globl	XGM_setMusicTempo
	.type	XGM_setMusicTempo, @function
XGM_setMusicTempo:
	move.w 6(%sp),xgmTempo
	move.w 12582916,%d0
	btst #0,%d0
	jeq .L13
	move.w #50,xgmTempoDef
	rts
.L13:
	move.w #60,xgmTempoDef
	rts
	.size	XGM_setMusicTempo, .-XGM_setMusicTempo
	.align	2
	.globl	XGM_resetLoadCalculation
	.type	XGM_resetLoadCalculation, @function
XGM_resetLoadCalculation:
	moveq #0,%d0
	lea xgmIdleTab,%a1
	lea xgmWaitTab,%a0
.L16:
	clr.w (%a1,%d0.l)
	clr.w (%a0,%d0.l)
	addq.l #2,%d0
	moveq #64,%d1
	cmp.l %d0,%d1
	jne .L16
	clr.w xgmTabInd
	clr.w xgmIdleMean
	clr.w xgmWaitMean
	rts
	.size	XGM_resetLoadCalculation, .-XGM_resetLoadCalculation
	.align	2
	.globl	Z80_isBusTaken
	.type	Z80_isBusTaken, @function
Z80_isBusTaken:
	move.w 10555648,%d0
	lsr.w #8,%d0
	eor.w #1,%d0
	and.w #1,%d0
	rts
	.size	Z80_isBusTaken, .-Z80_isBusTaken
	.align	2
	.globl	XGM_nextXFrame
	.type	XGM_nextXFrame, @function
XGM_nextXFrame:
	move.l %d3,-(%sp)
	move.l %d2,-(%sp)
	move.l 12(%sp),%d2
	cmp.w #5,currentDriver.l
	jne .L19
	jsr Z80_isBusTaken
	move.w #256,%d1
.L24:
	move.w %d1,10555648
	move.w %d1,10555904
.L22:
	move.w 10555648,%d3
	btst #8,%d3
	jne .L22
	move.b 10486034,%d3
	jeq .L23
	move.w #0,10555648
#APP
| 531 "src/xgm.c" 1
			movm.l %d0-%d3,-(%sp)

| 0 "" 2
| 532 "src/xgm.c" 1
			movm.l (%sp)+,%d0-%d3

| 0 "" 2
#NO_APP
	jra .L24
.L23:
	move.l #10486035,%a0
	move.b (%a0),%d1
	add.b %d2,%d1
	move.b %d1,(%a0)
	tst.w %d0
	jne .L19
	move.w #0,10555648
.L19:
	move.l (%sp)+,%d2
	move.l (%sp)+,%d3
	rts
	.size	XGM_nextXFrame, .-XGM_nextXFrame
	.align	2
	.globl	XGM_doVBlankProcess
	.type	XGM_doVBlankProcess, @function
XGM_doVBlankProcess:
	move.l %d3,-(%sp)
	move.l %d2,-(%sp)
	move.w xgmTempoCnt,%d0
	move.w xgmTempoDef,%d1
	clr.w %d2
.L28:
	tst.w %d0
	jle .L29
	sub.w xgmTempo,%d0
	move.w %d0,xgmTempoCnt
	jsr Z80_isBusTaken
	move.w #256,%d1
.L32:
	move.w %d1,10555648
	move.w %d1,10555904
.L30:
	move.w 10555648,%d3
	btst #8,%d3
	jne .L30
	move.b 10486034,%d3
	jeq .L31
	move.w #0,10555648
#APP
| 587 "src/xgm.c" 1
			movm.l %d0-%d3,-(%sp)

| 0 "" 2
| 588 "src/xgm.c" 1
			movm.l (%sp)+,%d0-%d3

| 0 "" 2
#NO_APP
	jra .L32
.L29:
	addq.w #1,%d2
	add.w %d1,%d0
	jra .L28
.L31:
	move.l #10486035,%a0
	move.b (%a0),%d1
	add.b %d2,%d1
	move.b %d1,(%a0)
	tst.w %d0
	jne .L27
	move.w #0,10555648
.L27:
	move.l (%sp)+,%d2
	move.l (%sp)+,%d3
	rts
	.size	XGM_doVBlankProcess, .-XGM_doVBlankProcess
	.align	2
	.globl	Z80_requestBus
	.type	Z80_requestBus, @function
Z80_requestBus:
	move.w 6(%sp),%d0
	move.w #256,10555648
	move.w #256,10555904
	tst.w %d0
	jeq .L35
.L40:
	move.w 10555648,%d0
	btst #8,%d0
	jne .L40
.L35:
	rts
	.size	Z80_requestBus, .-Z80_requestBus
	.align	2
	.globl	Z80_getAndRequestBus
	.type	Z80_getAndRequestBus, @function
Z80_getAndRequestBus:
	move.w 6(%sp),%d0
	move.w 10555648,%d1
	btst #8,%d1
	jeq .L46
	move.w #256,10555648
	move.w #256,10555904
	tst.w %d0
	jeq .L44
.L49:
	move.w 10555648,%d0
	btst #8,%d0
	jne .L49
	clr.w %d0
	rts
.L46:
	moveq #1,%d0
.L44:
	rts
	.size	Z80_getAndRequestBus, .-Z80_getAndRequestBus
	.align	2
	.globl	Z80_releaseBus
	.type	Z80_releaseBus, @function
Z80_releaseBus:
	move.w #0,10555648
	rts
	.size	Z80_releaseBus, .-Z80_releaseBus
	.align	2
	.globl	XGM_set68KBUSProtection
	.type	XGM_set68KBUSProtection, @function
XGM_set68KBUSProtection:
	move.l %d2,-(%sp)
	move.l 8(%sp),%d2
	cmp.w #5,currentDriver.l
	jne .L53
	pea 1.w
	jsr Z80_getAndRequestBus
	move.b %d2,10486033
	addq.l #4,%sp
	tst.w %d0
	jne .L53
	move.l (%sp)+,%d2
	jra Z80_releaseBus
.L53:
	move.l (%sp)+,%d2
	rts
	.size	XGM_set68KBUSProtection, .-XGM_set68KBUSProtection
	.align	2
	.globl	XGM_getElapsed
	.type	XGM_getElapsed, @function
XGM_getElapsed:
	movem.l #14336,-(%sp)
	cmp.w #5,currentDriver.l
	jne .L58
	pea 1.w
	jsr Z80_getAndRequestBus
	move.b 10486164,%d4
	move.b 10486165,%d2
	move.b 10486166,%d3
	addq.l #4,%sp
	tst.w %d0
	jne .L59
	jsr Z80_releaseBus
.L59:
	moveq #0,%d0
	move.b %d2,%d0
	lsl.l #8,%d0
	or.b %d4,%d0
	and.l #255,%d3
	swap %d3
	clr.w %d3
	or.l %d3,%d0
	cmp.l #16777199,%d0
	jls .L57
.L58:
	moveq #0,%d0
.L57:
	movem.l (%sp)+,#28
	rts
	.size	XGM_getElapsed, .-XGM_getElapsed
	.align	2
	.globl	XGM_getCPULoad
	.type	XGM_getCPULoad, @function
XGM_getCPULoad:
	move.l %d3,-(%sp)
	move.l %d2,-(%sp)
	cmp.w #5,currentDriver.l
	jne .L64
	pea 1.w
	jsr Z80_getAndRequestBus
	move.b 10486144,%d1
	move.b 10486145,%d2
	lsl.w #8,%d2
	and.w #255,%d1
	add.w %d1,%d2
	move.b #0,10486144
	move.b #0,10486145
	move.b 10486146,%d1
	move.b 10486147,%d3
	lsl.w #8,%d3
	and.w #255,%d1
	add.w %d1,%d3
	move.b #0,10486146
	move.b #0,10486147
	addq.l #4,%sp
	tst.w %d0
	jne .L63
	jsr Z80_releaseBus
.L63:
	move.w xgmTabInd,%a0
	moveq #0,%d0
	move.w %a0,%d0
	lea xgmIdleTab,%a1
	add.l %d0,%d0
	move.w %d2,%d1
	sub.w (%a1,%d0.l),%d1
	add.w xgmIdleMean,%d1
	move.w %d1,xgmIdleMean
	move.w %d2,(%a1,%d0.l)
	lea xgmWaitTab,%a1
	move.w %d3,%d2
	sub.w (%a1,%d0.l),%d2
	add.w xgmWaitMean,%d2
	move.w %d2,xgmWaitMean
	move.w %d3,(%a1,%d0.l)
	move.w %a0,%d0
	addq.w #1,%d0
	and.w #31,%d0
	move.w %d0,xgmTabInd
	lsr.w #5,%d1
	moveq #105,%d0
	sub.w %d1,%d0
	ext.l %d0
	lsr.w #5,%d2
	swap %d2
	clr.w %d2
	or.l %d2,%d0
.L61:
	move.l (%sp)+,%d2
	move.l (%sp)+,%d3
	rts
.L64:
	moveq #0,%d0
	jra .L61
	.size	XGM_getCPULoad, .-XGM_getCPULoad
	.align	2
	.globl	Z80_startReset
	.type	Z80_startReset, @function
Z80_startReset:
	move.w #0,10555904
	rts
	.size	Z80_startReset, .-Z80_startReset
	.align	2
	.globl	Z80_endReset
	.type	Z80_endReset, @function
Z80_endReset:
	move.w #256,10555904
	rts
	.size	Z80_endReset, .-Z80_endReset
	.align	2
	.globl	Z80_setBank
	.type	Z80_setBank, @function
Z80_setBank:
	move.w 6(%sp),%d1
	moveq #10,%d0
.L68:
	subq.w #1,%d0
	jne .L69
	rts
.L69:
	move.b %d1,10510336
	lsr.w #1,%d1
	jra .L68
	.size	Z80_setBank, .-Z80_setBank
	.align	2
	.globl	Z80_read
	.type	Z80_read, @function
Z80_read:
	moveq #0,%d0
	move.w 6(%sp),%d0
	add.l #10485760,%d0
	move.l %d0,%a0
	move.b (%a0),%d0
	rts
	.size	Z80_read, .-Z80_read
	.align	2
	.globl	Z80_write
	.type	Z80_write, @function
Z80_write:
	moveq #0,%d0
	move.w 6(%sp),%d0
	add.l #10485760,%d0
	move.l %d0,%a0
	move.b 11(%sp),(%a0)
	rts
	.size	Z80_write, .-Z80_write
	.align	2
	.globl	Z80_clear
	.type	Z80_clear, @function
Z80_clear:
	movem.l #14368,-(%sp)
	move.l 20(%sp),%d2
	move.l 24(%sp),%d3
	move.w 30(%sp),%d4
	pea 1.w
	jsr Z80_requestBus
	jsr getZeroU8
	and.l #65535,%d2
	move.l %d2,%a0
	add.l #10485760,%a0
	addq.l #4,%sp
.L73:
	dbra %d3,.L74
	tst.w %d4
	jeq .L75
	jsr Z80_startReset
.L75:
	jsr Z80_releaseBus
	lea Z80_isBusTaken,%a2
.L76:
	jsr (%a2)
	tst.w %d0
	jne .L76
	tst.w %d4
	jeq .L72
	movem.l (%sp)+,#1052
	jra Z80_endReset
.L74:
	move.b %d0,(%a0)
	addq.l #1,%a0
	jra .L73
.L72:
	movem.l (%sp)+,#1052
	rts
	.size	Z80_clear, .-Z80_clear
	.align	2
	.globl	Z80_upload
	.type	Z80_upload, @function
Z80_upload:
	movem.l #14368,-(%sp)
	move.l 24(%sp),%a2
	move.l 28(%sp),%d3
	move.w 22(%sp),%d2
	move.w 34(%sp),%d4
	pea 1.w
	jsr Z80_requestBus
	and.l #65535,%d3
	addq.l #4,%sp
	moveq #0,%d0
	and.l #65535,%d2
	add.l #10485760,%d2
.L83:
	move.l %d2,%a0
	add.l %d0,%a0
	cmp.l %d3,%d0
	jne .L84
	tst.w %d4
	jeq .L85
	jsr Z80_startReset
.L85:
	jsr Z80_releaseBus
	lea Z80_isBusTaken,%a2
.L86:
	jsr (%a2)
	tst.w %d0
	jne .L86
	tst.w %d4
	jeq .L82
	movem.l (%sp)+,#1052
	jra Z80_endReset
.L84:
	move.b (%a2,%d0.l),(%a0)
	addq.l #1,%d0
	jra .L83
.L82:
	movem.l (%sp)+,#1052
	rts
	.size	Z80_upload, .-Z80_upload
	.align	2
	.globl	Z80_download
	.type	Z80_download, @function
Z80_download:
	movem.l #14368,-(%sp)
	move.l 24(%sp),%a2
	move.l 28(%sp),%d3
	move.w 22(%sp),%d2
	pea 1.w
	jsr Z80_getAndRequestBus
	and.l #65535,%d3
	addq.l #4,%sp
	moveq #0,%d1
	and.l #65535,%d2
	add.l #10485760,%d2
.L93:
	move.l %d2,%a0
	add.l %d1,%a0
	cmp.l %d1,%d3
	jne .L94
	tst.w %d0
	jne .L92
	movem.l (%sp)+,#1052
	jra Z80_releaseBus
.L94:
	move.b (%a0),%d4
	move.b %d4,(%a2,%d1.l)
	addq.l #1,%d1
	jra .L93
.L92:
	movem.l (%sp)+,#1052
	rts
	.size	Z80_download, .-Z80_download
	.align	2
	.globl	Z80_getLoadedDriver
	.type	Z80_getLoadedDriver, @function
Z80_getLoadedDriver:
	move.w currentDriver,%d0
	rts
	.size	Z80_getLoadedDriver, .-Z80_getLoadedDriver
	.align	2
	.globl	Z80_unloadDriver
	.type	Z80_unloadDriver, @function
Z80_unloadDriver:
	tst.w currentDriver
	jeq .L97
	pea 1.w
	pea 8192.w
	clr.l -(%sp)
	jsr Z80_clear
	clr.w currentDriver
	move.w VBlankProcess,%d0
	and.w #-9,%d0
	move.w %d0,VBlankProcess
	lea (12,%sp),%sp
.L97:
	rts
	.size	Z80_unloadDriver, .-Z80_unloadDriver
	.align	2
	.globl	Z80_loadCustomDriver
	.type	Z80_loadCustomDriver, @function
Z80_loadCustomDriver:
	move.l %d2,-(%sp)
	move.l 12(%sp),%d2
	clr.l -(%sp)
	pea 8192.w
	clr.l -(%sp)
	jsr Z80_clear
	pea 1.w
	move.w %d2,-(%sp)
	clr.w -(%sp)
	move.l 28(%sp),-(%sp)
	clr.l -(%sp)
	jsr Z80_upload
	move.w #-1,currentDriver
	move.w VBlankProcess,%d0
	and.w #-9,%d0
	move.w %d0,VBlankProcess
	lea (28,%sp),%sp
	move.l (%sp)+,%d2
	rts
	.size	Z80_loadCustomDriver, .-Z80_loadCustomDriver
	.align	2
	.globl	Z80_isDriverReady
	.type	Z80_isDriverReady, @function
Z80_isDriverReady:
	move.l %d2,-(%sp)
	jsr Z80_isBusTaken
	tst.w %d0
	jeq .L101
	move.b 10486018,%d2
	and.b #-128,%d2
.L102:
	move.w %d2,%d0
	and.w #128,%d0
	move.l (%sp)+,%d2
	rts
.L101:
	pea 1.w
	jsr Z80_requestBus
	move.b 10486018,%d2
	and.b #-128,%d2
	jsr Z80_releaseBus
	addq.l #4,%sp
	jra .L102
	.size	Z80_isDriverReady, .-Z80_isDriverReady
	.align	2
	.globl	Z80_loadDriver
	.type	Z80_loadDriver, @function
Z80_loadDriver:
	movem.l #15920,-(%sp)
	move.l 32(%sp),%d5
	move.w %d5,%d2
	move.w 38(%sp),%d6
	cmp.w currentDriver.l,%d5
	jeq .L103
	cmp.w #5,%d5
	jhi .L103
	moveq #0,%d0
	move.w %d5,%d0
	add.l %d0,%d0
	move.w .L106(%pc,%d0.l),%d0
	jmp %pc@(2,%d0:w)
	.balignw 2,0x284c
	.swbeg	&6
.L106:
	.word .L131-.L106
	.word .L107-.L106
	.word .L108-.L106
	.word .L103-.L106
	.word .L109-.L106
	.word .L110-.L106
.L107:
	move.w #2859,%d4
	move.l #z80_drv1,%d3
.L105:
	clr.l -(%sp)
	pea 8192.w
	clr.l -(%sp)
	jsr Z80_clear
	pea 1.w
	move.l %d4,%d0
	and.l #8059,%d0
	move.l %d0,-(%sp)
	move.l %d3,-(%sp)
	clr.l -(%sp)
	lea Z80_upload,%a2
	jsr (%a2)
	lea (28,%sp),%sp
	cmp.w #2,%d2
	jeq .L112
	jhi .L113
	cmp.w #1,%d2
	jeq .L114
.L111:
	tst.w %d6
	jeq .L120
	cmp.w #1,%d2
	jcs .L120
	cmp.w #2,%d2
	jls .L121
	move.w %d5,%d0
	subq.w #4,%d0
	cmp.w #1,%d0
	jls .L121
.L120:
	move.w %d5,currentDriver
	cmp.w #5,%d2
	jne .L128
.L129:
	btst #0,driverFlags+1
	jne .L130
	move.w VBlankProcess,%d0
	or.w #8,%d0
	move.w %d0,VBlankProcess
.L130:
	pea 60.w
	jsr XGM_setMusicTempo
	addq.l #4,%sp
	movem.l (%sp)+,#3196
	jra XGM_resetLoadCalculation
.L108:
	move.w #3584,%d4
	move.l #z80_drv2,%d3
	jra .L105
.L109:
	move.w #2665,%d4
	move.l #z80_drv3,%d3
	jra .L105
.L110:
	move.w #5888,%d4
	move.l #z80_xgm,%d3
	jra .L105
.L131:
	moveq #58,%d4
	move.l #z80_drv0,%d3
	jra .L105
.L113:
	cmp.w #4,%d2
	jeq .L115
	cmp.w #5,%d2
	jne .L111
	jsr YM2612_reset
	jsr PSG_init
	pea 1.w
	jsr Z80_requestBus
	move.l #smp_null,%d0
	move.l %d0,%d1
	lsr.l #8,%d1
	move.b %d1,10492928
	clr.w %d0
	swap %d0
	move.b %d0,10492929
	move.b #1,10492930
	move.b #0,10492931
	jsr Z80_releaseBus
	addq.l #4,%sp
	tst.w %d6
	jne .L121
	move.w #5,currentDriver
	jra .L129
.L112:
	pea 1.w
	jsr Z80_requestBus
	move.l #smp_null_pcm,%d0
	move.l %d0,%d1
	lsr.l #7,%d1
	move.b %d1,10486052
	moveq #15,%d1
	lsr.l %d1,%d0
.L138:
	move.b %d0,10486053
	move.b #1,10486054
	move.b #0,10486055
	jsr Z80_releaseBus
	addq.l #4,%sp
	tst.w %d6
	jeq .L120
.L121:
	jsr Z80_releaseBus
	lea Z80_isBusTaken,%a2
.L124:
	jsr (%a2)
	tst.w %d0
	jne .L124
	lea Z80_isDriverReady,%a2
	lea waitMs,%a3
.L125:
	jsr (%a2)
	tst.w %d0
	jne .L120
	pea 1.w
	jsr (%a3)
	addq.l #4,%sp
	jra .L125
.L114:
	pea 1.w
	jsr Z80_requestBus
	move.l #smp_null,%d0
	move.l %d0,%d1
	lsr.l #8,%d1
	move.b %d1,10486052
	clr.w %d0
	swap %d0
	jra .L138
.L115:
	clr.l -(%sp)
	pea 4096.w
	pea tab_vol
	pea 4096.w
	jsr (%a2)
	pea 1.w
	jsr Z80_requestBus
	move.l #smp_null,%d0
	move.l %d0,%d1
	lsr.l #8,%d1
	move.b %d1,10486056
	clr.w %d0
	swap %d0
	move.b %d0,10486057
	move.b #1,10486058
	move.b #0,10486059
	jsr Z80_releaseBus
	lea (20,%sp),%sp
	tst.w %d6
	jne .L121
	move.w #4,currentDriver
.L128:
	move.w VBlankProcess,%d0
	and.w #-9,%d0
	move.w %d0,VBlankProcess
.L103:
	movem.l (%sp)+,#3196
	rts
	.size	Z80_loadDriver, .-Z80_loadDriver
	.align	2
	.globl	XGM_isPlaying
	.type	XGM_isPlaying, @function
XGM_isPlaying:
	move.l %d3,-(%sp)
	move.l %d2,-(%sp)
	jsr Z80_isBusTaken
	move.w %d0,%d3
	pea 1.w
	pea 5.w
	jsr Z80_loadDriver
	pea 1.w
	jsr Z80_requestBus
	move.b 10486018,%d2
	and.b #64,%d2
	lea (12,%sp),%sp
	tst.w %d3
	jne .L139
	jsr Z80_releaseBus
.L139:
	move.b %d2,%d0
	move.l (%sp)+,%d2
	move.l (%sp)+,%d3
	rts
	.size	XGM_isPlaying, .-XGM_isPlaying
	.align	2
	.globl	XGM_startPlay
	.type	XGM_startPlay, @function
XGM_startPlay:
	lea (-252,%sp),%sp
	movem.l #15904,-(%sp)
	move.l 280(%sp),%a2
	jsr Z80_isBusTaken
	move.w %d0,%d3
	pea 1.w
	pea 5.w
	jsr Z80_loadDriver
	lea (32,%sp),%a1
	move.l %a2,%d4
	add.l #252,%d4
	addq.l #8,%sp
	move.l %a2,%a0
	move.l %a1,%d2
	move.l %a2,%d5
	add.l #256,%d5
	move.l #smp_null,%d6
.L144:
	moveq #0,%d1
	move.b (%a0),%d1
	lsl.l #8,%d1
	moveq #0,%d0
	move.b 1(%a0),%d0
	swap %d0
	clr.w %d0
	or.l %d1,%d0
	cmp.l #16776960,%d0
	jne .L142
	move.l %d6,%d0
.L143:
	move.l %d0,%d1
	lsr.l #8,%d1
	move.b %d1,(%a1)
	clr.w %d0
	swap %d0
	move.b %d0,1(%a1)
	move.b 2(%a0),2(%a1)
	move.b 3(%a0),3(%a1)
	addq.l #4,%a0
	addq.l #4,%a1
	cmp.l %d4,%a0
	jne .L144
	clr.l -(%sp)
	pea 252.w
	move.l %d2,-(%sp)
	pea 7172.w
	jsr Z80_upload
	moveq #0,%d2
	move.b 253(%a2),%d2
	swap %d2
	clr.w %d2
	move.l %a2,%d0
	add.l #260,%d0
	add.l %d2,%d0
	moveq #0,%d2
	move.b 252(%a2),%d2
	lsl.l #8,%d2
	add.l %d0,%d2
	pea 1.w
	jsr Z80_requestBus
	move.b %d2,10486020
	move.l %d2,%d0
	lsr.l #8,%d0
	move.b %d0,10486021
	move.l %d2,%d0
	clr.w %d0
	swap %d0
	move.b %d0,10486022
	clr.w %d2
	swap %d2
	lsr.w #8,%d2
	move.b %d2,10486023
	move.l #10486016,%a0
	move.b (%a0),%d0
	or.b #64,%d0
	move.b %d0,(%a0)
	move.b #0,10486035
	lea (20,%sp),%sp
	tst.w %d3
	jne .L141
	jsr Z80_releaseBus
.L141:
	movem.l (%sp)+,#1148
	lea (252,%sp),%sp
	rts
.L142:
	add.l %d5,%d0
	jra .L143
	.size	XGM_startPlay, .-XGM_startPlay
	.align	2
	.globl	XGM_startPlay_FAR
	.type	XGM_startPlay_FAR, @function
XGM_startPlay_FAR:
	jra XGM_startPlay
	.size	XGM_startPlay_FAR, .-XGM_startPlay_FAR
	.align	2
	.globl	XGM_stopPlay
	.type	XGM_stopPlay, @function
XGM_stopPlay:
	move.l %d2,-(%sp)
	jsr Z80_isBusTaken
	move.w %d0,%d2
	pea 1.w
	pea 5.w
	jsr Z80_loadDriver
	pea 1.w
	jsr Z80_requestBus
	move.l #stop_xgm,%d1
	move.l %d1,%d0
	move.b %d0,10486020
	move.l %d1,%d0
	lsr.l #8,%d0
	move.b %d0,10486021
	move.l %d1,%d0
	clr.w %d0
	swap %d0
	move.b %d0,10486022
	clr.w %d1
	swap %d1
	lsr.w #8,%d1
	move.b %d1,10486023
	move.l #10486016,%a0
	move.b (%a0),%d1
	or.b #64,%d1
	move.b %d1,(%a0)
	move.b #0,10486035
	lea (12,%sp),%sp
	tst.w %d2
	jne .L148
	move.l (%sp)+,%d2
	jra Z80_releaseBus
.L148:
	move.l (%sp)+,%d2
	rts
	.size	XGM_stopPlay, .-XGM_stopPlay
	.align	2
	.globl	XGM_pausePlay
	.type	XGM_pausePlay, @function
XGM_pausePlay:
	move.l %d2,-(%sp)
	jsr Z80_isBusTaken
	move.w %d0,%d2
	pea 1.w
	pea 5.w
	jsr Z80_loadDriver
	pea 1.w
	jsr Z80_requestBus
	move.l #10486016,%a0
	move.b (%a0),%d1
	or.b #16,%d1
	move.b %d1,(%a0)
	lea (12,%sp),%sp
	tst.w %d2
	jne .L150
	move.l (%sp)+,%d2
	jra Z80_releaseBus
.L150:
	move.l (%sp)+,%d2
	rts
	.size	XGM_pausePlay, .-XGM_pausePlay
	.align	2
	.globl	XGM_resumePlay
	.type	XGM_resumePlay, @function
XGM_resumePlay:
	move.l %d2,-(%sp)
	jsr Z80_isBusTaken
	move.w %d0,%d2
	pea 1.w
	pea 5.w
	jsr Z80_loadDriver
	pea 1.w
	jsr Z80_requestBus
	move.l #10486016,%a0
	move.b (%a0),%d1
	or.b #32,%d1
	move.b %d1,(%a0)
	move.b #0,10486035
	lea (12,%sp),%sp
	tst.w %d2
	jne .L152
	move.l (%sp)+,%d2
	jra Z80_releaseBus
.L152:
	move.l (%sp)+,%d2
	rts
	.size	XGM_resumePlay, .-XGM_resumePlay
	.align	2
	.globl	XGM_isPlayingPCM
	.type	XGM_isPlayingPCM, @function
XGM_isPlayingPCM:
	movem.l #14336,-(%sp)
	move.l 16(%sp),%d4
	jsr Z80_isBusTaken
	move.w %d0,%d3
	pea 1.w
	pea 5.w
	jsr Z80_loadDriver
	pea 1.w
	jsr Z80_requestBus
	move.b 10486018,%d2
	and.b %d4,%d2
	lea (12,%sp),%sp
	tst.w %d3
	jne .L154
	jsr Z80_releaseBus
.L154:
	move.b %d2,%d0
	movem.l (%sp)+,#28
	rts
	.size	XGM_isPlayingPCM, .-XGM_isPlayingPCM
	.align	2
	.globl	XGM_setPCM
	.type	XGM_setPCM, @function
XGM_setPCM:
	movem.l #15360,-(%sp)
	move.l 20(%sp),%d3
	move.l 24(%sp),%d4
	move.l 28(%sp),%d5
	jsr Z80_isBusTaken
	move.w %d0,%d2
	pea 1.w
	pea 5.w
	jsr Z80_loadDriver
	pea 1.w
	jsr Z80_requestBus
	move.l %d5,-(%sp)
	move.l %d4,-(%sp)
	moveq #0,%d0
	not.b %d0
	and.l %d3,%d0
	move.l %d0,-(%sp)
	jsr XGM_setPCMFast
	lea (24,%sp),%sp
	tst.w %d2
	jne .L156
	movem.l (%sp)+,#60
	jra Z80_releaseBus
.L156:
	movem.l (%sp)+,#60
	rts
	.size	XGM_setPCM, .-XGM_setPCM
	.align	2
	.globl	XGM_startPlayPCM
	.type	XGM_startPlayPCM, @function
XGM_startPlayPCM:
	movem.l #15360,-(%sp)
	move.l 20(%sp),%d5
	move.l 24(%sp),%d3
	move.l 28(%sp),%d2
	jsr Z80_isBusTaken
	move.w %d0,%d4
	pea 1.w
	pea 5.w
	jsr Z80_loadDriver
	pea 1.w
	jsr Z80_requestBus
	moveq #0,%d1
	move.w %d2,%d1
	move.l %d1,%a0
	add.l #5243012,%a0
	add.l %a0,%a0
	and.b #15,%d3
	move.b %d3,(%a0)
	move.b %d5,1(%a0)
	move.l #10486016,%a0
	move.b (%a0),%d2
	moveq #1,%d3
	lsl.l %d1,%d3
	move.l %d3,%d1
	or.b %d2,%d1
	move.b %d1,(%a0)
	lea (12,%sp),%sp
	tst.w %d4
	jne .L158
	movem.l (%sp)+,#60
	jra Z80_releaseBus
.L158:
	movem.l (%sp)+,#60
	rts
	.size	XGM_startPlayPCM, .-XGM_startPlayPCM
	.align	2
	.globl	XGM_stopPlayPCM
	.type	XGM_stopPlayPCM, @function
XGM_stopPlayPCM:
	movem.l #14336,-(%sp)
	move.l 16(%sp),%d2
	jsr Z80_isBusTaken
	move.w %d0,%d4
	pea 1.w
	pea 5.w
	jsr Z80_loadDriver
	pea 1.w
	jsr Z80_requestBus
	moveq #0,%d1
	move.w %d2,%d1
	move.l %d1,%a0
	add.l #5243012,%a0
	add.l %a0,%a0
	move.b #15,(%a0)
	move.b #0,1(%a0)
	move.l #10486016,%a0
	move.b (%a0),%d2
	moveq #1,%d3
	lsl.l %d1,%d3
	move.l %d3,%d1
	or.b %d2,%d1
	move.b %d1,(%a0)
	lea (12,%sp),%sp
	tst.w %d4
	jne .L160
	movem.l (%sp)+,#28
	jra Z80_releaseBus
.L160:
	movem.l (%sp)+,#28
	rts
	.size	XGM_stopPlayPCM, .-XGM_stopPlayPCM
	.align	2
	.globl	XGM_setLoopNumber
	.type	XGM_setLoopNumber, @function
XGM_setLoopNumber:
	move.l %d3,-(%sp)
	move.l %d2,-(%sp)
	move.l 12(%sp),%d2
	jsr Z80_isBusTaken
	move.w %d0,%d3
	pea 1.w
	pea 5.w
	jsr Z80_loadDriver
	pea 1.w
	jsr Z80_requestBus
	addq.b #1,%d2
	move.b %d2,10486032
	lea (12,%sp),%sp
	tst.w %d3
	jne .L162
	move.l (%sp)+,%d2
	move.l (%sp)+,%d3
	jra Z80_releaseBus
.L162:
	move.l (%sp)+,%d2
	move.l (%sp)+,%d3
	rts
	.size	XGM_setLoopNumber, .-XGM_setLoopNumber
	.align	2
	.globl	Z80_init
	.type	Z80_init, @function
Z80_init:
	pea 1.w
	jsr Z80_requestBus
	clr.l -(%sp)
	jsr Z80_setBank
	move.w #-1,currentDriver
	clr.w driverFlags
	clr.l -(%sp)
	clr.l -(%sp)
	jsr Z80_loadDriver
	lea (16,%sp),%sp
	rts
	.size	Z80_init, .-Z80_init
	.comm	smp_null_pcm,128,1
	.comm	tab_vol,4096,1
	.comm	z80_xgm,5888,1
	.comm	z80_drv3,2665,1
	.comm	z80_drv2,3584,1
	.comm	z80_drv1,2859,1
	.comm	z80_drv0,58,1
	.local	xgmWaitMean
	.comm	xgmWaitMean,2,2
	.local	xgmIdleMean
	.comm	xgmIdleMean,2,2
	.local	xgmTabInd
	.comm	xgmTabInd,2,2
	.local	xgmWaitTab
	.comm	xgmWaitTab,64,2
	.local	xgmIdleTab
	.comm	xgmIdleTab,64,2
	.globl	xgmTempoCnt
	.section	.bss
	.align	2
	.type	xgmTempoCnt, @object
	.size	xgmTempoCnt, 2
xgmTempoCnt:
	.zero	2
	.local	xgmTempoDef
	.comm	xgmTempoDef,2,2
	.local	xgmTempo
	.comm	xgmTempo,2,2
	.comm	driverFlags,2,2
	.comm	currentDriver,2,2
	.comm	VBlankProcess,2,2
	.comm	stop_xgm,172,1
	.comm	smp_null,256,1
	.ident	"GCC: (GNU) 6.3.0"
