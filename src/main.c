
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
    main(); 
}

void SetVDPPlaneAddress(u8 plane, u16 addr);
void SetVDPAddress(u16 address);
/*void WaitVBlank();
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

#define WriteVDPRegister(v) asm("move.w %0,(0xC00004).l"::"g"(v):);

#define WaitVBlank() asm volatile("VB%=: move.w (0xc00004).l,%%d0\n\t" \
        "btst #3,%%d0\n\t" \
        "beq VB%=\n\t" \
    : \
    : \
    :"d0"); asm volatile("VB%=: move.w (0xc00004).l,%%d0\n\t" \
        "btst #3,%%d0\n\t" \
        "bne VB%=\n\t" \
    : \
    : \
    :"d0");

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

void main()
{
    WriteVDPRegister((u32)WRITE|REG(0)|0);
    WriteVDPRegister((u32)WRITE|REG(0x7)|1);
    SetVDPPlaneAddress(0, 0x2000);
    SetVDPPlaneAddress(1, 0x4000);
    SetVDPPlaneAddress(2, 0x6000);
    SetVDPPlaneAddress(3, 0x7000);
    SetVDPAddress(0x2000);
    while(1)
    { 
        WaitVBlank();
        // Loop
    }
}

