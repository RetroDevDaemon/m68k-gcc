typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

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

#define BREAKPOINT asm("BRK%=:\n\t""jra BRK%=":::);


void _start();
void main();

void __attribute__((optimize("Os"))) LoadPalette(u8 palNo, u16* p);


#define LOADPAL(pal) asm("move.l %1, (0xc00004).l\n\t"\
    "lea %0, %%a0\n\t"\
    "move.l #0x07, %%d0\n\t"\
    "clrlewp:\n\t"\
    "move.l (%%a0)+,(0xc00000).l\n\t"\
    "dbra %%d0, clrlewp"\
    :\
    :"m"(pal),"g"(CRAM_ADDR)\
    :"a0","d0");


void __attribute__((interrupt)) catch()
{
    //asm("rts");
    return;
}

void _start() {
    
    main(); 
}

void SetVDPPlaneAddress(u8 plane, u16 addr);
void SetVRAMWriteAddress(u16 address);

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



void SetVRAMWriteAddress(u16 address)
{
    u32 loc = 0x40000000 + ((address & 0x3fff) << 16) + ((address & 0xc000) >> 14);
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
