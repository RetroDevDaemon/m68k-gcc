;;;;;;;;;;;;;;;;;
;;
;; Z80 VGM Player
;; 
;; v 0.21
; WARNING: 68k side addresses changed from a00080+ to a00100+
; Set to NTSC 
; Samples currently OFF while debugging

; by RetroDevDiscord

; Special thanks to: 
;  Stef 
;  vytah

;;; TO USE: 
; Set byte a00100 to 1 every vblank to play!
; read byte a00101 to see if song is playing or not.
; Set SongBaseAddress (102h-105h) to full 32bit address 
; and RomBank (106h) to bit 15+ of the 68000 memory bank
; If PAL, set the PAL flag to 1 (108h), otherwise, 0=NTSC

; TODOS:
; - Tie z80 to vblank interrupt so it isn't driven by 68000
; - Add error checking for vgm 1.60
; - weirder bytecodes 
; - multiple sample streams 
; - WIP: multiple data blocks

;;;;;
; FIXMEs
; - copy and paste CurDac stuff within dac loop 
; - dac block transfer size needs adjusting / delay based on sample width!


FMREG0 EQU $4000
FMDAT0 EQU $4001
FMREG1 EQU $4002
FMDAT1 EQU $4003
BANKREG EQU $6000
PSGREG EQU $7F11


VGM_START EQU $00 
VGM_END EQU $04       ;*: ; 2004-2005
SSGCLOCK EQU $0C      ; : ; 200c-200f
SAMPLECOUNT EQU $18   ;*:  ; 2018-201b 0X22740
LOOPOFFSET EQU $1c    ;*: 201c-201f
LOOPNUMS EQU $20      ;*:   ; 2020-2023
RATE EQU $24          ;:   ; 2024-2027 = 60
SNFB EQU $28          ;*?:  ; 2028-9 = 0009
SNW EQU $2a           ;*?; 202a = 16
SF EQU $2b            ;*:    ; 202b ; ssg flags all 0
YM2612CLK EQU $2C     ;: ; 202c-202f = 0x750ab6 
VGMDATOFS EQU $34     ;*:  ; 2034-2037
;SPCMCLK EQU $2038       ;:    ; 2038-203b
;SPCMI EQU $203C         ;:   ; 203c-203f
VM EQU $7C            ;: volume modifier (default 0) ; 207c
LB EQU $7E            ;: loop base; 207e
LM EQU $7F            ;: loop modifier; 207f
        
        org $0

; disable interrupts
        di 

; clear the stack
        ld a, 0
        ld de, $1c00 
        ld b, 0 
_clrstack:
        LD (DE),A
        inc de 
        djnz _clrstack
_clr2
        ld (de),a 
        inc de 
        djnz _clr2
_clr3 
        ld (de),a 
        inc de 
        djnz _clr3

; set the stack pointer 
        ld sp,$1d80 

; clear and go!
        call CLEARPSG 
        call BANKSWAP

        jp LOADVGM 
        
; +8 and +9 should contain VGM version number.
;VERSIONCHECK:
;        ld hl, ($2008) 
;        cp h, $60
;        jr nz,WRONGVER
;        cp l, $01
;        jr nz,WRONGVER 
;        jp LOADVGM
;WRONGVER:
;        jp WRONGVER

CLRTB:
        defb $9f,$bf,$df,$ff
LoopPlay: 
        defb 0
LoopLoc:        
        defb 0,0,0,0

WORKRAM: defb 0,0,0,0

SongDataStart: defb 0,0,0,0


; sub 50h then times 2
CMDJUMPTABLE: ; starts at 50h
        defw WRITEPSG, 0, WRITEFM1, WRITEFM2, 0, 0, 0, 0 ; 50-57
        DEFW 0, 0, 0, 0, 0, 0, 0, 0 ;   58-5F
        defw 0, SAMPLEWAIT, FRAMEOVER2, FRAMEOVER2, 0, 0, SONGOVER, DATABLOCK ; 60-67
        DEFW WRITEPCM, 0, 0, 0, 0, 0, 0, 0
        DEFW QWAIT, QWAIT, QWAIT, QWAIT, QWAIT, QWAIT, QWAIT, QWAIT ;70
        DEFW QWAIT, QWAIT, QWAIT, QWAIT, QWAIT, QWAIT, QWAIT, QWAIT ;7F   
        DEFW QFMWAIT, QFMWAIT, QFMWAIT, QFMWAIT, QFMWAIT, QFMWAIT, QFMWAIT, QFMWAIT
        DEFW QFMWAIT, QFMWAIT, QFMWAIT, QFMWAIT, QFMWAIT, QFMWAIT, QFMWAIT, QFMWAIT ;..8f
        DEFW DACSETUP, SETDACSTREAM, SETDACFREQ, STARTDACSTREAM, STOPDAC, DACFASTCALL ; 90-95
        DEFW ERROR  ; 96

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        org $100

; Vars
PlayNext:
        defb 0          ; 100h
SongPlaying: 
        defb 0          ; 101h
SongBaseAddress:        ; Set me when loading the song from the 68000!
        defb $0, $0, 0, 0 ; 102-105h
RomBank:
        defb 0          ; 106h
StartBank:
        defb 0          ; --
; This is 0 if NTSC, and 1 if PAL.
PALFlag: defb 0         ; 108h
; Set this to 1 and the next frame the Z80 will reset (jp 0). 
; Do this after changing SongBaseAddress to change songs. 
ResetFlag: defb 0       ; 109h
 

LOADVGM:
;;;;;;;;;;;;;;;;
        
        LD A,1 
        LD (SongPlaying),A      ; set playing var to ON
        LD A,(RomBank) 
        LD (StartBank),A        ; Store the initial ROM bank for looping.
        
; Set the memory window to the song header (+1c)      
        ld hl,(SongBaseAddress)
        ld bc,$001c 
        xor a 
        adc hl,bc 
        ld (ZadrWork),hl 
        ld hl,(SongBaseAddress+2)
        ld bc,$0000
        adc hl,bc               ; catch the carry
        ld (ZadrWork+2),hl
        call SETZADDRESS        ; set bank to song base address + 1c!

; Is the loop variable = 00 00 ?
        ld a,(hl)               ; pointer is at loop portion of header. 
        ld c,a 
        call GetNextSongByte    ; HL++
        ld a,(hl)               ; Is there loop location?
        or c  
        jr z, noloop            ; checks first two bytes only~~~
        ld a,(hl)
        ld b,a                  ; 1c and 1d are in bc
; store the 32bit 68000 address at LoopLoc
        push hl 
         push bc 
         pop hl 
         ld (LoopLoc),hl ; store 1c-1d here
        pop hl          ; at LoopLoc
        call GetNextSongByte 
        ld a,(hl) 
        ld c,a 
        CALL GetNextSongByte
        ld a,(hl) 
        ld b,a 
        push bc         ; excg
        pop hl          ; bc and hl
        ld (LoopLoc+2),hl  ; and at LoopLoc+2
        ; WE AREN'T DONE!!!
; we need to convert this to the 68000 range.;
        ld hl,(SongBaseAddress) 
        push hl 
        pop bc                  ; HL = LoopLoc+0, BC=SongBaseAddress+0 
        ld hl,(LoopLoc) 
        xor a 
        adc hl,bc               ; if it overflows, its in the carry...
        ld (LoopLoc),hl         ; lower byte stored
        ld hl,(LoopLoc+2)       ; load upper byte and store carry
        ld bc,$0000
        adc hl,bc               ; <- carry taken care of. 
        ld (LoopLoc+2),hl 
        ; HL = LoopLoc+2 and carry
        push hl 
         ld hl,(SongBaseAddress+2) 
         push hl 
         pop bc 
        pop hl    ; HL = LoopLoc+2+c, BC = SongBaseAddress+2
        xor a     ; clear carry for the upper bytes 
        adc hl,bc               ; phew! NOW we're good 
        ld (LoopLoc+2),hl 
; Set start playing flag 
        ld a,1 
        ld (LoopPlay),a           ; set LoopPlay to 1
noloop:
; Now set HL to song data pointer - longword at 0x0034 into the VGM header 
        ld hl,(SongBaseAddress)    ; 00001ef7, need to add 83 to skip header and
                                   ; byte size
        ld bc, $0080      ; skip the header
        xor a 
        adc hl,bc  ; 0x100              ; 1f2b 1f2e is wrong! should be 1f7a
        ld (ZadrWork),hl           ; 16bit addition ; START BAD CODE
        ld bc,$0000
        ld hl,(SongBaseAddress+2)
        adc hl,bc ; catch the carry 
        ld (ZadrWork+2),hl              ; < ok
        call SETZADDRESS   
; TODO FIXME i am not reading addr from header right now     
; This means only 128-byte header VGMs (1.60 will work!)
        ; hl now contains base+34 or data offset start
; Get the 68000 address and store it in SongDataStart (our sram var) 
        ld a,(hl) ; data start offset bits 0-7. add this to ;

        dec hl  ;) < go down one because we call GetNextSongByte below
PLAYLOOP:
;;;;;;;;;;;;;;;;
        call GetNextSongByte
        LD A,(HL)
;BREAKPOINT: JP BREAKPOINT  
        ; sub 50h, times 2 + &CMDJUMPTABLE
        ld b,$50
        sub b ; a-50
        sla a   ; *2
        push hl 
         ld hl,CMDJUMPTABLE
         add a,l   
         ld l,a  ; jump table is < 256 so no high byte
         ld a,(HL) 
         ld c,a 
         inc hl 
         ld a,(HL) ; command address in bc 
         ld b,a  
         push bc 
         pop hl     ; to hl (dont forget to pop hl) 
         jp (hl) 

        ;cp $50 
        ; jp c,ERROR      ; 0-49f - ERROR, invalid control byte
        ;JP z,WRITEPSG   ; 50f - write PSG
        ;cp $52          ; 0x52 a b - 2612 port 0 (gen FM1) <-
        ;JP z,WRITEFM1
        ;cp $53          ; 0x53 a b - 2612 port 1 (gen FM2) <-
        ;JP z,WRITEFM2
        ;CP $61          ; 0X61 a b - wait ba samples
        ;JP Z,SAMPLEWAIT 
        ;CP $62          ; 0x62 wait 1/60 second
        ;JP Z,FRAMEOVER  ; (until next vblank)
        ;CP $63
        ;JP Z,FRAMEOVER  ; Same frame wait code won't make a difference on PAL
        ;CP $66          
        ; JP C,ERROR      ; 64-65 error 
        ;JP Z,SONGOVER   ; 0x66 stop music playback
        ;CP $67          ; DATA BLOCK 
        ;JP Z,DATABLOCK  ;  sets up all pointers
        ;cp $68 
        ;JP Z,WRITEPCM   ;  probably not used 
       ; CP $80
       ; JP C,QWAIT        ; 69-7f (not 7c) quick wait
       ; CP $90 
       ; JP C,QFMWAIT      ; QFMWAIT      ; 80-8f quick fm write/wait
       ; jp z,DACSETUP     ; 90 setup dac stream
       ; CP $91 
       ; JP Z,SETDACSTREAM
       ; CP $92 
       ; JP Z,SETDACFREQ
       ; CP $93 
       ; JP Z,STARTDACSTREAM ; UNSUPPORTED FIXME
       ; CP $94 
       ; JP Z,STOPDAC
       ; CP $95
       ; JP Z,DACFASTCALL
       ; CP $FF 
       ;  JP C,ERROR     ; 96-fe goto ERROR
FRAMEOVER2:
        pop hl ; jp table
FRAMEOVER:
;;;;;;;;;;;
; * A
; Waits until next vblank signal (done via 68k)
        ;JP FRAMEOVER 
        ; Before checking 68k vbl, we need to see if DAC are queued.
        ;LD A,(DacTransferActive) 
        ;cp 0 
        ;call DACBLOCKTRANSFER ; MAD TESTING BRUH
_framewait:
        LD A,(PlayNext)  ; check the 'play next frame' var
        CP 1             ; is it = 1?
        JR NZ,_framewait  ; if not, loop.
        xor a            ; if it is, clear it.
        ld (PlayNext),a 
        JP PLAYLOOP
;;;; 


;;;;;;;;;;;;;;;;;;;
ERRDEFB: DEFB $99
; This error handling is super rudimentary and only intended to get songs to
;  play without crashing!
ERROR:  
        ;pop hl  ; jp table
        ;cp $28    ; probably just got lost 
        ;jr _fixsongptr
        jp ERROR 

_fixsongptr: 
        call GetNextSongByte
        jp PLAYLOOP 

_songoverb: defb $55
SONGOVER:
;;;;;;;;;;;
; * A
; Stops the song when hits command byte 0x66
;  Or, if looping, jumps to loop
        pop hl 
        ld a,(LoopPlay) 
        cp 0 
        jr z, _endsong  
; LOOP PLAY:
        ; LoopLoc should be all set...
        ld hl,(LoopLoc) 
        ld (ZadrWork),hl 
        ld hl,(LoopLoc+2) 
        ld (ZadrWork+2),hl 
        call SETZADDRESS 
        jp PLAYLOOP
_endsong:
        XOR A 
        LD (SongPlaying),A
        call CLEARPSG
        ; TODO in case the VGM doesn't clear the FM stuff, do that here
_overloop
        JP _overloop


GetNextSongByte:
        inc hl 
        ld a,h 
        and $80 
        call z,BANKUP ; this also sets HL to 8000
        ret 


VOLUMESET:
        ;JP PLAYLOOP  
        JP VOLUMESET 
        

aaa: defb $11
WRITEPCM:
        ;jp PLAYLOOP  
        JP WRITEPCM 
bbb: defb $22
; Waits 1-16 samples
QWAIT:
        pop hl 
        ; TOO SHORT TO NOTICE?
        and $0f 
        ld b,a 
        ld c,0 
_qwl:   ; idk what to do other than skip 256*n cycles
        dec bc 
        ld a,c 
        or b 
        jr nz, _qwl
        ;JP QWAIT 
        JP PLAYLOOP 
ccc: defb $33
QFMWAIT:
        ;jp PLAYLOOP 
        JP QFMWAIT 

ZadrWork: defb 0,0,0,0

SETZADDRESS:
;;;;;;;;;;;;;;;;;;
; * A H L 
; Input: ZadrWork 32-bit address (68000 memory)
; Output: changes active rom bank, and sets HL to mem addr
        ld hl,(ZadrWork+1) ; ignore bits 0-7, so we have 8-23
        xor a ; clc 
        srl h
        rr l   ; hl >> 1
        srl h
        rr l   ; hl >> 2
        srl h
        rr l   ; hl >> 3
        srl h
        rr l   ; hl >> 4
        srl h
        rr l   ; hl >> 5
        srl h
        rr l   ; hl >> 6
        srl h
        rr l   ; hl >> 7 
        ld a,l 
        ld (RomBank),a ; BITS 15-22
        call BANKSWAP
        LD HL,(ZadrWork)  ; 0-15 
        ld a,$7f 
        and h 
        or $80 ; always set top bit so we look at $8000 + 
        ld h,a
        push hl 
        ld hl,$0000 
        ld (ZadrWork),hl 
        ld (ZadrWork+2),hl 
        pop hl 
        ; [HL] now contains memory pointer!
        ret
;;;;

GETROMADDRESS: 
;;;;;;;;;;;;;;
; INPUT: ZadrWork+0 BANK
;        ZadrWork+2~ZadrWork+3 HL
; (bank * 8000) + HL & 7fff
; OUTPUT: ZadrWork ~ ZadrWork+3 CONTAINS LONGWORD ADDRESS
; * A
; Preserves HL
        push hl
        push bc  
         xor a ; clc 
         ld a,(ZadrWork) ; contains RomBank, A15-A22
         ; 15:
         and $1 
         rr a   ; 15 > carry 
         rr a   ; carry > 7, carry = 0
         ld b,a ; contains bit 15 ; SAVE
         ld a,(ZadrWork) ; 15-22...
         and $fe
         srl a    ; and the last bit and shift RIGHT, so we have 16-23 (minus23)
         ld hl,(ZadrWork+2) 
         ld (ZadrWork+2),a ; put it in the 3rd byte 
         xor a 
         ld (ZadrWork),a 
         ld (ZadrWork+1),a ; zero the first two
         
         ld a,$7f 
         and h ; hl = bits 0-14, dont forget 15 in b!
         or b           ; d(>.< )
         ld h,a
         ld (ZadrWork),hl 
         xor a 
         ld (ZadrWork+3),a ; 24-31 are always 0
        pop bc 
        pop hl 
        RET 

DacWRAM: defb 0, 0
DACWORK
        defb 0,0,0,0
; Block struct:
;Type (ff means empty) 1
; Compatible types:
; 0: YM2612 PCM data 
;Size 4
; 32 bit size (from VGM file)
;Loc 2 (HL offset of data start)
;Bank 1 (bank of data start)
;Counter 4 (32bit offset remaining to play of this loop)
;BlockActive: 1 if we need to dac stream it, 0 skips
;_buffer 3 : to align to 16 bytes

DACBLOCKTRANSFER:
        ;ret 

;; Step through me again
        ; PRESERVE CURRENT ROM BANK AND SONG PTR IN WORKRAM~+3
        ld a,(RomBank)          
        ld (DACWORK),a 
        xor a
        ld (DACWORK+1),a     ; saving me for later!
        ld (DACWORK+2),hl       
        ; LOOP THROUGH DATA STRUCT[], SEARCH FOR ACTIVE BYTE == 1
;        ld hl,BlockActive-16
;        ld b,16   ; 16 data arrays
;        ld de,1  ; 16 bytes each FIXME
;_nextdb
;        OR A 
;        adc hl,de 
;        ld a,(hl) 
;        cp $0
;        jr nz,_found
;        djnz _nextdb
;        cp $0 
;        jp z,_daclpend     ; if all are 0, return
_found
        ; TESTING 
        ld a,(BlockActive) 
        cp 0 
        ret z 
        ; b = 15 to 0 (to flip: 15 - b)
        ;ld hl,15 
        ;ld c,0
        ;or a 
        ;sbc hl,bc 
        ;ld a,l
        ; now [A] has active data bank 0-15
        ; TODO 
        ; GET THE STRUCT'S CTR, STORE IT IN ACTIVEDACCTR

        ; switch to ActiveDacBank|ActiveDacLoc 
        ld a,(ActiveDacBank)     ; = 0
        ld (RomBank),a 
        call BANKSWAP 
        ; output 267 bytes from the bank (TODO DacFrequency)
        ; 1. CHECK ACTIVEDACCTR+2~3 - IF > 0, BC = $10B
        ; 2. COMPARE ADC+0~1 WITH $10B - IF >, BC=$10B
        ;    ELSE BC = ADC+0~1
        ; STORE BC IN DACWRAM
        
        CALL ZWAIT 
        ld a,$2b 
        ld ($4000),a 
        CALL ZWAIT 
        ld a,$80 
        ld ($4001),a 
        LD HL,(ActiveDacCtr+2) 
        LD A,H 
        or L 
        JR NZ,_norm   ; if either of the two high bytes are > 0, normal.
        
        ld bc,(ActiveDacCtr) 
        ld a,(DacFrequency)
        sub c 
        ld a,(DacFrequency+1)
        sbc a,b 
        jr c,_norm 
        ; else, short dac 
        ld bc,(ActiveDacCtr) 
        ld a,c 
        or b
        jp z,_enddac
        jp _nr
_norm:
;BREAKPOINT: JP BREAKPOINT 
        ;ld bc,(DacFrequency) ;; (DacFrequency) ; hopefully 267: debug me
_nr:
        ;LD BC,765
        ld (DacWRAM),bc 
        ld hl,(ActiveDacLoc) ; already -1!
dacwriteloop:
        call ZWAIT 
        ld a,$2a
        ld ($4000),a
        call GetNextSongByte
        call ZWAIT  
        ld a,(hl)
        ld ($4001),a 
        
        dec bc
        ld a,c 
        or b 
        jr nz,dacwriteloop ; a360 bank 0
        ; 3. SUBTRACT DACWRAM FROM ADC (32BIT-16BIT)
        ld a,(RomBank) 
        ld (ActiveDacBank),a ; may be unnecessary 
        ld (ActiveDacLoc),hl 
        
        or a ; clc 
        push hl 
         push bc 
          ld hl,(ActiveDacCtr) 
          ld bc,(DacWRAM) 
          or a  ; clc 
          sbc hl,bc 
          ld (ActiveDacCtr),hl 
          jp z,_enddac 
          jp p,_nouflow
          jp c,_enddac 
          ; else dec adc+2 
          ld hl,(ActiveDacCtr+2) 
          dec hl 
          ld (ActiveDacCtr+2),hl 
        ; 4. IF ADC UNDERFLOWS, END DAC (or check loop if its there)
          jp p,_nouflow ; if < 0, we're done with this. 
_enddac:
          CALL QUICKSTOP
          xor a 
          ld (DacTransferActive),a 
_nouflow: 
         pop bc 
        pop hl 
_daclpend:
        call ZWAIT 
        ld a,$2b 
        ld ($4000),a
        CALL ZWAIT  
        xor a
        ld ($4001),a 
        
        ld a,(DACWORK) 
        ld (RomBank),a 
        call BANKSWAP 
        ld hl,(DACWORK+2) 
        ; dont get next song byte outside the loop.
        ret 
;;;;

QUICKSTOP:
; TODO 
; get the active dac block (this is set inside the transfer each loop)
;  (ActiveDacBlock) is not used anywhere else
; set that block's "active" byte and counter to 0
        pop hl ; * 
        ld a,(ActiveDacBlock)   ; ? 
        push hl 
         push bc 
          call GETBLOCKSTRUCT
          ld bc,12 ; +12 = ACTIVE BYTE 
          or a 
          adc hl,bc 
          xor a 
          ld (hl),a 
          ;ld (DacTransferActive),a 
          ;ld (ActiveDacBank),a 
         pop bc 
        pop hl 
        ;?
        ld (ActiveDacBlock),a 
        ld (ActiveDacCtr),a 
        ld (ActiveDacCtr+1),a 
        ld (ActiveDacCtr+2),a 
        ld (ActiveDacCtr+3),a 
        ld (ActiveDacLoc),a 
        ld (ActiveDacLoc+1),a 
;       block transfer method takes care of turning off DAC mode 
        RET 

STOPDAC:
;;;;;;;;;;;;;;
; Stops DAC and disables DAC mode on CH6
; FIXME broken ?
        pop hl  ;  * 
        call GetNextSongByte ; unused 
        xor a 
        ld (DacTransferActive),a 
        ld (ActiveDacBank),a 
        ld (ActiveDacBlock),a 
        ld (ActiveDacCtr),a 
        ld (ActiveDacCtr+1),a 
        ld (ActiveDacCtr+2),a 
        ld (ActiveDacCtr+3),a 
        ld (ActiveDacLoc),a 
        ld (ActiveDacLoc+1),a 
        ;call ZWAIT 
        ;ld a,$2b 
        ;ld ($4000),a 
        ;call ZWAIT 
        ;xor a 
        ;;ld ($4001),a 
        jp PLAYLOOP  
;;;

StreamID: defb 0
; 90 0 2 0 2a 
; TODO SUPPORT MORE THAN ONE DAC STREAM :)
DACSETUP:
        pop hl 
        ;INC HL  ; STREAM ID 
        call GetNextSongByte ; 0 
        LD A,(HL) 
        LD (StreamID),A 
        ; We only want DAC mode on while transferring, so we leave that to
        ; the block transfer method
        ;INC HL  ; CHIP TYPE 
        call GetNextSongByte ; 2 
        ;LD A,(HL) ; for genesis this should only ever be 02
        ;CP 2
        ;JR NZ,fail
        ; I ignore these two bytes because VGM files appear to have
        ; discrepency over the byte order.
        call GetNextSongByte ; write this byte 
        call GetNextSongByte ; to this reg
        
        jp PLAYLOOP 

SETDACSTREAM:        
; 0X91 <STREAM> <DATABANK ID> <STEP SIZE> <STEP BASE>
;         0           0            1           0 
; Preserves BC 
        pop hl 
       push bc 
        call GetNextSongByte ; Stream ID is for multiple DAC channels.
        ; Genesis only supports one, so we skip it
        call GetNextSongByte ; Databank
        ; Set the active block
        ld a,(hl) 
        ;ld (ActiveDacBlock),a 
        ; always call DAC TRANSFER every frame, but loop it through the 
        ;  data structs instead to find Active Byte = 1
        ;push hl 
         ;call GETBLOCKSTRUCT
         ;or a 
         ;ld bc,12 
         ;adc hl,bc 
         ;push hl 
         ;pop bc 
        ;pop hl   ; HL = song ptr, BC = address of block A active byte 
        ;LD A,1
        ;LD (BC),A 

        call GetNextSongByte ; always 1? (step)
        call GetNextSongByte ; always 0? (step base)
       pop bc 

        jp PLAYLOOP  

_invfreq
        defb $88
INVALIDFREQ: JP INVALIDFREQ 

DacTransferActive: 
        defb 0
DacFrequency:
        defb 0, 0
RealFrequency: 
        defb 0,0,0,0

; 
FreqTable:
        DEFW 134,184,267,368,533
PALFreqTable:
        DEFW 160,221,320,441,640

; $92
SETDACFREQ: ; 7D00 == 32000
; 7D00 = 32000 = 533 N 640P ; 
; 5622 = 22050 = 368 N 441P ; 
; 3E80 = 16000 = 267 N 320P ; 
; 2b11 = 11025 = 184 N 221P
; 1f40 = 8000 =  134 N 160P
        pop hl 
        call GetNextSongByte ; STREAM I
        call GetNextSongByte ; LOW BYTE FREQ 
        ld a,(hl) 
        ld (RealFrequency),a 
        call GetNextSongByte ; 8-15
        ld a,(hl) 
        ld (RealFrequency+1),a
        call GetNextSongByte ; byte 3
        call GetNextSongByte ; byte 4

        push hl 
        push de 

        ld a,(PALFlag)
        cp 1
        jr nz,__2 
        ld hl,PALFreqTable
        jr __1
__2
        ld hl,FreqTable
__1

        ld a,(RealFrequency+1)               ; high byte of real freq is all we care about        
        cp $1f
        jr z,_set8
        cp $2b 
        jr z,_set11
        cp $3e 
        jr z,_set16 
        cp $56 
        jr z,_set22 
        cp $7d 
        jr z,_set32 
        jp INVALIDFREQ 
_set8:
        jr _endfset
_set11:
        or a 
        ld de,2 
        adc hl,de 
        jr _endfset
_set16:
        or a 
        ld de,4 
        adc hl,de 
        jr _endfset
_set22:
        or a 
        ld de,6 
        adc hl,de 
        jr _endfset
_set32:
        or a 
        ld de,8 
        adc hl,de 

_endfset
        ld a,(hl) 
        ld (DacFrequency),a  
        inc hl 
        ld a,(hl) 
        inc a  ; FIXME 
        ld (DacFrequency+1),a 
        
        pop de
        pop hl 
        ;call GetNextSongByte ; (theres an extra 0?)

        jp PLAYLOOP 


;;;;;;;;;;;;;;;;;
STARTDACSTREAM:
;        inc hl  ; stream id 
        ; TODO NOT USED IN MY EXAMPLES
        JR STARTDACSTREAM

GETBLOCKSTRUCT:
;;;;;;;;;;;;;;;;;;
; IN: [A]
; OUT: HL
; PRESERVES BC 
; * A H L 
        push bc 
        ld hl,DataType ; block 0 byte 0
        sla a ; * 2
        sla a ; * 4
        sla a ; * 8 
        sla a ; * 16  ; we are 16 bytes large 
        add a,l   ; carry?
        ld l,a 
        ld a,0 
        ld c,a 
        ld b,0   ; < this is to preserve carry 
        adc hl,bc ; HL now has (DataType) + blockID*8
        pop bc 
        RET 

; 95 00 (0-15) 00
ActiveDacBlock: defb 0  ; which block 0-15 stored below
ActiveDacCtr: defb 0,0,0,0 ; 32bit count
ActiveDacLoc: defb 0,0  ; what is the memory offset
ActiveDacBank: defb 0   ; which 68000 memory bank
LoopDac: defb 0

DACFASTCALL:
;;;;;;;;;;;;;;;;;;;;;;;;
; This should start the DAC playback
; 95 00 
; for data block info:
; data type 1
; datasize 4
; dataloc 2  (hl) 
; databank 1 (rom)
; datacounter 4 (bytes left this loop)
; blockactive 1 (gogogo)
; 0X95
        pop hl 
        call GetNextSongByte ; STREAM ID - ALWAYS 0 FOR NOW
        ;ld a, 1
        ;ld (DacTransferActive),a  ; SET TRANSFER TO ACTIVE SO WE DO IT LOL
        call GetNextSongByte
        ld a,(hl)       ; [A] contains block ID 
        add a,a 
        jr nz, _nope  ; FIXME: ONE STREAM FOR NOW
        ; warning! only have ram for 0-15!
        ld (ActiveDacBlock),a ; low byte 
        push hl 
         call GETBLOCKSTRUCT
         push HL 
         pop bc ; put it in BC
        pop hl ; recover song pointer 
        call GetNextSongByte ; block ID high 8 bits - skip this 
        ;ld a,(bc) ; data type
        INC BC 
        INC BC 
        INC BC 
        INC BC  ; SKIP FOUR SIZE BYTES

        inc bc 
        ld a,(bc) 
;        LD E,4 
;        ADD A,E 
        ld (ActiveDacLoc),a 
        inc bc         
        ld a,(bc)
;        ADC A,0 
        ld (ActiveDacLoc+1),a ; 16bit dac offset ;a1e0

        inc bc 
        ld a,(bc) ; data rom bank #
        ld (ActiveDacBank),a 

        push de 
        inc bc 
        ld a,(bc) ; CTR+0
        ld d,a 
        ld (ActiveDacCtr),a 
        inc bc 
        ld a,(bc) 
        ld (ActiveDacCtr+1),a 
        or d 
        ld d,a 
        inc bc 
        ld a,(bc) 
        ld (ActiveDacCtr+2),a 
        or d 
        ld d,a 
        inc bc 
        ld a,(bc) 
        ld (ActiveDacCtr+3),a ; store 32bit dac size
        or d 
        cp 0 
        jr nz,_active
_inactive 
        inc bc 
        xor a 
        jr _setblockon
_active
        inc bc 
        ld a,1
_setblockon
        ld (bc),a 
        pop de 
        ;INC HL          ; FLAGS 
        call GetNextSongByte
        ld a, (hl)      ; AM I LOOPING?
        ld (LoopDac),a  ; FIXME - NO LOOP FOR NOW
        ; enable!
        jp PLAYLOOP 
_nope: 
        call GetNextSongByte
        call GetNextSongByte
        jp PLAYLOOP

; Data Block struct:
DataType: defb $ff
DataSize: defb 0,0,0,0
DataLoc: defb 0,0
DataBank: defb 0
DataCounter: defb 0,0,0,0 
BlockActive: defb 0
_buffer: defb 0,0,0          ; 16 bytes 
;Type (ff means empty) 1
; Compatible types:
; 0: YM2612 PCM data 
;Size 4
; 32 bit size (from VGM file)
;Loc 2 (HL offset of data start)
;Bank 1 (bank of data start)
;Counter 4 (32bit offset remaining to play of this loop)
;BlockActive: 1 if we need to dac stream it, 0 skips
;_buffer 3 : to align to 16 bytes
Block1: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block2: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block3: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block4: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block5: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block6: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block7: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block8: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block9: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block10: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block11: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block12: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block13: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block14: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0
Block15: defb $ff,0,0,0,0,0,0,0
        defb $0,0,0,0,0,0,0,0

DBWORK: defb 0,0

DATABLOCK:
;;;;;;;;;;;;
;
        pop hl 
      push de 
        call GetNextSongByte ; should be 0x66
        ld a,(hl) 
        ; find an empty block of pointers
        push hl ;9f43 = 66
         ld hl,DataType-16
         ld bc,16      ; 16 BYTES IN SIZE 
_blockfind:
         xor a 
         adc hl,bc 
         ld a,(hl)    ; hl += 16
         cp $ff 
         jr z,_blockok 
         jr _blockfind 
_blockok:         
         push hl 
         pop bc ; now BC has block struct pointer. 
         LD (DBWORK),BC 
        pop hl                  

; FIXME I dont work with empty data blocks.
        call GetNextSongByte
        ld a,(hl) 
        ld (bc),a  ; data type  ; = 0
        call GetNextSongByte
        ; TODO: Check data type
        
        inc bc 
        ld a,(hl)  ; 9f45 = 77
        ld (bc),a ; data size
        ld (WORKRAM),a 
        ;ld d,a
        call GetNextSongByte
        
        ld a,(hl)
        inc bc  
        ld (bc),a ; data size+1 = 0c
        ld (WORKRAM+1),a ; for later
        ;or d 
        ;ld d,a
        call GetNextSongByte
        
        inc bc 
        ld a,(hl) 
        ld (bc),a ; data size+2 = 00
        ld (WORKRAM+2),a 
        ;or d  
        ;ld d,a 
        call GetNextSongByte
        
        inc bc 
        ld a,(hl) 
        ld (bc),a ; data size+3 = 00
        ld (WORKRAM+3),a 
        ;or d
        ;CP 0             ; D contains 0 if all bytes were 0 
        ;jr z,_emptyblock ; so don't use this 
        
        ; now store HL, which is right before data start...
        inc bc 
        ld a,l   ; dataloc l...
        ld (bc),a 
        
        ld a,h   ; and h
        ;and $7f 
        inc bc 
        ld (bc),a ;
        
        ld a,(RomBank) 
        inc bc     ; DataBank
        ld (bc),a ; and store current rom bank, cuz this must be where it starts
        ld (ZadrWork),a 
        xor a 
        ld (ZadrWork+1),a 
        ld (ZadrWork+2),hl ; 0, 0, 9a9f ( = 1a9f)
;Counter 4 (32bit offset remaining to play of this loop)
        ld hl,(WORKRAM) 
        ld a,l 
        inc bc     ; ctr+0
        ld (bc),a 
        ld a,h 
        inc bc 
        ld (bc),a  ; ctr+1 
        ld hl,(WORKRAM+2) 
        ld a,l 
        inc bc   ; ctr+2
        ld (bc),a 
        ld a,h 
        inc bc 
        ld (bc),a  ; ctr+3 
        ; BlockActive starts at 0, we set it to 1 when we hit FASTCALL. 
        ; so ignore the rest.

        ; NEW FUNCTION: Get 68000 address from current bank/HL
        call GETROMADDRESS
        ; ZadrWork = 32bit address of data bank start 
        ; WORKRAM = data size 
        ; add WORKRAM to ZadrWork
        ld hl,(ZadrWork) ; 
        ld bc,(WORKRAM)  ; data size low 16 
        or a 
        adc hl,bc ; check carry 
        ld (ZadrWork),hl ; save low 16
        ld hl,(ZadrWork+2) 
        ld bc,$0000
        adc hl,bc  ; ok  ; carry is cleared here)
        ld bc,(WORKRAM+2) 
        adc hl,bc  ; upper 16
        ld (ZadrWork+2),hl 
        call SETZADDRESS  ; s
        ; now our song pointer and rom window should be
        ; at the byte following the data block -1.
     pop de 
_compressed: ; TODO do extra stuff for compressed data
_uncompressed:

        jp PLAYLOOP     

_emptyblock
        LD A,$FF 
        PUSH HL 
        LD HL,(DBWORK) 
        LD (HL),A 
        POP HL 
        POP DE 
        JP PLAYLOOP 

BANKSWAP:
;;;;;;;;;;;;;
; * A (preserve bc)
; RomBank has the # of the 32kb block to write to 6001.
;  Keep it 8 bits for now FIXME for > 8MB song locations 
        push bc 
        ld c, 1         ; = 0b00000001
        or a 
_bswp2: 
        ld a,(RomBank)
        and c 
        cp 0 
        jr z,_bzero 
        jr _bone
_bzero: 
        ;call ZWAIT 
        xor a 
        jr _bwrt 
_bone:
        ;call ZWAIT 
        ld a,1
_bwrt:
        ld ($6001),a 
        sla c           ; = 0b00000010
        jr nc,_bswp2    ; loop until bit goes into carry

        ;call ZWAIT 
        xor a 
        ld ($6001),a       ; 9 bit = 0 ( works for < 8MB)
        pop bc 
        ret 
;;;;

BANKUP:
;;;;;;;;;;;;;;
; * A H L
; Pops up the rom bank by 1 and 
; sets HL back to $8000
        ld a,(RomBank) 
        inc a 
        ld (RomBank),a 
        call BANKSWAP 
        ld hl,$8000
        ret 
;;;;


WRITEFM1:
;;;;;;;;;;;;;;;;;;;
; HL - location of song byte
; * A  
        pop hl 
        call GetNextSongByte ; register 
        CALL ZWAIT 
        LD A,(HL) 
        LD (FMREG0),A
        call GetNextSongByte ; data byte 
        CALL ZWAIT  
        LD A,(HL) 
        LD (FMDAT0),A 
        JP PLAYLOOP
;;;;

ZWAIT:
;;;;;;;;;;;;;;;;;;
; * A C
        LD A,($4000) 
        ADD A, A 
        JR C,ZWAIT   
        RET 
;;;;

WRITEFM2:
;;;;;;;;;;;;;;;;
; HL = song byte
; * A
        pop hl 
        call GetNextSongByte
        CALL ZWAIT 
        LD A,(HL) 
        LD (FMREG1),A 
        call GetNextSongByte
        CALL ZWAIT 
        LD A,(HL) 
        LD (FMDAT1),A 
        JP PLAYLOOP 
;;;;

WRITEPSG:
;;;;;;;;;;;;;;;;
; HL = song byte 
; * A
        pop hl 
        call GetNextSongByte
        ;CALL ZWAIT 
        LD A,(HL) 
        ld (PSGREG),a 
        JP PLAYLOOP 
;;;;



SAMPLEWAIT:
;;;;;;;;;;;
; * A, B, C
; HL = song byte 
        pop hl 
        call GetNextSongByte
        LD A,(HL)
        LD C,A  
        call GetNextSongByte
        LD A,(HL)
        LD B,A          ; BC = SAMPLE WAIT 16BIT
_sw2 
        ;call DACBLOCKTRANSFER
_sampwt:        
        LD A,(PlayNext)  ; check the 'play next frame' var
        CP 0             ; is it = 1?
        JR Z,_sampwt    ; if not, wait until frame is over
        ;call DACBLOCKTRANSFER
        xor a
        ld (PlayNext),a  ; clear play var
        ; subtract 735 from BC 
        push hl         ; save song pointer
          push bc       ; sample wait value:
          pop hl        ; bc to hl 
         ld a,(PALFlag) 
         cp 1 
         jr nz,_palsize
         ;push hl 
         ; ld hl,267  ;735
         ; ld bc,(DacFrequency) 
         ; sbc hl,bc 
         ; push hl 
         ; pop bc 
         ;pop hl ;  bc = 735-dacfreq
         ld bc,735     ;  267
         jr __3
_palsize 
         ld bc,882      ; PAL
__3 
         sbc hl,bc      ; samplewait - 735 -> HL
         push hl 
         pop bc         ; store it back in bc 
        pop hl          ; recover song pointer
        jr nc,_sw2   ; wait another frame if samplewait is still > 0

        JP PLAYLOOP 
;;;;


CLEARPSG:
;;;;;;;;;;;;;;;;;;;;
; clear the SSG (mute)
; * A, B (preserve HL)
        push hl 
        LD HL,CLRTB 
        ld b,4
_clearlp:
        ld a,(hl) 
        ld (PSGREG),a 
        call GetNextSongByte
        djnz _clearlp
        pop hl 
        RET
;;;;
