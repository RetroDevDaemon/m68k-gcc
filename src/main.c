//main.c
#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800
#define VRAM_SCROLL 0xfc00

#define BG_WIDTH 64
#define BG_HEIGHT 32

#include "bentgen.h"

// Tile format is: e.g. 0x0077BB77
// 2 Pixels per byte, left to right, color 0-f. Easy to plot
#include "gfx.h"

#define Joy1Down(b) (bool)\
    (!(last_joyState1 & b) && \
    (joyState1 & b))
#define Joy1Up(b) (bool)\
    ((last_joyState1 & b) &&\
    !(joyState1 & b))
#define Joy2Down(b) (bool)\
    (!(last_joyState2 & b) && \
    (joyState2 & b))
#define Joy2Up(b) (bool)\
    ((last_joyState2 & b) &&\
    !(joyState2 & b))
#define elseif else if 

// game function defs 
void main();
void ProcessInput();

// Global Vars
u8 fq;
u8 pSpeed;
u32 frameCounter;
bool frameFlip;
u16 vdpstat;
u32 hcount;
String hw = "Hello World!";
String hw2 = "Lemme GeT DaT SWETET SWEEt";
String hw3 = "BLasT PROceSSIN!!!@@";
String kb = "UDLR Ss ABC";

static u16 joyState1;
static u16 last_joyState1;
static u16 joyState2;
static u16 last_joyState2;

bool REORDER_SPRITES = false;
u8 sprites_destroyed = 0;
     
// Sprite definitions
static Sprite empty[80];

u8 sixtyFrameCounter = 0;
u8 twentyFrameCounter = 0;
u8 tenFrameCounter = 0;
Sprite* test;
u8 NUM_SPRITES;
u32* spriteRamPtr;

// contains the tile_attr 
u16 bg_a_map[64*32];

// Players - 2
// player bullets - 19 each (flicker=38 each)
// enemies - 10
// enemy bullets - 30 (flicker = 60)
// Players 1
// player bullets - 29 (flicker=58)
// enemies - 15
// e. bullets - 35 (flicker = 70)

void __attribute__((optimize("Os"))) stdcpy(u32* src, u32* dst, u32 siz)
{
    for(u32 cs = 0; cs < siz; cs++) *dst++ = *src++;
}


void main()
{   
    WaitVBlank();
    //stdcpy(0x00ff0000, 0x00ff0001, 100);
    spriteRamBase = &empty[0];
    LinkAllSpriteData();
    LoadPalette(0, (u16*)&palette);

    u8 i = 0;
    u16 c = 0; 
    u32* cr;
    u8 r = 0;
    pSpeed = 3;
    NUM_SPRITES = 0;

/// INITIALIZE VRAM GRAPHICS ///
    // 0-400h is empty for now
    // Copy in our font!
    // Tile number 32 (start of ascii table) * 32 bytes per tile = 1024 = $400
    //WriteVDPRegister(WRITE|REG(0xf)|4);
#define ASCIIBASETILE 32
    SetVRAMWriteAddress(0x400);
    for(c = 0; c < 0x300; c++) 
    {
        cr = (u32*)&font_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
    SetVRAMWriteAddress(0xc000);
    WRITE_DATAREG16("A");
    // + c00 = 1000
///
    //AddSprite(&moon, 260, SPRSIZE(4,4), SPR_ATTR(SUNTILE, 0, 0, 0, 0), 350);
    // draw map test
    // FRONT LAYER
    //DrawTile(BG_A, TILEATTR(STAGE1BGBTMTILE, 0, 0, 0, 0), 0, 19, 27, 8);
    // BG LAYER
#define STAGE1BGWID 376/8
#define STAGE1BGHEI 256/8
    //for(u8 y = 0; y < STAGE1BGHEI; y++){
    //    SetVRAMWriteAddress(VRAM_BG_B + (0x80 * y));
    //    for(c = 0; c < STAGE1BGWID; c++) {
    //        WRITE_DATAREG16((u16)(STAGE1BGATILE + stage1_bga_b_map[(y*STAGE1BGWID)+c])|(pal_no(2)));
    //    }
   // }
    // set ship as 4x3 sprite
    // size: E. 8 byte
#define PLAYER1_SPR 184
   // p1ship[0] = AddSprite(&player1sprite[0], 200, SPRSIZE(4,4), SPR_ATTR(PLAYER1_SPR, 0, 0, 0, 0), 200);

    // Enable VBlank on VDP 
    WriteVDPRegister(WRITE|REG(1)|0x64);

    bgb_vscroll_pos = 0;//p1ship[0]->y_pos / 16;
    bgb_hscroll_pos += 0;


    while(1)
    { 
        WaitVBlank();       // Wait until draw is done
        // MAIN GAME LOOP 
        ProcessInput();     // Process last frame's buttons

        // Counter-y stuff
        twentyFrameCounter++;
        if(twentyFrameCounter > 19) twentyFrameCounter = 0;
        tenFrameCounter++;
        if(tenFrameCounter > 9) tenFrameCounter = 0;
        

    }
}

// Called during VBlank
void GAME_DRAW()
{   
    if(frameFlip == 0) frameFlip = 1;
    else frameFlip = 0;

    last_joyState1 = joyState1;
    GETJOYSTATE1(joyState1);
    last_joyState2 = joyState2;
    GETJOYSTATE2(joyState2);
    // Sprite shit
    // TODO: Convert this to DMA
    volatile u32* spr = spriteRamBase;
    u8 i = 0;
    //SetVRAMWriteAddress(VRAM_SAT);
    // player1
    //for(i = 0; i < 4 * 2; i++) WRITE_DATAREG32(*spr++);

    // write scroll ram
    UpdateBGScroll();
}


void ProcessInput()
{
    u8 ps = 0;
    if(joyState1 & BTN_UP_PRESSED) // Up
    {

    }
    if(joyState1 & BTN_DOWN_PRESSED) // Down
    {

    }
    if(joyState1 & BTN_LEFT_PRESSED) // Left
    {

    }
    if(joyState1 & BTN_RIGHT_PRESSED) // Right
    {

    }
    if(joyState1 & BTN_A_PRESSED) 
    {   // Fire test

    }

    // FOCUS MODE
    if(Joy1Down(BTN_B_PRESSED))
    {

    }
    else if(Joy1Up(BTN_B_PRESSED))
    {

    }

    // SWITCH FACING
    if(Joy1Down(BTN_C_PRESSED))
    {

    }
}
