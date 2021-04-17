//main.c
#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800

#define BG_WIDTH 64
#define BG_HEIGHT 32

#include "bentgen.h"

//#define static static const
// Tile format is: e.g. 0x0077BB77
// 2 Pixels per byte, left to right, color 0-f. Easy to plot
#include "font.h"
#include "palette.h"
#include "player1.h"
#include "playership_pal.h"

// MUST BE STATIC CONST POINTER! idk why
//static u16* pals[4];
u8 fq;
u32 frameCounter;
u16 vdpstat;
u32 hcount;
String hw = "Hello World!";
String hw2 = "Lemme GeT DaT SWETET SWEEt";
String hw3 = "BLasT PROceSSIN!!!@@";

static SpriteAttribute p1t;
static SpriteAttribute p2t;

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
    p1t.y_pos = 200;
    p1t.size = SPRSIZE(4,3);
    p1t.next = 0;
    p1t.spr_attr = SPR_ATTR(128, 0, 0, 1, 0);
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
    print(BG_A, 0, 0, hw);
    print(BG_A, 1, 1, hw2);
    print(BG_A, 2, 2, hw3);
        
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
        p1t.y_pos++;
        p1t.x_pos += 2;
        WaitVBlank();
    }
}

// Called during VBlank
void GAME_DRAW()
{   
    u32* spr = &p1t;
    SetVRAMWriteAddress(VRAM_SAT);
    WRITE_DATAREG32(*spr++);
    WRITE_DATAREG32(*spr++);
    
}

u16 strsize(String* s)
{
    u16 sz;
    while(*s != '\00') sz++;
    return sz;
}


