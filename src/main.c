
#include "bentgen.h"

u32 frameCounter;
u16 vdpstat;
u32 hcount;

#define BG_A 0 
#define BG_B 1 
#define SPRITE 2 
#define WINDOW 3
#define CRAM_ADDR (u32)(0xC0000003)
volatile u16* VDP_DATAREG = (u16*)0x00c00000;
volatile u32* VDP_CTRLREG = (u32*)0x00c00004;

const u16 palette[16] = {
    0x0000, 0x000e, 0x00e0, 0x0e00,
    0x0000, 0x0eee, 0x00ee, 0x008e,
    0x0e0e, 0x0808, 0x0444, 0x0888,
    0x0ee0, 0x000a, 0x0600, 0x0060
};

void main()
{   

    /*    
    WriteVDPRegister((u16)WRITE|REG(0)|0);
    
    SetVDPPlaneAddress(BG_A, 0x2000);
    SetVDPPlaneAddress(BG_B, 0x4000);
    SetVDPPlaneAddress(SPRITE, 0x6000);
    SetVDPPlaneAddress(WINDOW, 0x7000);
    
    //SetVDPAddress(0x2000);
    */
    
    /*
    *VDP_CTRLREG = CRAM_ADDR;
    for(u8 i = 0; i < 16; i++) *VDP_DATAREG = palette[i];
    WriteVDPRegister((u16)0x8708);
    */
   
    WriteVDPRegister((u16)WRITE|REG(0xf)|2); // auto-increment
    asm("move.l #0xc0000003, 0x00c00004\n\t"
    "lea palette, %%a0\n\t"
    "move.l #0x07, %%d0\n\t"
    "mvploop: move.l (%%a0)+, 0x00c00000\n\t"
    "dbra %%d0, mvploop":::"a0", "d0");
   
    WriteVDPRegister((u32)WRITE|REG(0x7)|8);
    while(1)
    { 
        /*
        WaitVBlank();
        frameCounter++;
        WaitHBlank();
        VDPStatus_u16(vdpstat);
        hcount++;
        */
        
    }
}
