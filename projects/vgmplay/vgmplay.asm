;;;;;;;;;;;;;;;;;
;;
;; Z80 VGM Player
;; 
; (c) Ben Ferguson 2021

;;; TO USE: 
; Set byte a00080 to 1 every vblank to play!
; read byte a00081 to see if song is playing or not.
; Set SongBaseAddress to lower 16bit address 
; and RomBank to bit 15+ of the 68000 memory bank

; TODOS:
; - Tie z80 to vblank interrupt so it isn't driven by 68000
; - Add error checking for vgm 1.60
; - weirder bytecodes 
; - sample support for non-16000hz samples
; - sample fix: < 260 samples will not play

FMREG0 EQU $4000
FMDAT0 EQU $4001
FMREG1 EQU $4002
FMDAT1 EQU $4003
BANKREG EQU $6000
PSGREG EQU $7F11

; (44100 sample rate)
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
        ld de, $1b00 
        ld b, 0 
_clrstack:
        LD (DE),A
        inc de 
        djnz _clrstack

; set the stack pointer 
        ld sp,$1b80 

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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        org $80

; Vars

PlayNext:
        defb 0
SongPlaying: 
        defb 0
SongBaseAddress:        ; Set me when loading the song from the 68000!
        defb $0, $0, 0, 0
RomBank:
        defb 0
StartBank:
        defb 0

;; Entry point
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
        ; hl now contains base+34 or data offset start
; Get the 68000 address and store it in SongDataStart (our sram var) 
        ld a,(hl) ; data start offset bits 0-7. add this to ;

        dec hl  ;) < go down one because we call GetNextSongByte below
PLAYLOOP:
;;;;;;;;;;;;;;;;
        call GetNextSongByte
        LD A,(HL) 
        cp $50 
        ;jp c,ERROR 
        JP z,WRITEPSG 
        cp $52          ; 0x52 a b - 2612 port 0 (gen FM1) <-
        JP z,WRITEFM1
        cp $53          ; 0x53 a b - 2612 port 1 (gen FM2) <-
        JP z,WRITEFM2
        CP $61          ; 0X61 a b - wait ba samples
        JP Z,SAMPLEWAIT 
        CP $62          ; 0x62 wait 1/60 second
        JP Z,FRAMEOVER  
        CP $66          ; 0x66 stop playing
        JP Z,SONGOVER
        CP $67          ; DATA BLOCK 
        JP Z,DATABLOCK
        cp $68 
        JP Z,WRITEPCM   ; 
        CP $7C
        JR Z,VOLUMESET
        CP $80
        JP C,QWAIT
        CP $90 
        JP C,QFMWAIT
        jp z,DACSETUP
        CP $91 
        JP Z,SETDACSTREAM
        CP $92 
        JP Z,SETDACFREQ
        CP $93 
        JP Z,STARTDACSTREAM ; UNSUPPORTED FIXME
        CP $94 
        JP Z,STOPDAC
        CP $95
        JP Z,DACFASTCALL
        ;CP $FF 
        ;JP C,ERROR 
        
FRAMEOVER:
;;;;;;;;;;;
; * A
; Waits until next vblank signal (done via 68k)
        ; Before checking 68k vbl, we need to see if DAC are queued.
        LD A,(DacTransferActive) 
        cp 0 
        call nz,DACBLOCKTRANSFER ; MAD TESTING BRUH
_framewait:
        LD A,(PlayNext)  ; check the 'play next frame' var
        CP 1             ; is it = 1?
        JR NZ,_framewait  ; if not, loop.
        xor a            ; if it is, clear it.
        ld (PlayNext),a 
        JP PLAYLOOP
;;;; 

ERRDEFB: DEFB $99, 0
ERROR:  
        ld (ERRDEFB+1),a 
        jp ERROR 

SONGOVER:
;;;;;;;;;;;
; * A
; Stops the song when hits command byte 0x66
;  Or, if looping, jumps to loop
        ld a,(LoopPlay) 
        cp 1 
        jr nz, _endsong  
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
        JP FRAMEOVER


GetNextSongByte:
        inc hl 
        ld a,h 
        and $80 
        call z,BANKUP ; this also sets HL to 8000
        ret 


VOLUMESET:
        JP PLAYLOOP  
        JP VOLUMESET 
        

aaa: defb $11
WRITEPCM:
        jp PLAYLOOP  
        JP WRITEPCM 
bbb: defb $22
; Waits 1-16 samples
QWAIT:
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
        jp PLAYLOOP 
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
         or b 
         ld h,a
         ld (ZadrWork),hl 
         xor a 
         ld (ZadrWork+3),a ; 24-31 are always 0
        pop bc 
        pop hl 
        RET 

DacWRAM: defb 0, 0
; Block struct:
; 0 - data type
; 01-04 - data size 
; 05-06 - data loc (8000+ offset) <- this is the ptr
; 07 - rom bank < remember to update this along with ptr after frame
DACBLOCKTRANSFER
        ;JR DACBLOCKTRANSFER
        ; WORKRAM+0 to +2 have RomBank and HL
        ld a,(RomBank)          
        ld (WORKRAM),a 
        xor a
        ld (WORKRAM+1),a     ; saving me for later!
        ld (WORKRAM+2),hl       
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
        ; TODO end of sample playback?
        ;push hl 
        ;push bc 
        ld bc,(ActiveDacCtr) 
        ld a,(DacFrequency)
        sub c 
        ld a,(DacFrequency+1)
        sbc a,b 
        jr c,_norm 
        ;ld bc,(ActiveDacCtr) 
        ;jr _nr 
_wasteme:
        ;push hl 
        ;push bc 
        ;ld bc,(ActiveDacCtr) 
        jr _shortdac 
        ;jp _nr
_norm:
        ld bc,(DacFrequency) ;; (DacFrequency) ; hopefully 267: debug me
_nr:
        ld (DacWRAM),bc 
        ld hl,(ActiveDacLoc) ; already -1!
dacwriteloop:
        ;call ZWAIT 
        ld a,$2a
        ld ($4000),a
        call GetNextSongByte
        ;call ZWAIT  
        ld a,(hl)
;BREAKPT: JP BREAKPT 
        ld ($4001),a 
        ; reduce ctr each time and check byte boundary
        ld a,(RomBank) 
        ld (ActiveDacBank),a 
        ld (ActiveDacLoc),hl 
        dec bc
        ld a,c 
        or b 
        jr nz,dacwriteloop 
_shortdac:        
        ; 3. SUBTRACT DACWRAM FROM ADC (32BIT-16BIT)
        or a ; clc 
        push hl 
         push bc 
          ld hl,(ActiveDacCtr) 
          ld bc,(DacWRAM) 
          or a  ; clc 
          sbc hl,bc 
          ld (ActiveDacCtr),hl 
          jp p,_nouflow
          jr c,_enddac 
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
        ;
        call ZWAIT 
        ld a,$2b 
        ld ($4000),a
        CALL ZWAIT  
        xor a
        ld ($4001),a 
        
        ld a,(WORKRAM) 
        ld (RomBank),a 
        call BANKSWAP 
        ld hl,(WORKRAM+2) 
        ; dont get next song byte outside the loop.
        ret 
;;;;

QUICKSTOP:
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
        ;ld ($4001),a 
        RET 

STOPDAC:
;;;;;;;;;;;;;;
; Just stops it
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
        ;ld ($4001),a 
        jp PLAYLOOP  
;;;

StreamID: defb 0
; 90 0 2 0 2a 
; TODO SUPPORT MORE THAN ONE DAC STREAM :)
DACSETUP:
        ;INC HL  ; STREAM ID 
        call GetNextSongByte ; 0 
        LD A,(HL) 
        LD (StreamID),A 
        ; Enable DAC mode on CH6
        ;call ZWAIT 
        ;ld a,$2b 
        ;ld ($4000),a 
        ;call ZWAIT 
        ;ld a,$80 
        ;ld ($4001),a 
        ;INC HL  ; CHIP TYPE 
        call GetNextSongByte ; 2 
        ;LD A,(HL) 
        ;CP 2
        ;JR NZ,fail
        ;INC HL  ; data (0)
        call GetNextSongByte ; 0 
        ;ld a,(hl)
        ;ld b,a   ; store
        call GetNextSongByte ; reg
        ;call ZWAIT 
        ;ld a,(hl) ; reg 
        ;ld ($4000),a    ; 2a
        ;call ZWAIT  
        ;ld a,b       ; 0 
        ;ld ($4001),a 
        jp PLAYLOOP  ; next is 91

SETDACSTREAM:        
; 0X91 <STREAM> <DATABANK ID> <STEP SIZE> <STEP BASE>
;         0           0            1           0 
; Both examples I've seen are 91 00 00 01 00 
        call GetNextSongByte ; sTREAM ID
        ; skip = -
        call GetNextSongByte ; Databank ; almost always 0
        ; Set the active block
        ld a,(hl) 
        ld (ActiveDacBlock),a 
        ; realistically I should be resetting it here too 
        call GetNextSongByte ; always 1? (step)
        call GetNextSongByte ; always 0? (step base)
        jp PLAYLOOP  ; next 52

DacTransferActive: 
        defb 0
DacFrequency:
        defb 0, 0

SETDACFREQ: ; 7D00 == 32000
; 7D00 = 32000 = 533 N 640P
; 3E80 = 16000 = 267 N 320P
; 5622 = 22050 = 368 N 441P 
; 2b11 = 11025 = 184 N 221P
; 1f40 = 8000 =  134 N 160P
; Right now does nothing. Samplerate hard coded
        call GetNextSongByte ; STREAM I
        call GetNextSongByte ; LOW BYTE FREQ 
        call GetNextSongByte ; 8-15
        call GetNextSongByte
        call GetNextSongByte 
       ;  ; TODO divide by framerate for PAL support
        push hl 
         ld hl,267                ; 267  
         ld (DacFrequency),hl ; typically 16000, or 267 per frame
        pop hl ; bc = freq, hl = song ptr, DacFrequency = freq
        jp PLAYLOOP 

STARTDACSTREAM:
;        inc hl  ; stream id 
        ; TODO NOT USED IN MY EXAMPLES
        JR STARTDACSTREAM

; 95 00 (0-15) 00
ActiveDacBlock: defb 0  ; which block 0-15 stored below
ActiveDacCtr: defb 0,0,0,0 ; 32bit count
ActiveDacLoc: defb 0,0  ; what is the memory offset
ActiveDacBank: defb 0   ; which 68000 memory bank
LoopDac: defb 0

DACFASTCALL:
;;;;;;;;;;;;;;;;;;;;;;;;
; This should start the DAC playback
; 90 00 
; for data block info:
; data type 1
; datasize 4
; dataloc 2  (hl) 
; databank 1 (rom)
; datacounter 4 (bytes left this loop)
; blockactive 1 (gogogo)

        call GetNextSongByte ; STREAM ID - ALWAYS 0 FOR NOW
        ld a, 1
        ld (DacTransferActive),a  ; SET TRANSFER TO ACTIVE SO WE DO IT LOL
        call GetNextSongByte
        ld a,(hl)       ; [A] contains block ID 
        cp 0 
        jr nz, _nope  ; FIXME: ONE STREAM FOR NOW
        ; warning! only have ram for 0-15!
        ld (ActiveDacBlock),a ; low byte 
        push hl 
         ld hl,DataType ; block 0 byte 0
         sla a ; * 2
         sla a ; * 4
         sla a ; * 8 
         sla a ; * 16  ; we are 16 bytes large 
         add a,l   ; carry?
         ld l,a 
         ld a,0 
         ld c,a 
         ld b,0 
         adc hl,bc ; HL now has (DataType) + blockID*8
         push HL 
         pop bc ; put it in BC
        pop hl ; recover song pointer 
        call GetNextSongByte ; block ID high 8 bits - skip this 
        ld a,(bc) ; data type
        inc bc  ; TODO data types
        ld a,(bc) ; data size 0
        ld (ActiveDacCtr),a 
        inc bc 
        ld a,(bc) 
        ld (ActiveDacCtr+1),a 
        inc bc 
        ld a,(bc) 
        ld (ActiveDacCtr+2),a 
        inc bc 
        ld a,(bc) 
        ld (ActiveDacCtr+3),a ; store 32bit dac size
        inc bc 
        ld a,(bc) 
        ld (ActiveDacLoc),a 
        inc bc         
        ld a,(bc) 
        ld (ActiveDacLoc+1),a ; 16bit dac offset 

        inc bc 
        ld a,(bc) ; data rom bank #
        ld (ActiveDacBank),a 
        ;INC HL          ; FLAGS 
        call GetNextSongByte
        ld a, (hl)      ; AM I LOOPING?
        ld (LoopDac),a  ; FIXME - NO LOOP FOR NOW
        ; enable!

        JP PLAYLOOP 

_nope: 
        call GetNextSongByte
        call GetNextSongByte
        jp PLAYLOOP 
        ; sigh 


; data block struct:
; (ff means empty)
; Compatible types:
; 0: YM2612 PCM data 
; 40: compressed YM2612 PCM data
DataType: defb $ff
DataSize: defb 0,0,0,0
DataLoc: defb 0,0
DataBank: defb 0
DataCounter: defb 0,0,0,0 
BlockActive: defb 0
_buffer: defb 0,0,0          ; 16 bytes 

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


DATABLOCK:
;;;;;;;;;;;;
;
        call GetNextSongByte ; should be 0x66
        ; find an empty block of pointers
        push hl ;9f43 = 66
         ld hl,DataType-16
         ld bc,16      ; 8 BYTES IN SIZE 
_blockfind:
         xor a 
         adc hl,bc 
         ld a,(hl)    ; hl += 8
         cp $ff 
         jr nz,_blockfind 
         push hl 
         pop bc ; now BC has block struct pointer. < up to here ok
_blockok:         
        pop hl                  ; 9f43++
        call GetNextSongByte
        ld a,(hl) 
        ld (bc),a  ; data type  ; = 0
        call GetNextSongByte
        
        inc bc 
        ld a,(hl)  ; 9f45 = 77
        ld (bc),a ; data size
        ld (WORKRAM),a 
        call GetNextSongByte
        
        ld a,(hl)
        inc bc  
        ld (bc),a ; data size+1 = 0c
        ld (WORKRAM+1),a 
        call GetNextSongByte
        
        inc bc 
        ld a,(hl) 
        ld (bc),a ; data size+2 = 00
        ld (WORKRAM+2),a 
        call GetNextSongByte
        
        inc bc 
        ld a,(hl) 
        ld (bc),a ; data size+3 = 00
        ld (WORKRAM+3),a 
        
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
        ; TODO data counter and blockactive 
        ld (bc),a ; and store current rom bank, cuz this must be where it starts
        ; TODO: Check data type
        ; store BANK and HL
        ld (ZadrWork),a 
        xor a 
        ld (ZadrWork+1),a 
        ld (ZadrWork+2),hl ; 0, 0, 9a9f ( = 1a9f)
        ; NEW FUNCTION: Get 68000 address from current bank/HL
        call GETROMADDRESS
        ; add WORKRAM to ZadrWork
        ld hl,(ZadrWork) ; 
        ld bc,(WORKRAM)  ; data size low 16 (c77 = ok)
        or a 
        adc hl,bc ; check carry 
        ld (ZadrWork),hl ; save low 16
        ld hl,(ZadrWork+2) 
        ld bc,$0000
        adc hl,bc  ; ok  ; carry is cleared here)
        ld bc,(WORKRAM+2) 
        adc hl,bc  ; upper 16
        ld (ZadrWork+2),hl 
        call SETZADDRESS  ; should be ok
_compressed: ; TODO do extra stuff for compressed data
_uncompressed:
        jp PLAYLOOP     


;bswork: defb 0,0,0,0,0,0,0,0,0

BANKSWAP:
;;;;;;;;;;;;;
; * A (preserve bc)
; RomBank has the # of the 32kb block to write to 6001.
;  Keep it 8 bits for now TODO 9th bit
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
        call ZWAIT 
        xor a 
        jr _bwrt 
_bone:
        call ZWAIT 
        ld a,1
_bwrt:
        ld ($6001),a 
        sla c           ; = 0b00000010
        jr nc,_bswp2    ; loop until bit goes into carry

        call ZWAIT 
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
        call GetNextSongByte
        CALL ZWAIT 
        LD A,(HL) 
        ld (PSGREG),a 
        JP PLAYLOOP 
;;;;

SAMPLEWAIT:
;;;;;;;;;;;
; * A, B, C
; HL = song byte 
        call GetNextSongByte
        LD A,(HL)
        LD C,A  
        call GetNextSongByte
        LD A,(HL)
        LD B,A          ; BC = SAMPLE WAIT 16BIT
        ld a,1 
        ld (PlayNext),a  ; always do the first frame, otherwise we will lag
_sampwt:
        LD A,(PlayNext)  ; check the 'play next frame' var
        CP 1             ; is it = 1?
        JR NZ,_sampwt    ; if not, wait until frame is over
        xor a
        ld (PlayNext),a  ; clear play var
        ; subtract 735 from BC 
        push hl         ; save song pointer
          push bc       ; sample wait value:
          pop hl        ; bc to hl 
         ld bc,735      ; TODO PAL. 735 = NTSC
         sbc hl,bc      ; samplewait - 735 -> HL
         push hl 
         pop bc         ; store it back in bc 
        pop hl          ; recover song pointer
        jr nc,_sampwt   ; wait another frame if samplewait is still > 0
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
