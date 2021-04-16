//bentgen.h
#ifndef BENTGEN
#define BENTGEN 

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef const char String[];

#define FALSE 0 
#define TRUE 1
#define true TRUE 
#define false FALSE 

#define WRITE 0x8000
#define READ 0
#define REG(n) (n << 8)
#define bit(n) (1 << n)

#define BG_A 0 
#define BG_B 1 
#define SPRITE 2 
#define WINDOW 3
#define WRITE_PAL0 (u32)(0xc0000000)
#define WRITE_PAL1 (u32)(0xc0200000)
#define WRITE_PAL2 (u32)(0xc0400000)
#define WRITE_PAL3 (u32)(0xc0600000)

#define WRITE_CTRLREG(n) asm("move.l %0,(0xc00004).l"::"g"(n))
#define WRITE_DATAREG8(n) asm("move.b %0,(0xC00000).l"::"g"(n))
#define WRITE_DATAREG16(n) asm("move.w %0,(0xc00000).l"::"g"(n))
#define WRITE_DATAREG32(n) asm("move.l %0,(0xc00000).l"::"g"(n))
#define READ_DATAREG32(n) asm("move.l (0xc00000).l,%0":"=g"(n):)
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
#define VDP_CRAM_READ 0x20000000
#define VDP_CRAM_WRITE WRITE_PAL0
#define VDP_VRAM_READ 0x00000000
#define VDP_VRAM_WRITE 0x40000000
#define VDP_VSRAM_READ 0x10000000
#define VDP_VSRAM_WRITE 0x14000000
//; buttons
#define BUTTON_UP_PRESSED 0x01
#define BUTTON_DOWN_PRESSED 0x02
#define BUTTON_LEFT_PRESSED 0x04
#define BUTTON_RIGHT_PRESSED 0x08
#define BUTTON_B_PRESSED 0x10
#define BUTTON_C_PRESSED 0x20
#define BUTTON_A_PRESSED 0x40
#define BUTTON_START_PRESSED 0x80

// standard vram map:
// 0000 - C000 : pattern defs
// C000 - D800 : BG_A
// D800 - E000 : SAT
// e000 - f000 : BG_B
// f000 - fc00 : window map
// fc00 +      : hscroll table 

// Tilemap format:
// LPPVHTTT TTTTTTTT
// bits 0-10: tile num 0-1024
// bit 11: Hflip
// bit 12: Vflip
// bits 13-14: palette no (0-3)
// bit 15: layer priority
#define pal_no(n) (n<<13)

void _start();
void main();
void __attribute__((interrupt)) catch();
void __attribute__((interrupt)) HBlank();
void __attribute__((interrupt)) VBlank();
void __attribute__((optimize("Os"))) LoadPalette(u8 palNo, const u16* p);
void SetVDPPlaneAddress(u8 plane, u16 addr);
void SetVRAMWriteAddress(u16 address);
void SetVRAMReadAddress(u16 address);


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

#define VDPStatus_u16(var) \
    asm("move.w (0xc00004).l,%0":"=g"(var)::)
#define WaitVBlank() asm volatile("VB%=: move.w (0xc00004).l,%%d0\n\t" \
        "btst #3,%%d0\n\t" \
        "beq VB%=\n\t" \
        "VBB%=: move.w (0xc00004).l,%%d0\n\t"\
        "btst #3,%%d0\n\t"\
        "bne VBB%="\
    : \
    : \
    :"d0")
    
#define WaitHBlank() \
    asm volatile("VB%=: move.w (0xc00004).l,%%d0\n\t" \
        "btst #2,%%d0\n\t" \
        "beq VB%=" \
    : \
    : \
    :"d0"); \
    asm volatile("VB%=: move.w (0xc00004).l,%%d0\n\t" \
        "btst #2,%%d0\n\t" \
        "bne VB%=" \
    : \
    : \
    :"d0")

#define EnableIRQLevel(n) asm("move.w %0,sr"::"g"((n<<8)|0x20))


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
void VBlank()
{
    GAME_DRAW();
    return;
}

void _start() {
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
void LoadPalette(u8 palNo, const u16* p)
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


#endif