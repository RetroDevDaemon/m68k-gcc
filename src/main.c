
#include "bentgen.h"

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
