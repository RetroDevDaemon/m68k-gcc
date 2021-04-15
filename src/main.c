//main.c
#include "bentgen.h"
#include "palette-test.h"
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
#include "tiles.h"

// MUST BE STATIC CONST POINTER! idk why
static const u16* pals[4];
u8 fq;
u32 frameCounter;
u16 vdpstat;
u32 hcount;

void GAME_DRAW();

void main()
{   
    LoadPalette(0, &palette);
    LoadPalette(1, &palette4);

    pals[0] = &palette;
    pals[1] = &palette4;
    LoadPalette(2, pals[0]);
    LoadPalette(3, pals[1]);

    u8 i;
    int c = 0; 
    // Tile number 32 (start of ascii table) * 32 bytes per tile = 1024 = $400
    SetVRAMWriteAddress(0x400);
    for(c = 0; c < 0x300; c++) {
        u32* cr = &tile_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }

    // BG plane A
    SetVRAMWriteAddress(0xc000);
    const char hw[12]="Hello World!";
    for(i = 0; i < 12; i++) WRITE_DATAREG16(hw[i]);

    // Enable VBlank on VDP 
    WriteVDPRegister(WRITE|REG(1)|0x64);

    while(1)
    { 
        /* Do nothing!!
        frameCounter++;
        WaitHBlank();
        VDPStatus_u16(vdpstat);
        hcount++;
        */
    }
}

// Called during VBlank
void GAME_DRAW()
{
    WriteVDPRegister((u32)WRITE|REG(7)|fq);
    fq++;
    if(fq > 63) fq = 0;
}
