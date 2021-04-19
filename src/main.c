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
#include "pbullet1.h"

// game function defs 
void __attribute__((optimize("Os"))) main();
void GAME_INPUT();

void ProcessInput();
Sprite* AddSprite(Sprite* as, u16 ypos, u8 size, u16 attr, u16 xpos);
u16 strsize(String* s);
void LinkAllSpriteData();

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
static Sprite player1sprite;
const void* spriteRamBase = &player1sprite;
static Sprite playerBullets[29];
static Sprite enemySprites[15];
static Sprite enemyBullets[35];
// 80 = end of active sprite data
static Sprite* p1ship;
static Sprite* playerBulletPtrs[29*2];
static Sprite playerBulletsTwo[29];

u8 num_p_bullets = 0;

Sprite* test;
u8 NUM_SPRITES;
u32* spriteRamPtr;

// Players - 2
// player bullets - 19 each (flicker=38 each)
// enemies - 10
// enemy bullets - 30 (flicker = 60)
// Players 1
// player bullets - 29 (flicker=58)
// enemies - 15
// e. bullets - 35 (flicker = 70)
struct bullet 
{ 
    signed char timer;
    Sprite* myspr;
};


void main()
{   
    spriteRamBase = &player1sprite;
    LinkAllSpriteData();
    LoadPalette(1, (u16*)&playership_pal);
    LoadPalette(0, (u16*)&palette);
    
    u8 i = 0;
    u16 c = 0; 
    u32* cr;
    u8 r = 0;
    pSpeed = 3;
    NUM_SPRITES = 0;

/// INITIALIZE VRAM GRAPHICS ///
    // Copy in our font!
    // Tile number 32 (start of ascii table) * 32 bytes per tile = 1024 = $400
    SetVRAMWriteAddress(0x400);
    for(c = 0; c < 0x300; c++) 
    {
        cr = (u32*)&font_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
    // + c00 = 1000
    // Copy in ship
    SetVRAMWriteAddress(0x1000); // 128
    for(c = 0; c < (12*8); c++) 
    {
        cr = (u32*)&player1_0;  
        cr += c;
        WRITE_DATAREG32(*cr);
    }
    // Copy in bullet - ID 140
    for(c = 0; c < (8 * 4); c++) {
        cr = (u32*)&pbullet1_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
///

    // set ship as 4x3 sprite
    // size: E. 8 bytes
    p1ship = AddSprite(&player1sprite, 200, SPRSIZE(4,3), SPR_ATTR(128, 0, 0, 1, 0), 200);
    test = AddSprite(&enemySprites[0], 250, SPRSIZE(4,3), SPR_ATTR(128, 0, 0, 0, 0), 250);
    
    // BG plane A
    print(BG_A, 0, 0, hw);
    print(BG_A, 1, 1, hw2);
    print(BG_A, 2, 2, hw3);

#define PLAYER_BULLET_MAX 29
        
    // clear bullets
    for(i = 0; i < PLAYER_BULLET_MAX*2; i++) playerBulletPtrs[i] = null;

    // Enable VBlank on VDP 
    WriteVDPRegister(WRITE|REG(1)|0x64);
    num_p_bullets = 0;
    while(1)
    { 
        WaitVBlank();       // Wait until draw is done
        
        // MAIN GAME LOOP 
        ProcessInput();     // Process last frame's buttons

        // BULLET HELL!
        for(i = 0; i < PLAYER_BULLET_MAX * 2; i++)
        { 
            if(playerBulletPtrs[i]->spr_attr != 0)
            {
                //process bullet's code
                playerBulletPtrs[i]->x_pos += 16;
                // out of bounds?
                if((playerBulletPtrs[i]->x_pos > p1ship->x_pos + 320) \
                || (playerBulletPtrs[i]->x_pos > 490))
                {
                    // ...destroy the bullet
                    num_p_bullets--;
                    playerBulletPtrs[i]->y_pos = 0;
                    playerBulletPtrs[i]->spr_attr = 0;
                    playerBulletPtrs[i]->size = 0;
                    playerBulletPtrs[i] = null;
                }
            }
        }
    }
}

// Called during VBlank
void GAME_DRAW()
{   
    if(frameFlip == 0) frameFlip = 1;
    else frameFlip = 0;
    //frameFlip = 1;
    GAME_INPUT();       // get fresh joy states
    // TODO: Convert this to DMA

    u32* spr = spriteRamBase;
    SetVRAMWriteAddress(VRAM_SAT);
    // player1
    WRITE_DATAREG32(*spr++);
    WRITE_DATAREG32(*spr++); 
    // bullet flicker
    if(frameFlip) spr = (u32*)&playerBullets;
    else spr = (u32*)&playerBulletsTwo;
    for(u8 i = 0; i < PLAYER_BULLET_MAX*2; i++) WRITE_DATAREG32(*spr++);
}

void GAME_INPUT()
{
    last_joyState1 = joyState1;
    GETJOYSTATE1(joyState1);
    last_joyState2 = joyState2;
    GETJOYSTATE2(joyState2);
}

 /* old loop: Do nothing!!
frameCounter++;
WaitHBlank();
VDPStatus_u16(vdpstat);
hcount++;
    */

Sprite* AddSprite(Sprite* as, u16 ypos, u8 size, u16 attr, u16 xpos)
{
    as->y_pos = ypos;
    as->size = size;     // use SPRSIZE macro 
    as->spr_attr = attr;
    as->x_pos = xpos;
    return as;
}

void LinkAllSpriteData()
{
    Sprite* s = (Sprite*)spriteRamBase;
    // Set all 80 sprites to linear draw/link order
    for(u8 i = 0; i < 80; i++) 
    {
        s[i].spr_attr = null;   // set to blank sprite
        s[i].next = i + 1;
        s[i].x_pos = 0;
        s[i].y_pos = 0;
        s[i].size = 0;
    }
    s[79].next = 0;     // final sprite must link to 0
    // UH OH! Link these fools!
    s = (Sprite*)&playerBullets;
    for(u8 i = 0; i < PLAYER_BULLET_MAX; i++){
        playerBulletsTwo[i].next = s[i].next;
    }
}

u8 FindFirstEmptyBullet()
{
    u8 pp = 0;
    if(frameFlip)
    {
        for(pp = 0; pp < PLAYER_BULLET_MAX; pp++)
        {
            if(playerBulletPtrs[pp] == null) return pp;
        }
    }
    else {
        for(pp = PLAYER_BULLET_MAX; pp < PLAYER_BULLET_MAX*2; pp++)
        {
            if(playerBulletPtrs[pp] == null) return pp;
        }
    }
    
    return 0xFF;
}

void ProcessInput()
{
    if(joyState1 & BTN_A_PRESSED) 
    {   // Fire test
        // find first empty ptr
        u8 nextBulletSpot = FindFirstEmptyBullet();
        if(nextBulletSpot != 0xff)
        {
            if(frameFlip) {
                playerBulletPtrs[nextBulletSpot] = AddSprite(&playerBullets[nextBulletSpot], \
                    p1ship->y_pos,\
                    SPRSIZE(2,2), \
                    SPR_ATTR(140, 0, 0, 1, 0), \
                    p1ship->x_pos);
            }
            else { 
                playerBulletPtrs[nextBulletSpot] = AddSprite(&playerBulletsTwo[nextBulletSpot-PLAYER_BULLET_MAX], \
                    p1ship->y_pos,\
                    SPRSIZE(2,2), \
                    SPR_ATTR(140, 0, 0, 1, 0), \
                    p1ship->x_pos+16);
            }
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


u16 strsize(String* s)
{
    u16 sz;
    while(*s != '\00') sz++;
    return sz;
}
