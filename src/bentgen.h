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

void _start();
void main();

void _start() {
    // Test RESET
    asm("tst.w 0x00A10008\n\t"  
    "bne __resetOK\n\t"          
    "tst.w 0x00A1000C\n\t"  
    "bne __resetOK\n\t"         
    "__resetOK:":::); 
    // CLEAR RAM
    asm("move.l #0x00000000, %%d0\n\t"
    "move.l #0x00000000, %%a0\n\t"
    "move.l #0x00003FFF, %%d1\n\t"
    "__Clearram:\n\t"
    "move.l %%d0, -(%%a0)\n\t"      
    "dbra %%d1, __Clearram"
    :::"d0", "d1", "a0"); 
    // SEGA TMSS
    asm("move.b 0x00A10001, %%d0\n\t"      
    "andi.b #0x0f, %%d0\n\t"           
    "beq __Skiptmss\n\t"                  
    "move.l #0x53454741, 0x00A14000\n\t" 
    "__Skiptmss:"
    :::"d0");
    // INIT Z80
    asm("move.w #0x0100, 0x00a11100\n\t" //req acc to z80 bus
    "move.w #0x0100, 0x00a11200\n\t" // hold in reset
    "__initwait:\n\t"
    "btst #0, 0x00a11100\n\t" //68k has control?
    "bne __initwait\n\t"
    "move.l __Z80Data, %%a0\n\t"
    "move.l #0x00a00000, %%a1\n\t"
    "move.l #0x29, %%d0\n\t"
    "__copyz80init:\n\t"
    "move.b (%%a0)+,(%%a1)+\n\t"
    "dbra %%d0, __copyz80init\n\t"
    "move.w #0, 0x00a11200\n\t"
    "move.w #0, 0x00a11100\n\t"
    "jra __InitPSG\n\t"
    "__Z80Data:\n\t"
    ".word 0xaf01\n\t.word 0xd91f\n\t"
    ".word 0x1127\n\t.word 0x0021\n\t"
    ".word 0x2600\n\t.word 0xf977\n\t"
    ".word 0xedb0\n\t.word 0xdde1\n\t"
    ".word 0xfde1\n\t.word 0xed47\n\t"
    ".word 0xed4f\n\t.word 0xd1e1\n\t"
    ".word 0xf108\n\t.word 0xd9c1\n\t"
    ".word 0xd1e1\n\t.word 0xf1f9\n\t"
    ".word 0xf3ed\n\t.word 0x5636\n\t"
    ".word 0xe9e9\n\t.word 0x8104\n\t"
    ".word 0x8f01\n\t"
    :::"d0", "a0", "a1");
    // Init PSG
    asm("__InitPSG:\n\t"
    "move.l _PSGData, %%a0\n\t"
    "move.l #3, %%d0\n\t"
    "__copypsgp: move.b (%%a0)+, 0x00c00011\n\t"
    "dbra %%d0, __copypsgp\n\tjra __doneinitpsg\n\t"
    "_PSGData: .word 0x9fbf\n\t.word 0xdfff\n\t"
    "__doneinitpsg:"
    :::"a0","d0");
    //Init VDP
    asm("move.l __vdpreginitdata, %%a0\n\t"
    "move.l #0x18, %%d0\n\t"
    "move.l #0x00008000, %%d1\n\t"
    "_vdprcpy: move.b (%%a0)+,%%d1\n\t"
    "move.w %%d1, 0x00c00004\n\t"
    "add.w #0x0100, %%d1\n\t"
    "dbra %%d0, _vdprcpy\n\t"
    "jra __donevdpinit\n\t"
    "__vdpreginitdata: .byte 0x20\n\t.byte 0x74\n\t"
    ".byte 0x30\n\t.byte 0x40\n\t"
    ".byte 0x05\n\t.byte 0x70\n\t" //4-5
    ".byte 0x00\n\t.byte 0x00\n\t"
    ".byte 0x00\n\t.byte 0x00\n\t"
    ".byte 0x00\n\t.byte 0x08\n\t" //10-11
    ".byte 0x81\n\t.byte 0x34\n\t"
    ".byte 0x00\n\t.byte 0x00\n\t"
    ".byte 0x01\n\t.byte 0x00\n\t" //16-17
    ".byte 0x00\n\t.byte 0x00\n\t"
    ".byte 0x00\n\t.byte 0x00\n\t"
    ".byte 0x00\n\t.byte 0x00\n\t" //22-23
    "__donevdpinit:":::"d0","a0","d1");
    // Init Controller ports 
    asm("move.b #0, 0x000a10009\n\t"
    "move.b #0, 0x000a1000b\n\t"
    "move.b #0, 0x000a1000d":::);
    // Clear and go
    asm("move.l #0, %%a0\n\t"
    "movem.l (%%a0), %%d0-%%d7/%%a1-%%a7\n\t"
    "move #0x2700, %%SR\n\t":::"a0", "d0", "d7", "a1", "sp");
    //sp
    asm("movea.l #0xfffffe00, %a7"); 
    main(); 
}

void SetVDPPlaneAddress(u8 plane, u16 addr);
void SetVDPAddress(u16 address);
/*
void WaitVBlank();
void WaitVBlank()
{
    asm volatile("VB%=: move.w (0xc00004).l,%%d0\n\t"
        "btst #3,%%d0\n\t"
        "beq VB%=\n\t"
    :
    :
    :"d0");
    asm volatile("VB%=: move.w (0xc00004).l,%%d0\n\t"
        "btst #3,%%d0\n\t"
        "bne VB%=\n\t"
    :
    :
    :"d0");
}
*/

#define WriteVDPRegister(v) asm("move.w %0,(0xC00004).l"::"g"(v):)

#define VDPStatus_u16(var) \
    asm("move.w (0xc00004).l,%0":"=g"(var)::)
#define WaitVBlank() \
    asm volatile("VB%=: move.w (0xc00004).l,%%d0\n\t" \
        "btst #3,%%d0\n\t" \
        "beq VB%=" \
    : \
    : \
    :"d0"); \
    asm volatile("VB%=: move.w (0xc00004).l,%%d0\n\t" \
        "btst #3,%%d0\n\t" \
        "bne VB%=" \
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


volatile u32* VDP_STATUSREG = (volatile u32*)0xc00004;


void SetVDPAddress(u16 address)
{
    u32 loc = 0x40000000 + ((address & 0x3fff) << 16) + ((address & 0xc000) >> 14);
    asm("move.l %0,(0xc00004).l"
    :
    :"g"(loc)
    :);
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
    : "g"(f)
    :);
}
