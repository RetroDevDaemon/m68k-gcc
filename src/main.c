

#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800

#define BG_WIDTH 64
#define BG_HEIGHT 32

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

// 2 Pixels per byte, left to right, color 0-f. Easy to plot:
static const u32 spritetest[8] = {
    0x01234567,
    0x12345678, 
    0x23456789, 
    0x3456789a, 
    0x456789ab, 
    0x56789abc, 
    0x6789abcd,
    0x789abcde
};
static const u32 letterA[8] = { 
    0x00055000,
    0x00555500,
    0x05500550,
    0x05500550,
    0x05555550,
    0x05500550,
    0x05500550,
    0x00000000
};

// MUST BE STATIC CONST POINTER! idk why
static const u16* pals[4];
u8 fq;

void main()
{   

    LoadPalette(0, &palette);
    LoadPalette(1, &palette4);

    pals[0] = &palette;
    pals[1] = &palette4;
    LoadPalette(2, pals[0]);
    LoadPalette(3, pals[1]);
    // Enable VBlank on VDP 
    WriteVDPRegister(WRITE|REG(1)|0x64);
    
    while(1)
    { 
        /*
        frameCounter++;
        WaitHBlank();
        VDPStatus_u16(vdpstat);
        hcount++;
        */
    }
}

void GAME_DRAW()
{
    WriteVDPRegister((u32)WRITE|REG(7)|fq);
    fq++;
    if(fq > 63) fq = 0;
}
