
#include "bentgen.h"

u32 frameCounter;
u16 vdpstat;
u32 hcount;

static const u16 palette[16] = {
    0x0000, 0x000e, 0x00e0, 0x0e00,
    0x0000, 0x0eee, 0x00ee, 0x008e,
    0x0e0e, 0x0808, 0x0444, 0x0888,
    0x0ee0, 0x000a, 0x0600, 0x0060
};
static const u16 palette2[16] = {
    0x0000, 0x000e, 0x00e0, 0x0e00,
    0x0000, 0x0eee, 0x00ee, 0x008e,
    0x0e0e, 0x0808, 0x0444, 0x0888,
    0x0ee0, 0x000a, 0x0600, 0x0060
};
static const u16 palette3[16] = {
    0x0000, 0x000e, 0x00e0, 0x0e00,
    0x0000, 0x0eee, 0x00ee, 0x008e,
    0x0e0e, 0x0808, 0x0444, 0x0888,
    0x0ee0, 0x000a, 0x0600, 0x0060
};
static const u16 palette4[16] = { // const cannot guaruntee its location.
    0x0000, 0x0eee, 0x0eee, 0x0eee,
    0x0eee, 0x0eee, 0x0eee, 0x0eee,
    0x0eee, 0x0eee, 0x0eee, 0x0eee,
    0x0eee, 0x0eea, 0x0eee, 0x0eee
};

// MUST BE STATIC CONST POINTER! idk why
static const u16* pals[4];
    
void main()
{   

    LoadPalette(0, &palette);
    LoadPalette(1, &palette4);

    pals[0] = &palette;
    pals[1] = &palette4;
    LoadPalette(2, pals[0]);
    LoadPalette(3, pals[1]);

    u8 fq;
    while(1)
    { 
        WriteVDPRegister((u32)WRITE|REG(7)|fq);
        fq++;
        u8 i;
        for(i = 0; i < 10; i++) WaitVBlank();
        if(fq > 63) fq = 0;
        
        /*
        frameCounter++;
        WaitHBlank();
        VDPStatus_u16(vdpstat);
        hcount++;
        */
    }
}
