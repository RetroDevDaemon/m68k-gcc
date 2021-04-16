//main.c
#include "bentgen.h"

//#define static static const

// Tile format is: e.g. 0x0077BB77
// 2 Pixels per byte, left to right, color 0-f. Easy to plot
#include "font.h"
#include "palette.h"
#include "player1.h"
#include "playership_pal.h"

#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800


// MUST BE STATIC CONST POINTER! idk why
static u16* pals[4];
u8 fq;
u32 frameCounter;
u16 vdpstat;
u32 hcount;
String hw = "Hello World!";
String hw2 = "Lemme GeT DaT SWETET SWEEt";
String hw3 = "BLasT PROceSSIN!!!@@";

struct SpriteAttribute { 
    u16 y_pos;
    u8 size;
    u8 next;
    u16 tile_attr;
    u16 x_pos;
};
static struct SpriteAttribute p1t;
static struct SpriteAttribute p2t;

void GAME_DRAW();

void main()
{   
    //LoadPalette(0, &playership_pal);
    LoadPalette(1, &playership_pal);
    LoadPalette(0, &palette);
    
    u8 i;
    u16 c = 0; 
    u32* cr;
    // Copy in our font!
    // Tile number 32 (start of ascii table) * 32 bytes per tile = 1024 = $400
    SetVRAMWriteAddress(0x400);
    for(c = 0; c < 0x300; c++) {
        cr = &font_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
    // + c00 = 1000

    // Copy in ship
    SetVRAMWriteAddress(0x1000);
    for(c = 0; c < (12*8); c++) {
        cr = &player1_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }

    u8 r = 0;
    // set ship as 4x3 sprite
    // size: E. 8 bytes
    // 0-1: Y position (9 bits, 0-511, or 0-1023 in double mode)
    // 2: 'next' sprite. final sprite = 0
    // 3: bits 0-1 Y size, bits 2-3 X size.
    //    0xE => X=4 Y=3
    // 4-5: tile number(10 bits, 0-2047) | hf(1<<11) | vf(1<<12) | pal(2<<13) | priority(1<<15)
    // 6-7: X position (8 bits, 0-511)
    p1t.y_pos = 200;
    p1t.next = 0;
    p1t.size = 0xe;
    p1t.tile_attr = 128 | (1 << 13);
    p1t.x_pos = 200;
    // Write it in
    u32* mp = 0xff2000;
    u32* loadptr = &p1t;
    SetVRAMWriteAddress(VRAM_SAT);
    WRITE_DATAREG32(*loadptr++);
    WRITE_DATAREG32(*loadptr++);
    
    // test - read SAT #0 from vram and write to ram
    SetVRAMReadAddress(VRAM_SAT);
    READ_DATAREG32(*mp++);
    SetVRAMReadAddress(VRAM_SAT + 4);
    READ_DATAREG32(*mp++);

    // BG plane A
    SetVRAMWriteAddress(VRAM_BG_A);
    for(i = 0; i < 13; i++) WRITE_DATAREG16(hw[i]);
    SetVRAMWriteAddress(VRAM_BG_A + (0x41 * 2)); 
    // 2 bytes per character, 64 chars per plane row * 2 = 128 or $80 for newline
    for(i = 0; i < 27; i++) WRITE_DATAREG16(hw2[i]);
    SetVRAMWriteAddress(VRAM_BG_A + (0x82 * 2));
    for(i = 0; i < 21; i++) WRITE_DATAREG16(hw3[i]);

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
    SetVRAMWriteAddress(VRAM_SAT);
    WRITE_DATAREG16(p1t.y_pos++);
    SetVRAMWriteAddress(VRAM_SAT + 6);    // sprite 0's X pos
    WRITE_DATAREG16(p1t.x_pos);   // write and increment
    p1t.x_pos += 2;
    
}
