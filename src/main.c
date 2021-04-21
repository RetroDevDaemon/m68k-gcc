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

// game function defs 
void main();
void GAME_INPUT();

u8 FindFirstEmptyBullet();
void LinkBulletsTwo();
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
static Sprite player1sprite[3];
static Sprite playerBullets[27];
static Sprite enemySprites[15];
static Sprite enemyBullets[30];
static Sprite moon;
static Sprite empty[4];
// 4 left
// 80 = end of active sprite data
static Sprite* p1ship[3];
static Sprite* playerBulletPtrs[27*2];
static Sprite playerBulletsTwo[27];
typedef struct {
    Sprite* ptr;    // fill when adding the sprite
    u16 tile_attr;  // tile start and pal etc. <- swap for animation
    u8 size;        // size macro 
    u8 nop;
} EnemyType;
static EnemyType Enemies[15];

u8 num_p_bullets = 0;
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
struct bullet 
{ 
    signed char timer;
    Sprite* myspr;
};

void __attribute__((optimize("Os"))) stdcpy(u32* src, u32* dst, u32 siz)
{
    for(u32 cs = 0; cs < siz; cs++) *dst++ = *src++;
}


void main()
{   
    //WaitVBlank();
    //stdcpy(0x00ff0000, 0x00ff0001, 100);
    spriteRamBase = &player1sprite;
    LinkAllSpriteData();
    LinkBulletsTwo();
    LoadPalette(1, (u16*)&phaes1_palette);
    LoadPalette(0, (u16*)&level1_bga_palette);
    LoadPalette(2, (u16*)&level1_bga_palette2);
    LoadPalette(3, (u16*)&slime1_palette);
    
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
        cr = (u32*)&player1b_0;  
        cr += c;
        WRITE_DATAREG32(*cr);
    }
    // Copy in bullet - ID 140
    for(c = 0; c < (8 * 4); c++) {
        cr = (u32*)&pbullet1_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
    // asteroid - ID 144
    for(c = 0; c < (8 * 20); c++) {
        cr = (u32*)&asteroid_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    } // to 164
    for(c = 0; c < (8 * 32); c++) { // 2x2 * 8 frames
        cr = (u32*)&slime1_sheet_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    } // to 196
    for(c = 0; c < (16*3*8); c++) { // phaes only
    //for(c = 0; c < (16*5*8); c++) { // phaesta and hairb +harc
        cr = (u32*)&phaes1_rows_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    } //244:
    for(c = 0; c < (8 * 575); c++) { //575
        cr = (u32*)&stage1_bga_b_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    } // 819:
    for(c = 0; c < (8 * 216); c++) { 
        cr = (u32*)&stage1_bottombg_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    } // 1035:
    for(c = 0; c < (8 * 16); c++) { // moon
        cr = (u32*)&moons_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
///
    AddSprite(&moon, 150, SPRSIZE(4,4), SPR_ATTR(1035, 0, 0, 0, 0), 300);
    // draw map test
    // FRONT LAYER
    DrawTile(BG_A, TILEATTR(819, 0, 0, 0, 0), 0, 21, 27, 8);
    DrawTile(BG_A, TILEATTR(819, 0, 0, 0, 0), 27, 21, 27, 8);
    DrawTile(BG_A, TILEATTR(819, 0, 0, 0, 0), 64-27, 21, 27, 8);
    // BG LAYER
    for(u8 y = 0; y < 30; y++){
        SetVRAMWriteAddress(VRAM_BG_B + (0x80 * y));
        for(c = 0; c < 48; c++) {
            WRITE_DATAREG16((u16)(244 + stage1_bga_b_map[(y*48)+c])|(pal_no(2)));
        }
    }
    // set ship as 4x3 sprite
    // size: E. 8 byte
#define PLAYER1_SPR 128
    p1ship[0] = AddSprite(&player1sprite[0], 200, SPRSIZE(4,4), SPR_ATTR(196, 0, 0, 1, 0), 200);
    p1ship[1] = AddSprite(&player1sprite[1], 232, SPRSIZE(4,4), SPR_ATTR(196+16, 0, 0, 1, 0), 200);
    p1ship[2] = AddSprite(&player1sprite[2], 200, SPRSIZE(4,4), SPR_ATTR(196+32, 0, 0, 1, 0), 232);

#define BLOB1_SPR 164
    Enemies[0].size = SPRSIZE(2,2);
    Enemies[0].tile_attr = SPR_ATTR(BLOB1_SPR, 0, 0, 3, 0);
    Enemies[0].ptr = AddSprite(&enemySprites[0], 180, Enemies[0].size, Enemies[0].tile_attr, 400);
    Enemies[1].ptr = AddSprite(&enemySprites[1], 200, Enemies[0].size, Enemies[0].tile_attr, 420);
    
#define ASTEROID_A 144
    //DrawTile(BG_B, TILEATTR(144, 0, 0, 2, 0), 10, 10, 5, 4);
    //DrawTile(BG_B, TILEATTR(144, 0, 0, 2, 0), 12, 17, 5, 4);

    // BG plane A
    //print(BG_A, 0, 0, hw);
    //print(BG_A, 1, 1, hw2);
    //print(BG_A, 2, 2, hw3);

#define PLAYER_BULLET_MAX 27
        
    // clear bullets
    for(i = 0; i < PLAYER_BULLET_MAX*2; i++) playerBulletPtrs[i] = null;

    // Enable VBlank on VDP 
    WriteVDPRegister(WRITE|REG(1)|0x64);
    num_p_bullets = 0;
    bgb_vscroll_pos += 45;
    bgb_hscroll_pos += 300;
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
        
        // BULLET HELL!
        for(i = 0; i < PLAYER_BULLET_MAX * 2; i++)
        { 
            if(playerBulletPtrs[i]->spr_attr != 0)
            {
                //process bullet's code 
                playerBulletPtrs[i]->x_pos += 16;
                // out of bounds?
                if((playerBulletPtrs[i]->x_pos > 480))
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
        
        // Enemy behavior
        for(i = 0; i < 2; i++) {
            Enemies[i].ptr->x_pos --;
            if(twentyFrameCounter == 0) {
                Enemies[i].ptr->spr_attr = TILEATTR(BLOB1_SPR + 4, 0, 0, 3, 0);
            }
            if(twentyFrameCounter == 10) {
                Enemies[i].ptr->spr_attr = TILEATTR(BLOB1_SPR, 0, 0, 3, 0);
            }  
            //enemySprites[i].spr_attr = Enemies[i].tile_attr;
        }
        

        // Player animation
        /*
        if(twentyFrameCounter == 0) {
            player1sprite[0].spr_attr = TILEATTR(196, 0, 0, 1, 0);
        }
        else if (twentyFrameCounter == 5){
            player1sprite[0].spr_attr = TILEATTR(196+48, 0, 0, 1, 0);
        }
        else if (twentyFrameCounter == 10){
            player1sprite[0].spr_attr = TILEATTR(196, 0, 0, 1, 0);
        }
        else if (twentyFrameCounter == 15){
            player1sprite[0].spr_attr = TILEATTR(196+64, 0, 0, 1, 0);
        }*/

        // BG scrolling test 
        sixtyFrameCounter++;
        if(sixtyFrameCounter > 59) {
            sixtyFrameCounter = 0;
            bgb_hscroll_pos--;
        }
        //bgb_hscroll_pos -= 6;
        //bgb_vscroll_pos -= 8;
        bga_hscroll_pos -= 2;
        //bgb_vscroll_pos++;
    }
}

// Called during VBlank
void GAME_DRAW()
{   
    if(frameFlip == 0) frameFlip = 1;
    else frameFlip = 0;

    GAME_INPUT();       // get fresh joy states

    // Sprite shit
    // TODO: Convert this to DMA
    u32* spr = spriteRamBase;
    u8 i = 0;
    SetVRAMWriteAddress(VRAM_SAT);
    // player1
    for(i = 0; i < 3 * 2; i++) WRITE_DATAREG32(*spr++);
    // bullet flicker
    if(frameFlip) 
        spr = (u32*)&playerBullets; 
    else 
        spr = (u32*)&playerBulletsTwo; 
    for(i = 0; i < PLAYER_BULLET_MAX*2; i++)
        WRITE_DATAREG32(*spr++); 
    // enemy sprites
    spr = (u32*)&enemySprites;
    for(i = 0; i < 15 * 2; i++) 
        WRITE_DATAREG32(*spr++); 
    // moon!
    spr = (u32*)&moon;
    WRITE_DATAREG32(*spr++);
    WRITE_DATAREG32(*spr++);
    // write scroll ram
    UpdateBGScroll();
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

void __attribute__((optimize("Os"))) LinkBulletsTwo()
{
    // UH OH! Link these fools!
    Sprite* s = (Sprite*)&playerBullets;
    for(u8 i = 0; i < PLAYER_BULLET_MAX; i++){
        playerBulletsTwo[i].next = s[i].next;
        playerBulletsTwo[i].x_pos = 511;
        playerBulletsTwo[i].y_pos = 511;
        playerBulletsTwo[i].spr_attr = 0;
    }
}

u8 __attribute__((optimize("Os"))) FindFirstEmptyBullet()
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
    u8 ps = 0;
    if(joyState1 & BTN_UP_PRESSED) // Up
    {
        for(ps = 0; ps < 3; ps++) p1ship[ps]->y_pos -= pSpeed;
    }
    if(joyState1 & BTN_DOWN_PRESSED) // Down
    {
        for(ps = 0; ps < 3; ps++) p1ship[ps]->y_pos += pSpeed;
    }
    if(joyState1 & BTN_LEFT_PRESSED) // Left
    {
        for(ps = 0; ps < 3; ps++) p1ship[ps]->x_pos -= pSpeed;
    }
    if(joyState1 & BTN_RIGHT_PRESSED) // Right
    {
        for(ps = 0; ps < 3; ps++) p1ship[ps]->x_pos += pSpeed;
    }
    if(joyState1 & BTN_A_PRESSED) 
    {   // Fire test
        // find first empty ptr
        u8 nextBulletSpot = FindFirstEmptyBullet();
        if(nextBulletSpot != 0xff)
        {
            volatile Sprite* bulletsat;
            u16 x_ofs = p1ship[2]->x_pos + 4;
            if(frameFlip) {
                bulletsat = &playerBullets[nextBulletSpot];
                //bullet level 0
            }
            else { 
                x_ofs += 16;
                //bullet level 0
                bulletsat = &playerBulletsTwo[nextBulletSpot-PLAYER_BULLET_MAX];
            }
            playerBulletPtrs[nextBulletSpot] = AddSprite(bulletsat, \
                    p1ship[2]->y_pos + 8,\
                    SPRSIZE(1,1), \
                    SPR_ATTR(140, 0, 0, 1, 0), \
                    x_ofs);
        }
    }

}
