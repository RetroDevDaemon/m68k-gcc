//main.c
#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800

#define BG_WIDTH 64
#define BG_HEIGHT 32

inline void GAME_DRAW();

#include "bentgen.h"

//#define static static const
// Tile format is: e.g. 0x0077BB77
// 2 Pixels per byte, left to right, color 0-f. Easy to plot
#include "font.h"
#include "palette.h"
#include "player1.h"
#include "playership_pal.h"

// game function defs 

inline void GAME_INPUT();
Sprite* AddSprite(u16 ypos, u8 size, u16 attr, u16 xpos);
u16 strsize(String* s);

// MUST BE STATIC CONST POINTER! idk why
//static u16* pals[4];
u8 fq;
u8 pSpeed;
u32 frameCounter;
u16 vdpstat;
u32 hcount;
String hw = "Hello World!";
String hw2 = "Lemme GeT DaT SWETET SWEEt";
String hw3 = "BLasT PROceSSIN!!!@@";
String kb = "UDLR Ss ABC";

Sprite* p1ship;
Sprite* test;
u8 NUM_SPRITES;
u32* spriteRamPtr;
u16 joyState;
    
static Sprite activeSprites[80];

Sprite* AddSprite(u16 ypos, u8 size, u16 attr, u16 xpos)
{
    activeSprites[NUM_SPRITES].y_pos = ypos;
    activeSprites[NUM_SPRITES].size = size; // use macro 
    if(NUM_SPRITES > 0) activeSprites[NUM_SPRITES-1].next = NUM_SPRITES;
    activeSprites[NUM_SPRITES].next = 0;
    activeSprites[NUM_SPRITES].spr_attr = attr;
    activeSprites[NUM_SPRITES].x_pos = xpos;
    NUM_SPRITES++;
    return &activeSprites[NUM_SPRITES-1];
}

void main()
{   
    LoadPalette(1, &playership_pal);
    LoadPalette(0, &palette);
    
    u8 i = 0;
    u16 c = 0; 
    u32* cr;
    u8 r = 0;
    pSpeed = 3;
    NUM_SPRITES = 0;

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

    // set ship as 4x3 sprite
    // size: E. 8 bytes
    p1ship = AddSprite(200, SPRSIZE(4,3), SPR_ATTR(128, 0, 0, 1, 0), 200);
    test = AddSprite(250, SPRSIZE(4,3), SPR_ATTR(128, 0, 0, 0, 0), 250);
    
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
        //GAME_INPUT();

        WaitVBlank();
    }
}

// Called during VBlank
void __attribute__((optimize("O3"))) GAME_DRAW()
{   
    GAME_INPUT();
    // TODO: Convert this to DMA
    u32* spr = &activeSprites[0];
    SetVRAMWriteAddress(VRAM_SAT);
    for(u8 sl = 0; sl < 2 * NUM_SPRITES; sl++) WRITE_DATAREG32(*spr++);
}

void GAME_INPUT()
{
    joyState = null;
    GETJOYSTATE1(joyState);

    if(joyState & bit(0)) // Up
    {
        p1ship->y_pos -= pSpeed;
        test->y_pos -= pSpeed;
    }
    if(joyState & bit(1)) // Down
    {
        p1ship->y_pos += pSpeed;
        test->y_pos += pSpeed;
    }
    //if(joyState & bit(4)) // A
    if(joyState & bit(10)) // Left
    {
        p1ship->x_pos -= pSpeed;
        test->x_pos -= pSpeed;
    }
    //if(joyState & bit(5)) // St
    if(joyState & bit(11)) // Right
    {
        p1ship->x_pos += pSpeed;
        test->x_pos += pSpeed;
    }
    // bit(12) B 
    // bit(13) C
}

u16 strsize(String* s)
{
    u16 sz;
    while(*s != '\00') sz++;
    return sz;
}
