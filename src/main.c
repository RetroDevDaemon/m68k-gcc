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
#define SPRITE_RAM_BASE 0xfff500

static Sprite activeSprites[80];

void GAME_DRAW();

Sprite* AddSprite(u16 ypos, u8 size, u16 attr, u16 xpos)
{
    activeSprites[NUM_SPRITES].y_pos = ypos;
    activeSprites[NUM_SPRITES].size = size; // use macro 
    activeSprites[NUM_SPRITES-1].next = NUM_SPRITES;
    activeSprites[NUM_SPRITES].next = 0;
    activeSprites[NUM_SPRITES].spr_attr = attr;
    activeSprites[NUM_SPRITES].x_pos = xpos;
    NUM_SPRITES++;
    return &activeSprites[NUM_SPRITES-1];
}

void main()
{   
    NUM_SPRITES = 0;
    spriteRamPtr = SPRITE_RAM_BASE; // 2560, or 0xa00
    //LoadPalette(0, &playership_pal);
    LoadPalette(1, &playership_pal);
    LoadPalette(0, &palette);
    
    u8 i = 0;
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
    p1ship = AddSprite(200, SPRSIZE(4,3), SPR_ATTR(128, 0, 0, 1, 0), 200);
    test = AddSprite(250, SPRSIZE(4,3), SPR_ATTR(128, 0, 0, 0, 0), 250);
    
    // BG plane A
    print(BG_A, 0, 0, hw);
    print(BG_A, 1, 1, hw2);
    print(BG_A, 2, 2, hw3);
        
    // Enable VBlank on VDP 
    WriteVDPRegister(WRITE|REG(1)|0x64);
    pSpeed = 3;

    u16 joyState = 0;    
    while(1)
    { 
        /* Do nothing!!
        frameCounter++;
        WaitHBlank();
        VDPStatus_u16(vdpstat);
        hcount++;
        */
        joyState = null;
        GETJOYSTATE1(joyState);
        String aff = "A";

        if(joyState & BTN_UP_PRESSED) 
        {
            p1ship->y_pos -= pSpeed;
            test->y_pos -= pSpeed;
        }
        if(joyState & BTN_DOWN_PRESSED) 
        {
            p1ship->y_pos += pSpeed;
            test->y_pos += pSpeed;
        }
        if(joyState & BTN_LEFT_PRESSED) 
        {
            p1ship->x_pos -= pSpeed;
            test->x_pos -= pSpeed;
        }
        if(joyState & BTN_RIGHT_PRESSED) 
        {
            p1ship->x_pos += pSpeed;
            test->x_pos += pSpeed;
        }
        //if(joyState & BTN_A_PRESSED) p1t.x_pos += pSpeed;

        // copy sprites into the DMA buffer
        spriteRamPtr = SPRITE_RAM_BASE;
        u32* loadptr = &activeSprites[0];
        for(u8 e = 0; e < NUM_SPRITES; e++) 
            for(r = 0; r < 2 * NUM_SPRITES; r++) 
                *spriteRamPtr++ = loadptr[r];

        WaitVBlank();
    }
}

// Called during VBlank
void GAME_DRAW()
{   
    // TODO: Convert this to DMA
    u32* spr = SPRITE_RAM_BASE;
    SetVRAMWriteAddress(VRAM_SAT);
    for(u8 sl = 0; sl < 80; sl++) WRITE_DATAREG32(*spr++);
}

u16 strsize(String* s)
{
    u16 sz;
    while(*s != '\00') sz++;
    return sz;
}
