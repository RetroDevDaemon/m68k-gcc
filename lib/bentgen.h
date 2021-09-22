//bentgen.h
#ifndef BENTGEN
#define BENTGEN 

// VGM PLAYER BYTES
#include "vgmplayer.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef volatile u8 vu8;
// fix me 
typedef u8 bool;
typedef const unsigned char String[];
typedef signed long s32;
typedef signed short s16;
typedef signed char s8;

#define NULL 0 
#define null NULL 
#define FALSE 0 
#define TRUE 1
#define true TRUE 
#define false FALSE 

#define NO_HFLIP 0
#define YES_HFLIP 1
#define NO_VFLIP 0
#define YES_VFLIP 1
#define PAL0 0
#define PAL1 1
#define PAL2 2
#define PAL3 3
#define PRIORITY_LOW 0
#define PRIORITY_HI 1

#define nullptr 0

#define WRITE 0x8000
#define READ 0
#define REG(n) ((n) << 8)
#define bit(n) (1 << (n))

#define BG_A 0 
#define BG_B 1 
#define SPRITE 2 
#define WINDOW 3
#define WRITE_PAL0 (u32)(0xc0000000)
#define WRITE_PAL1 (u32)(0xc0200000)
#define WRITE_PAL2 (u32)(0xc0400000)
#define WRITE_PAL3 (u32)(0xc0600000)
#define SPRSIZE(x,y) (u8)(((x-1)<<2)|(y-1))

#define Video_ON (1 << 6)
#define Video_OFF (0)
#define VBLIRQ_ON (1 << 5) 
#define VBLIRQ_OFF (0)
#define DMA_ON (1 << 4) 
#define DMA_OFF (0) 
#define NTSC (0) 
#define PAL (1 << 3) 
#define MODE (1 << 2)
// MODE|Video_ON|VBLIRQ_ON|DMA_ON|NTSC

#define WRITE_CTRLREG(n) asm("move.l %0,(0xc00004).l"::"g"(n))
#define WRITE_DATAREG8(n) asm("move.b %0,(0xC00000).l"::"g"(n))
#define WRITE_DATAREG16(n) asm("move.w %0,(0xc00000).l"::"g"(n))
#define WRITE_DATAREG32(n) asm("move.l %0,(0xc00000).l"::"g"(n))
#define READ_DATAREG32(n) asm("move.l (0xc00000).l,%0":"=g"(n):)
#define READ_DATAREG16(n) asm("move.w (0xc00000).l,%0":"=g"(n):)
#define BREAKPOINT asm("BRK%=:\n\t""jra BRK%=":::);

//; 68k memory map
#define CTRL_1_DATA 0x00A10003
#define CTRL_2_DATA 0x00A10005
#define CTRL_X_DATA 0x00A10007
#define CTRL_1_CONTROL 0x00A10009
#define CTRL_2_CONTROL 0x00A1000B
#define CTRL_X_control 0x00A1000D
#define REG_HWVERSION 0x00A10001
#define REG_TMS 0x00A14000
#define PSG_INPUT 0x00C00011
#define RAM_START 0x00FF0000
#define VDP_DATA 0x00C00000
#define VDP_CONTROL 0x00C00004
#define VDP_COUNTER 0x00C00008
#define Z80_ADDRESS_SPACE 0x00A10000
#define Z80_BUS 0x00A11100
#define Z80_RESET 0x00A11200
//; VDP access modes
#define VDP_CRAM_READ 0x20000000 // 0x200000 ++..
#define VDP_CRAM_WRITE WRITE_PAL0
#define VDP_VRAM_READ 0x00000000
#define VDP_VRAM_WRITE 0x40000000
#define VDP_VSRAM_READ 0x10000000
//#define VDP_VSRAM_WRITE 0x14000000 bad
//; buttons
#define BTN_UP_PRESSED (u16)bit(0)
#define BTN_DOWN_PRESSED (u16)bit(1)
#define BTN_LEFT_PRESSED (u16)bit(10)
#define BTN_RIGHT_PRESSED (u16)bit(11)
#define BTN_B_PRESSED (u16)bit(12)
#define BTN_C_PRESSED (u16)bit(13)
#define BTN_A_PRESSED (u16)bit(4)
#define BTN_START_PRESSED (u16)bit(5)
#define BTN_X_PRESSED (u32)(bit(19))
#define BTN_Y_PRESSED (u32)(bit(18))
#define BTN_Z_PRESSED (u32)(bit(17))
#define BTN_MODE_PRESSED (u32)(bit(16))

#define REPT10(n) n;n;n;n;n;n;n;n;n;n;
#define REPT4(n) n;n;n;n;
#define REPT8(n) REPT4(n);REPT4(n);
#define REPT3(n) n;n;n;
#define REPT9(n) REPT3(REPT3(n))
#define REPT20(n) REPT10(n);REPT10(n);

// standard vram map:
// 0000 - C000 : pattern defs (enough for 1536, max 2047=0xffeo)
// C000 - D800 : BG_A
// D800 - E000 : SAT
// e000 - f000 : BG_B
// f000 - f800 : window map
// f800 +      : hscroll table 

// Tilemap format:
// LPPVHTTT TTTTTTTT
// bits 0-10: tile num 0-1024
// bit 11: Hflip
// bit 12: Vflip
// bits 13-14: palette no (0-3)
// bit 15: layer priority
#define pal_no(n) (n<<13)


// Roll a die:
#define roll(y) ((random() & (y-1)) + 1)

// Color word in vram format, 0-30 even only
#define CRAMRGB(R, G, B) (u16)((B<<8)|(G<<4)|(R<<0)) 

/*
Sprite Attribute Table
----------------------

All sprite data is stored in a region of VRAM called sprite attribute table.
The table is 640 bytes in size. Each 8-byte entry has the following format:

Index + 0 : ------yy yyyyyyyy
Index + 2 : ----hhvv
Index + 3 : -lllllll
Index + 4 : pccvhnnn nnnnnnnn
Index + 6 : ------xx xxxxxxxx

y = Vertical coordinate of sprite
h = Horizontal size in cells (00b=1 cell, 11b=4 cells)
v = Vertical size in cells (00b=1 cell, 11b=4 cells)
l = Link field
p = Priority
c = Color palette
v = Vertical flip
h = Horizontal flip
n = Sprite pattern start index
x = Horizontal coordinate of sprite
*/

// 0-1: Y position (9 bits, 0-511, or 0-1023 in double mode)
// 2: 'next' sprite. final sprite = 0
// 3: bits 0-1 Y size, bits 2-3 X size.
//    0xE => X=4 Y=3
// 4-5: tile number(10 bits, 0-2047) | hf(1<<11) | vf(1<<12) | pal(2<<13) | priority(1<<15)
// 6-7: X position (8 bits, 0-511)
typedef struct spriteAttribute { 
    u16 y_pos;
    u8 size;
    u8 next;
    u16 spr_attr;
    u16 x_pos;
} Sprite;

#define SPR_ATTR(tileno, hf, vf, pal, pri) \
    (u16)((tileno)|((hf)<<11)|((vf)<<12)|((pal)<<13)|((pri)<<15))
#define TILEATTR(n, hf, vf, pal, pri) \
    (u16)((n)|((hf)<<11)|((vf)<<12)|((pal)<<13)|((pri)<<15))

void _start();
int main();
void GAME_DRAW();
void __attribute__((interrupt)) catch();
void __attribute__((interrupt)) HBlank();
void __attribute__((interrupt)) VBlank();
void __attribute__((optimize("Os"))) LoadPalette(u8 palNo, u16* p);
void SetVDPPlaneAddress(u8 plane, u16 addr);
//void __attribute__((section(".funcram"))) SetVRAMWriteAddress(u16 address);
void SetVRAMWriteAddress(u16 address);
void SetDMAWriteAddress(u16 address);
void SetVRAMReadAddress(u16 address);
void print(u8 plane, u8 x, u8 y, String str);
void byToHex(u8 by, u8* ar);

u16 VDPLoadTiles(u16 ti, u32* src, u16 numTiles);
typedef struct meta_tile { 
	u8 size;
	u8 pal;
	u16 ia; // top left tile index 
	u16 ib; // optional - 2x2
	u16 ic; // optional - 3x3
	// 0, 1, [2]
	// 20, 21, [22]
	//[40, 41, 42]
} metaTile;
//
Sprite* AddSprite(Sprite* as, u16 ypos, u8 size, u16 attr, u16 xpos);
u16 strsize(String* s);
void LinkAllSpriteData();
void DrawSpriteAsTile(u8 plane, u16 tileNo, u8 x, u8 y, u8 w, u8 h);
void DrawTile(u8 plane, u16 TILEATTR, u8 x, u8 y, u8 w, u8 h);
void UpdateBGScroll();
//
void PopulateMetatileList(u16 st_t, u16 en_t, u8 sz, metaTile* mt, u8 pal); 
// Sound
void LoadSong(const u8* son);
void PlaySong();


//void FlipTileRegionH(VDPPlane plane, u8 x1, u8 y1, u8 x2, u8 y2);
//void FlipTileRegionV(VDPPlane plane, u8 x1, u8 y1, u8 x2, u8 y2);
//void CopyMapRect(Map* source, VDPPlane tgtplane, u8 palNo, u16 tileIndex, u8 x1, u8 y1, u8 w, u8 h, u8 x2, u8 y2, BOOL p);
//void ChangeTileRectPalette(VDPPlane plane, u8 x1, u8 y1, u8 x2, u8 y2, u8 palNo);
void FlashAllPalettes();


static u16 tempPalettes[4][16];     
static s16 bga_hscroll_pos = 0;
static s16 bga_vscroll_pos = 0;
static s16 bgb_hscroll_pos = 0;
static s16 bgb_vscroll_pos = 0;
// required defs
typedef s32 fp32;
typedef s16 fp16;
typedef unsigned char Map[]; // FIXME
typedef u8 TileSet[];
typedef u8 SpriteDefinition[];

typedef s32 fix32;
typedef s16 fix16;
#define fp16(n) (fix16)((n)*(1<<8))
#define fp32(n) (fix32)((n)*(1<<16))


#define LOADPAL(pal) asm("move.l %1, (0xc00004).l\n\t"\
    "lea %0, %%a0\n\t"\
    "move.l #0x07, %%d0\n\t"\
    "clrlewp:\n\t"\
    "move.l (%%a0)+,(0xc00000).l\n\t"\
    "dbra %%d0, clrlewp"\
    :\
    :"m"(pal),"g"(CRAM_ADDR)\
    :"a0","d0");

#define WriteVDPRegister(v) asm("move.w %0,(0xC00004).l"::"g"(v))
#define VDPStatus_u16(var) asm("move.w (0xc00004).l,%0":"=g"(var)::)
#define EnableIRQLevel(n) asm("move.w %0,sr"::"g"((n<<8)|0x20))


#define WaitVBlank() asm("VB%=: move.w (0xc00004).l,%%d0\n\t" \
        "btst #3,%%d0\n\t" \
        "beq VB%=\n\t" \
        "VBB%=: move.w (0xc00004).l,%%d0\n\t"\
        "btst #3,%%d0\n\t"\
        "bne VBB%=":::"d0");
    
#define WaitHBlank() asm("VB%=: move.w (0xc00004).l,%%d0\n\t" \
        "btst #2,%%d0\n\t" \
        "beq VB%=":::"d0");\
    asm("VB%=: move.w (0xc00004).l,%%d0\n\t" \
        "btst #2,%%d0\n\t" \
        "bne VB%=":::"d0")

#define GETJOYSTATE1(n) asm(\
        "move.l #0,%%d0             | clear d0\n\t"\
        "moveq #0x40,%%d0           | set bit 6\n\t"\
        "move.b %%d0,(0xa10009).l   | set TH pin to 'write'\n\t"\
        "move.b %%d0,(0xa10003).l   | TH to 1\n\t"\
        "nop\n\tnop                 | (sync!)\n\t"\
        "move.b (0xa10003).l,%%d0   | read byte 1\n\t"\
        "rol.w #8,%%d0              | shift to upper\n\t"\
        "move.b #0,(0xa10003).l     | TH to 0\n\t"\
        "nop\n\tnop                 | (sync!)\n\t"\
        "move.b (0xa10003).l,%%d0   | read byte 2\n\t"\
        "move.l %%d0,%0             | copy to output"\
        :"=g"(n)::"d0"); n ^= 0xffff; // OR result
        
#define GETJOYSTATE2(n) asm(\
        "move.l #0,%%d0             | clear d0\n\t"\
        "moveq #0x40,%%d0           | set bit 6\n\t"\
        "move.b %%d0,(0xa1000b).l   | set TH pin to 'write'\n\t"\
        "move.b %%d0,(0xa10005).l   | TH to 1\n\t"\
        "nop\n\tnop                 | (sync!)\n\t"\
        "move.b (0xa10005).l,%%d0   | read byte 1\n\t"\
        "rol.w #8,%%d0              | shift to upper\n\t"\
        "move.b #0,(0xa10005).l     | TH to 0\n\t"\
        "nop\n\tnop                 | (sync!)\n\t"\
        "move.b (0xa10005).l,%%d0   | read byte 2\n\t"\
        "move.w %%d0,%0             | copy to output"\
        :"=g"(n)::"d0"); n ^= 0xffff; // OR result

#define Joy1Down(b) (bool)\
    (!(last_joyState1 & b) && \
    (joyState1 & b))
#define Joy1Up(b) (bool)\
    ((last_joyState1 & b) &&\
    !(joyState1 & b))
#define Joy2Down(b) (bool)\
    (!(last_joyState2 & b) && \
    (joyState2 & b))
#define Joy2Up(b) (bool)\
    ((last_joyState2 & b) &&\
    !(joyState2 & b))
#define elseif else if 


void catch()
{
    asm("jsr exceptionDump");
    return;
}

// Horizontal blank requires EnableIRQLevel(3) and VDP reg 0 set
void HBlank()
{
    return;
}

// Vertical blank requires EnableIRQLevel(5) and VDP reg 1 set
//static bool VBL_DONE = false;
void VBlank()
{
    //if(VBL_DONE) return;
    GAME_DRAW();
    //VBL_DONE = true;
    return;
}

extern char _sbss[];
extern char _ebss[];

void memset(char* start, u32 val, unsigned int length)
{
    // length is between 0-65000.
    u16 i;
    for(i = 0; i < length; i++) *start++ = val;
    /*
    u8 div;
    div = length % 4;
    switch(div){
        case(0): // remainder 0, longword
            asm("\n"
            "move.l     %0,%%d1\n\t"\
            "asr.w 	    #2,%%d1		;| divide by 4 (longword)\n\t"
            "movea.l	%1,%%a0 ;| address of what to erase	\n\t"
        "ClrLoop%=:\n\t"
            "move.l	    %2,(%%a0)+\n\t"
            "dbra	    d1,ClrLoop%="
            :                  // outputs
            :"g"(length), "m"(start), "g"(val) // inputs
            :"d0", "d1", "a0"); // used registers
        break;

        case(2):
            asm("\n"
            "move.l     %0,%%d1\n\t"\
            "asr.w 	    #2,%%d1		;| divide by 2 (word)\n\t"
            "movea.l	%1,%%a0 ;| address of what to erase	\n\t"
        "ClrLoop%=:\n\t"
            "move.w	    %2,(%%a0)+\n\t"
            "dbra	    d1,ClrLoop%="
            :                  // outputs
            :"g"(length), "m"(start), "g"((u16)(val & 0xffff)) // inputs
            :"d0", "d1", "a0"); // used registers
                    
        case(3):
        case(1): //remainder 1, byte 
            asm("\n"
            "move.l     %0,%%d1\n\t"\
            //"asr.w 	    #2,%%d1		;| divide by 4 (longword)\n\t"
            "movea.l	%1,%%a0 ;| address of what to erase	\n\t"
        "ClrLoop%=:\n\t"
            "move.b	    %2,(%%a0)+\n\t"
            "dbra	    d1,ClrLoop%="
            :                  // outputs
            :"g"(length), "m"(start), "g"(val) // inputs
            :"d0", "d1", "a0"); // used registers
        break;
    }
    */
}

void _start() {
    //asm("movea.l 0x00FFF000,%%sp":::"sp"); // set stack pointer if its not
    // Important!! Clear BSS!!
    memset(_sbss, 0, _ebss - _sbss);
    // enable VBL IRQ
    EnableIRQLevel(5);
    main(); 
}

void SetVRAMWriteAddress(u16 address)
{
    u32 loc = 0x40000000 + ((address & 0x3fff) << 16) + ((address & 0xc000) >> 14);
    asm("move.l %0,(0xc00004).l"
    :
    :"g"(loc));
    // MOVE.L #$40000000 + ((0xc000 & 0x3fff) << 16) + ((0xc000 & 0xc000) >> 14);
}

void SetDMAWriteAddress(u16 address)
{
    u32 loc = 0x40000080 + ((address & 0x3fff) << 16) + ((address & 0xc000) >> 14);
    //loc |= 0x80; // flag for DMA
    asm("move.l %0,(0xc00004).l"
    :
    :"g"(loc));
}

void SetVRAMReadAddress(u16 address)
{
    u32 loc = 0x00000000 + ((address & 0x3fff) << 16) + ((address & 0xc000) >> 14);
    asm("move.l %0,(0xc00004).l"
    :
    :"g"(loc));
}

void SetVDPPlaneAddress(u8 plane, u16 addr)
{
    u32 f = 0;
    switch(plane){
        case(0): // BG_A
            f = 0x8200 + (addr >> 10);
            break;
        case(1): // BG_B 
            f = 0x8400 + (addr >> 13);
            break;
        case(2): // WINDOW
            f = 0x8300 + (addr >> 10);
            break;
        case(3): // SPRITE 
            f = 0x8500 + (addr >> 9);
            break;
    }
    asm("move.w %0,(0xC00004).l"
    :
    : "g"(f));
}

// 
void LoadPalette(u8 palNo, u16* p)
{
    // Auto-inc to Word
    WriteVDPRegister((u32)WRITE|REG(0xf)|2);
    switch(palNo) {
        case(0):
            WRITE_CTRLREG(WRITE_PAL0); 
            break;
        case(1):
            WRITE_CTRLREG(WRITE_PAL1); 
            break;
        case(2):
            WRITE_CTRLREG(WRITE_PAL2); 
            break;
        case(3):
            WRITE_CTRLREG(WRITE_PAL3); 
            break;
    }
    u8 i;
    for(i = 0; i < 16; i++) WRITE_DATAREG16(p[i]);   
}

void print(u8 plane, u8 x, u8 y, String str)
{
    // 2 bytes per character, 64 chars per plane row * 2 = 128 or $80 for newline
    switch(plane){
        case(BG_A):    // BG_A
            // page $c0 + 2*((BG_WIDTH * y) + x)
            SetVRAMWriteAddress( VRAM_BG_A + (2 * ((BG_WIDTH * y) + x)) );
            break;
        case(BG_B):    // BG_B
            SetVRAMWriteAddress( VRAM_BG_B + (2 * ((BG_WIDTH * y) + x)) );
            break;
    }
    u8 i = 0;
    while (str[i] != '\00')
    {
        WRITE_DATAREG16(str[i]);
        i++;
    }
}


// Use DrawTile when possible
void DrawSpriteAsTile(u8 plane, u16 tileNo, u8 x, u8 y, u8 w, u8 h)
{
    //WriteVDPRegister(WRITE|REG(0xf)|2); // auto - inc
    u16 start = VRAM_BG_A + (2 * ((BG_WIDTH * y) + x));
    if(plane == BG_B) start = VRAM_BG_B + (2 * ((BG_WIDTH + y) + x));
    for(u8 xx = 0; xx < w; xx++) { 
        for(u8 yy = 0; yy < h; yy++) {
            SetVRAMWriteAddress((u16)start + (0x40*yy) + xx); // much slower iteration loop
            WRITE_DATAREG16(tileNo++);
        }
    }
}

// Tilemap format:
// LPPVHTTT TTTTTTTT
// bits 0-10: tile num 0-1024
// bit 11: Hflip
// bit 12: Vflip
// bits 13-14: palette no (0-3)
// bit 15: layer priority
void DrawTile(u8 plane, u16 TILEATTR, u8 x, u8 y, u8 w, u8 h)
{
    //WriteVDPRegister(WRITE|REG(0xf)|2); // auto - inc
    // 2 bytes per character, 64 chars per plane row * 2 = 128 or $40 for newline
    u16 start = 0;
    if(plane == BG_A) start = VRAM_BG_A + (2 * ((BG_WIDTH * y) + x));
    if(plane == BG_B) start = VRAM_BG_B + (2 * ((BG_WIDTH + y) + x));
    for(u8 yy = 0; yy < h; yy++) { 
        SetVRAMWriteAddress((u16)(start + (0x40*yy)));
        for(u8 xx = 0; xx < w; xx++) {
            WRITE_DATAREG16(TILEATTR++);
        }
    }
}


// Ensure VRAM_SCROLL is defined first: default 0xf800
///// WARNING: THIS ONLY WORKS FOR FC00
void UpdateBGScroll()
{
    //WriteVDPRegister(WRITE|REG(0xf)|4);
    SetVRAMWriteAddress(VRAM_SCROLL);
    WRITE_DATAREG32((u32)(bga_hscroll_pos << 16) | (bgb_hscroll_pos));
    asm("move.l %0,(0xc00004).l":
        :"g"(0x7c000013):); // i think this is broke
    WRITE_DATAREG32((u32)(bga_vscroll_pos << 16) | (bgb_vscroll_pos));
    //WriteVDPRegister(WRITE|REG(0xf)|2);
}

// Usage: 
//<new sprite ptr> = AddSprite(<ptr to sprite data in SAT>, ...)
Sprite* AddSprite(Sprite* as, u16 ypos, u8 size, u16 attr, u16 xpos)
{
    as->y_pos = ypos;
    as->size = size;     // use SPRSIZE macro 
    as->spr_attr = attr;
    as->x_pos = xpos;
    return as;
}

const void* spriteRamBase;
// Before calling this, point spriteRamBase to the correct location
void LinkAllSpriteData()
{
    Sprite* s = (Sprite*)spriteRamBase;
    // Set all 80 sprites to linear draw/link order
    for(u8 i = 0; i < 80; i++) 
    {
        s[i].spr_attr = null;   // set to blank sprite
        s[i].next = i + 1;
        s[i].x_pos = 511;
        s[i].y_pos = 511;
        s[i].size = 0;
    }
    s[79].next = 0;     // final sprite must link to 0
}

u16 strsize(String* s)
{
    u16 sz;
    char* c = s;
    while(*c++ != '\00') sz++;
    return sz;
}

void byToHex(u8 by, u8* ar)
{
    u8 a = by & 0xf;
    u8 b = (by & 0xf0) >> 4;
    if (a > 9) a += 7;
    if (b > 9) b += 7;
    a += 0x30;
    b += 0x30;
    ar[0] = b; ar[1] = a; ar[2] = 0x20;
}

u16 VDPLoadTiles(u16 ti, u32* src, u16 numTiles)
{
    u16 c;
    u32* cr;
    SetVRAMWriteAddress(ti * 32);
    cr = src;
    for(c = 0; c < (8*numTiles); c++)
    {
        WRITE_DATAREG32(*cr++);
    }
    ti += numTiles;
    return ti;
}


static u8 zcyclesl;
static u8 zcyclesh;

void PlaySong()
{
	asm("move.w #0x100,(z80_bus_request).l");
	asm("z80busreqwait:");
	asm("btst #0,(z80_bus_request).l");
	asm("bne.s z80busreqwait");
	asm("move.b #1,(0xa00100).l");
	asm("move.b (0xa0010a).l, %%d0\n\t"\
		 "move.b %%d0,(%0)":"=g"(zcyclesl)::"d0");
	asm("move.b (0xa0010b).l, %%d0\n\t"\
		 "move.b %%d0,(%0)":"=g"(zcyclesh)::"d0");
	asm("move.w #0,(z80_bus_request).l");
}

void LoadSong(const u8* son)
{
	u32 stadr = (u32)son;
	u8 bank = (u8)((stadr >> 15));
	//stadr += 0x80;
#define z80_base_ram 0xa00000 
#define z80_bus_request 0xa11100
#define z80_reset 0xa11200     
	asm("\n\t\
	nop \n\t\
	move.w	#0x100,(z80_bus_request).l\n\t\
	move.w	#0x100,(z80_reset).l\n\t");
// COPY z80prg[] INTO Z80 MEMORY (A00000+)
	asm(\
	"movea.l %0,%%a0\n\t"\
	"movea.l %1,%%a1\n\t"\
	::"g"(&vgmplayer), "g"(z80_base_ram):"a0","a1","d1");
	asm(\
	"move.l %0,%%d1"\
	::"g"(sizeof(vgmplayer)):"d1");
	asm(".Z80COPYLOOP:\n\t"\
	"move.b (%%a0)+,%%d0\n\t"\
	"move.b %%d0,(%%a1)+\n\t"\
	"subq 	#1,%%d1\n\t"\
	"bne 	.Z80COPYLOOP\n\t"\
	:::"d0","d1","a0","a1");
// set up vgm address on the z80
	asm("move.b %0,(0xa00106).l"::"g"(bank)); // Rom bank # (A15-A23)
	// 0x82-0x85 = start address 32 bit 
	asm("move.b %0,(0xa00102).l"::"g"(stadr & 0xff));
	asm("move.b %0,(0xa00103).l"::"g"((stadr >> 8)));
	asm("move.b %0,(0xa00104).l"::"g"((stadr >>16)));
	asm("move.b %0,(0xa00105).l"::"g"((stadr >>24)));
	
// reset, start z80
	asm("\t\
	move.w #0,(z80_reset).l \n\t\
	nop \n\t\
	nop \n\t\
	nop \n\t\
	nop \n\t\
	move.w #0x100,(z80_reset).l \n\t\
	move.w #0,(z80_bus_request).l");
}


// TODO: bigger than 3x3
// TODO: sizes other than 10x15
void PopulateMetatileList(u16 st_t, u16 en_t, u8 sz, metaTile* mt, u8 pal)
{
	// start tile, end tile, mt size in tiles (2 or 3)
	u16 i;
	metaTile* tile = mt;
	u16 d = st_t;
	u8 r = 0;
	
	for(i = 0; i < 150; i++)
	{
		tile[i].pal = pal;
		tile[i].size = sz;
		tile[i].ia = d; // 138;
		tile[i].ib = d + 20; //158;
		tile[i].ic = d + 40;
		d += sz;
		r += 1;
		if(((r)%10) == 0){
			if(sz == 2) 
				d += 20;
			else if(sz == 3)
				d += 40;
			r = 0;
		}
	}
}



//#include "gfx.h"

// cram 0,0 is at vram 0x0000.
// pal 01 is at +32, or 0b0000000000100000
//BBAA AAAA AAAA AAAA 0000 0000 BBBB 00AA
//AA = Address 16 bit
//--DC BA98 7654 3210 ---- ---- ---- --FE
//BB = command
//10-- ---- ---- ---- ---- ---- 5432 ----
//01                            0001

//000000 – VRAM Read
//000001 – VRAM Write
//001000 – CRAM Read (!!)
//000011 – CRAM Write(!!)
//000100 – VSRAM Read
//000101 – VSRAM Write (testing)
#define read_cram 0b00000000000000000000000000100000
#define cram_pal0 0                                     // 0
#define cram_pal1 0b00000000001000000000000000000000    // 0x20
#define cram_pal2 0b00000000010000000000000000000000    // 0x40
#define cram_pal3 0b00000000011000000000000000000000    // 0x60
#define vsram_write 0b01111100000000000000000000010011 //fc00 -> 7c000013

//
/*
; ---------------------------------------------------------------------------
; stop the Z80
; ---------------------------------------------------------------------------

stopZ80:	macro
		move.w	#$100,(z80_bus_request).l
		endm

; ---------------------------------------------------------------------------
; wait for Z80 to stop
; ---------------------------------------------------------------------------

waitZ80:	macro
	@wait:	btst	#0,(z80_bus_request).l
		bne.s	@wait
		endm

; ---------------------------------------------------------------------------
; reset the Z80
; ---------------------------------------------------------------------------

resetZ80:	macro
		move.w	#$100,(z80_reset).l
		endm

resetZ80a:	macro
		move.w	#0,(z80_reset).l
		endm

; ---------------------------------------------------------------------------
; start the Z80
; ---------------------------------------------------------------------------

startZ80:	macro
		move.w	#0,(z80_bus_request).l
		endm

; ---------------------------------------------------------------------------
; disable interrupts
; ---------------------------------------------------------------------------

disable_ints:	macro
		move	#$2700,sr
		endm

; ---------------------------------------------------------------------------
; enable interrupts
; ---------------------------------------------------------------------------

enable_ints:	macro
		move	#$2300,sr
		endm

*/
#define SetDMASourceAddress(_a) WriteVDPRegister(WRITE|REG(0x15)|(u8)(((_a)>>1) & 0xff));\
		WriteVDPRegister(WRITE|REG(0x16)|(u8)((((_a)>>1) & 0xff00) >> 8));\
		WriteVDPRegister(WRITE|REG(0x17)|(u8)((((_a)>>1) & 0x7f0000) >> 16));

#define SetDMALength(_L) WriteVDPRegister(WRITE|REG(0x13)|(u8)(_L & 0xff));\
		WriteVDPRegister(WRITE|REG(0x14)|(u8)(_L >> 8));



#endif