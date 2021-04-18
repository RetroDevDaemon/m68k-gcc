//main.c
#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800

#define BG_WIDTH 64
#define BG_HEIGHT 32

void __attribute__((optimize("O3"))) GAME_DRAW();

#include "bentgen.h"

// Tile format is: e.g. 0x0077BB77
// 2 Pixels per byte, left to right, color 0-f. Easy to plot
#include "font.h"
#include "palette.h"
#include "player1.h"
#include "playership_pal.h"

// game function defs 
void __attribute__((optimize("Os"))) main();
void GAME_INPUT();
void ProcessInput();
Sprite* AddSprite(u16 ypos, u8 size, u16 attr, u16 xpos);
u16 strsize(String* s);

// MUST BE ram CONST POINTER! idk why
//ram u16* pals[4];
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
static u16 joyState1;
static u16 last_joyState1;
static u16 joyState2;
static u16 last_joyState2;
    
static Sprite activeSprites[80];
//static struct bullet* playerBullets[40];
u8 num_p_bullets = 0;

struct bullet { 
    signed char timer;
    Sprite* myspr;
};

bool REORDER_SPRITES = false;
u8 sprites_destroyed = 0;

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

Sprite* AddSpriteAt(u16 ypos, u8 size, u16 attr, u16 xpos, u8 spriteNum)
{
    activeSprites[spriteNum].y_pos = ypos;
    activeSprites[spriteNum].size = size; // use macro 
    //if(NUM_SPRITES > 0) activeSprites[NUM_SPRITES-1].next = NUM_SPRITES;
    activeSprites[spriteNum].next = spriteNum + 1;
    activeSprites[spriteNum].spr_attr = attr;
    activeSprites[spriteNum].x_pos = xpos;
    //NUM_SPRITES++;
    return &activeSprites[spriteNum];
}

void main()
{   
    LoadPalette(1, (u16*)&playership_pal);
    LoadPalette(0, (u16*)&palette);
    
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
        cr = (u32*)&font_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
    // + c00 = 1000

    // Copy in ship
    SetVRAMWriteAddress(0x1000);
    for(c = 0; c < (12*8); c++) {
        cr = (u32*)&player1_0;
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
        u8 lastEmpty = 0;
        // every time a sprite is changed, search through SAT RAM and 
        //  find the first spr_attr = 0. mark this as the next sprite.
        //  -- don't unlink sprites! just move them off-screen! --
        ProcessInput();     // Process last frame's buttons
        for(i = 0; i < 40; i++) // BULLET HELL!
        { 
            activeSprites[i+2].x_pos += 18;
            if(activeSprites[i+2].x_pos > 470) {
                activeSprites[i+2].x_pos = p1ship->x_pos;
                activeSprites[i+2].y_pos = p1ship->y_pos;
            }
        }

        WaitVBlank();       // Wait until draw is done
        GAME_INPUT();       // get fresh joy states
    }
}

// how to remove a sprite:
// (1. change link order if priority is an issue)
// 2. set y=350, attr and size to 0
//      AND decrement sprite counter
// 3. set REORDER_SPRITES flag to true
// 4. next loop, reorder all sprite ram - if attr != 0 then its a valid sprite
//    copy entire spr data backwards to the last empty location - minus link
void RemoveSprite(Sprite* spr)
{
    spr->y_pos = 350;
    spr->spr_attr = 0;
    spr->size = 0;
    NUM_SPRITES--;
    REORDER_SPRITES = true;
}

 /* old loop: Do nothing!!
frameCounter++;
WaitHBlank();
VDPStatus_u16(vdpstat);
hcount++;
    */

void ProcessInput()
{
    if(joyState1 & BTN_A_PRESSED) { // Fire test
        if(num_p_bullets < 40)
        {
            AddSprite(p1ship->y_pos,\
                SPRSIZE(2,2), \
                SPR_ATTR(0x31, 0, 0, 0, 0), \
                p1ship->x_pos);
            num_p_bullets++;
            //playerBullets[num_p_bullets]->timer = 15;
        }
        
    }
    if(joyState1 & BTN_UP_PRESSED) // Up
    {
        p1ship->y_pos -= pSpeed;
        test->y_pos -= pSpeed;
    }
    if(joyState1 & BTN_DOWN_PRESSED) // Down
    {
        p1ship->y_pos += pSpeed;
        test->y_pos += pSpeed;
    }
    if(joyState1 & BTN_LEFT_PRESSED) // Left
    {
        p1ship->x_pos -= pSpeed;
        test->x_pos -= pSpeed;
    }
    if(joyState1 & BTN_RIGHT_PRESSED) // Right
    {
        p1ship->x_pos += pSpeed;
        test->x_pos += pSpeed;
    }
}

// Called during VBlank
void GAME_DRAW()
{   
    // TODO: Convert this to DMA
    u32* spr = (u32*)&activeSprites[0];
    SetVRAMWriteAddress(VRAM_SAT);
    for(u8 sl = 0; sl < (2 * 80); sl++) WRITE_DATAREG32(*spr++);
}

void GAME_INPUT()
{
    last_joyState1 = joyState1;
    GETJOYSTATE1(joyState1);
    last_joyState2 = joyState2;
    GETJOYSTATE2(joyState2);
}

u16 strsize(String* s)
{
    u16 sz;
    while(*s != '\00') sz++;
    return sz;
}
