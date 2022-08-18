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
int main();
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

bool facingLeft = false;
bool focused = false;

bool REORDER_SPRITES = false;
u8 sprites_destroyed = 0;

#define PLAYER_BULLET_MAX 10
        
// Sprite definitions
static Sprite hitbox;
static Sprite player1sprite[3];
static Sprite playerBullets[PLAYER_BULLET_MAX]; 
static Sprite enemySprites[15]; 
static Sprite moon; //30
static Sprite sunn;
static Sprite enemyBullets[15]; //46
static Sprite empty[34];

// 80 = end of active sprite data
static Sprite playerBulletsTwo[PLAYER_BULLET_MAX];
static Sprite playerBulletsThree[PLAYER_BULLET_MAX];
static Sprite playerBulletsFour[PLAYER_BULLET_MAX];

static Sprite* p1ship[3];

typedef struct {
    Sprite* ptr;    // fill when adding the sprite
    u16 tile_attr;  // tile start and pal etc. <- swap for animation
    u8 size;        // size macro 
    //u8 nop;
} EnemyType;
static EnemyType Enemies[15];

typedef struct 
{ 
    s8 timer;
    s8 spd;
    Sprite* myspr;
} bullet;
static bullet playerBulletObj[PLAYER_BULLET_MAX*2];

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

void __attribute__((optimize("Os"))) standardcopy(u32* src, u32* dst, u32 siz)
{
    for(u32 cs = 0; cs < siz; cs++) *dst++ = *src++;
}


int main()
{   
    WaitVBlank();
    //stdcpy(0x00ff0000, 0x00ff0001, 100);
    spriteRamBase = &hitbox;
    LinkAllSpriteData();
    LinkBulletsTwo();
    LoadPalette(1, (u16*)&enemy_palette1);
    LoadPalette(0, (u16*)&level1_bga_palette);
    LoadPalette(2, (u16*)&stage1_palette);
    //LoadPalette(3, (u16*)&slime1_palette);
    
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
    SetVRAMWriteAddress(ASCIIBASETILE*0x20);
    for(c = 0; c < 0x300; c++) 
    {
        cr = (u32*)&font_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
    // + c00 = 1000
    // Copy in ship
#define PSHIPBASETILE 128
    SetVRAMWriteAddress(PSHIPBASETILE*0x20); // 128
    for(c = 0; c < (12*8); c++) 
    {
        cr = (u32*)&player1b_0;  
        cr += c;
        WRITE_DATAREG32(*cr);
    }
#define BULLETTILE 140
    // Copy in bullet - ID 140
    for(c = 0; c < (8 * 4); c++) {
        cr = (u32*)&pbullet1_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
#define ASTEROIDTILE 144
    // asteroid - ID 144
    for(c = 0; c < (8 * 20); c++) {
        cr = (u32*)&asteroid_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    } // to 164
#define SLIME1_TILE 164
    for(c = 0; c < (8 * 20); c++) { // 5 frames
        cr = (u32*)&slime1_sheet_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    } // to 184
#define PHAESTATILE 184
    for(c = 0; c < (8 * 80); c++) { // phaes - +80
        cr = (u32*)&phaes1_rows_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    } //232+32:
#define STAGE1BGATILE 264
    for(c = 0; c < (8 * 744); c++) { 
        cr = (u32*)&stage1_bga_b_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    } 
#define STAGE1BGBTMTILE 1008
    for(c = 0; c < (8 * 216); c++) { 
        cr = (u32*)&stage1_bottombg_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    } // 1081:
#define MOONTILE 1224
    for(c = 0; c < (8 * 16); c++) { // moon
        cr = (u32*)&moons_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
#define SUNTILE 1224+16
        for(c = 0; c < (8 * 16); c++) { // sun
        cr = (u32*)&sun_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
#define HITBOXTILE 1224+32
    for(c = 0; c < (8 * 2); c++) // 2 tiles for hb
    {
        cr = (u32*)&hitbox_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
#define ENEMY1TILE 1224+32+2
    for(c = 0; c < (8 * 16); c++)
    {
        cr = (u32*)&enemy1_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
#define ENEMY2TILE 1224+32+2+16
    for(c = 0; c < (8 * 16); c++)
    {
        cr = (u32*)&enemy2_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
#define ENEMY3TILE 1290 // cap: 1536 (c000)
    for(c = 0; c < (8 * 16); c++)
    {
        cr = (u32*)&enemy3_0;
        cr += c;
        WRITE_DATAREG32(*cr);
    }
///
    AddSprite(&moon, 260, SPRSIZE(4,4), SPR_ATTR(SUNTILE, 0, 0, 0, 0), 350);
    AddSprite(&sunn, 150, SPRSIZE(4,4), SPR_ATTR(MOONTILE, 0, 0, 0, 0), 300);
    AddSprite(&hitbox, 511, SPRSIZE(1,1), SPR_ATTR(HITBOXTILE, 0, 0, 0, 0), 511);
    // draw map test
    // FRONT LAYER
    DrawTile(BG_A, TILEATTR(STAGE1BGBTMTILE, 0, 0, 0, 0), 0, 19, 27, 8);
    DrawTile(BG_A, TILEATTR(STAGE1BGBTMTILE, 0, 0, 0, 0), 27, 19, 27, 8);
    DrawTile(BG_A, TILEATTR(STAGE1BGBTMTILE, 0, 0, 0, 0), 64-27, 19, 27, 8);
    // BG LAYER
#define STAGE1BGWID 376/8
#define STAGE1BGHEI 256/8
    for(u8 y = 0; y < STAGE1BGHEI; y++){
        SetVRAMWriteAddress(VRAM_BG_B + (0x80 * y));
        for(c = 0; c < STAGE1BGWID; c++) {
            WRITE_DATAREG16((u16)(STAGE1BGATILE + stage1_bga_b_map[(y*STAGE1BGWID)+c])|(pal_no(2)));
        }
    }
    // set ship as 4x3 sprite
    // size: E. 8 byte
#define PLAYER1_SPR 184
    p1ship[0] = AddSprite(&player1sprite[0], 200, SPRSIZE(4,4), SPR_ATTR(PLAYER1_SPR, 0, 0, 0, 0), 200);
    p1ship[1] = AddSprite(&player1sprite[1], 232, SPRSIZE(4,4), SPR_ATTR(PLAYER1_SPR+16, 0, 0, 0, 0), 200);
    p1ship[2] = AddSprite(&player1sprite[2], 200, SPRSIZE(4,4), SPR_ATTR(PLAYER1_SPR+32, 0, 0, 0, 0), 232);

#define BLOB1_SPR 164
    Enemies[0].size = SPRSIZE(4,4);
    Enemies[0].tile_attr = SPR_ATTR(ENEMY1TILE, 0, 0, 1, 0);
    Enemies[0].ptr = AddSprite(&enemySprites[0], 180, Enemies[0].size, Enemies[0].tile_attr, 400);
    Enemies[1].size = SPRSIZE(4,4);
    Enemies[1].tile_attr = SPR_ATTR(ENEMY2TILE, 0, 0, 1, 0);
    Enemies[1].ptr = AddSprite(&enemySprites[1], 210, Enemies[1].size, Enemies[1].tile_attr, 385);
    
    //Enemies[1].ptr = AddSprite(&enemySprites[1], 200, Enemies[0].size, Enemies[0].tile_attr, 420);
    
#define ASTEROID_A 144
    //DrawTile(BG_B, TILEATTR(144, 0, 0, 2, 0), 10, 10, 5, 4);
    //DrawTile(BG_B, TILEATTR(144, 0, 0, 2, 0), 12, 17, 5, 4);


    // clear bullets
    for(i = 0; i < PLAYER_BULLET_MAX*2; i++) playerBulletObj[i].myspr = null;

    // Enable VBlank on VDP 
    WriteVDPRegister(WRITE|REG(1)|0x64);
    num_p_bullets = 0;
    bgb_vscroll_pos = 0;//p1ship[0]->y_pos / 16;
    bgb_hscroll_pos += 0;
    focused = false;

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
            if(playerBulletObj[i].myspr->spr_attr != 0)
            {
                //process bullet's code
                playerBulletObj[i].myspr->x_pos += playerBulletObj[i].spd;
                // out of bounds?
                if((playerBulletObj[i].myspr->x_pos > 480))
                { 
                    // ...destroy the bullet
                    num_p_bullets--;
                    playerBulletObj[i].myspr->y_pos = 0;
                    playerBulletObj[i].myspr->spr_attr = 0;
                    playerBulletObj[i].myspr->size = 0;
                    playerBulletObj[i].myspr= null;
                }
            }
        }
        
        // Enemy behavior
        for(i = 0; i < 2; i++) {
            Enemies[i].ptr->x_pos --;
            /**
            if(twentyFrameCounter == 0) {
                Enemies[i].ptr->spr_attr = TILEATTR(BLOB1_SPR + 4, 0, 0, 1, 0);
            }
            if(twentyFrameCounter == 10) {
                Enemies[i].ptr->spr_attr = TILEATTR(BLOB1_SPR, 0, 0, 1, 0);
            }  
            **/
        }

        // flash hitbox
        
        if(focused)
        {
            if(tenFrameCounter == 5) {
                hitbox.spr_attr = SPR_ATTR(HITBOXTILE+1, 0, 0, 0, 0);
            }
            elseif (tenFrameCounter == 0) {
                hitbox.spr_attr = SPR_ATTR(HITBOXTILE, 0, 0, 0, 0);
            }
            hitbox.x_pos = p1ship[0]->x_pos + 20 + (facingLeft * 16);
            hitbox.y_pos = p1ship[0]->y_pos + 20;
        }
        

        // BG scrolling test 
        sixtyFrameCounter++;
        if(sixtyFrameCounter > 59) {
            sixtyFrameCounter = 0;
            bgb_hscroll_pos--;
        }
        bga_vscroll_pos = 0;//64;
        if(p1ship[0]->y_pos > 128) {
            bgb_vscroll_pos = (p1ship[0]->y_pos/4) - 16;
            bga_vscroll_pos = (p1ship[0]->y_pos/8) - 56;
            if(bgb_vscroll_pos < 0) bgb_vscroll_pos = 0;
            //if(bga_vscroll_pos > 192) bga_vscroll_pos = 192;
        }
        else 
        {
            bga_vscroll_pos = (128/8)-56;
        }
        bga_hscroll_pos -= 2;
    }
    return 0;
}

// Called during VBlank
void GAME_DRAW()
{   
    if(frameFlip == 0) frameFlip = 1;
    else frameFlip = 0;

    GAME_INPUT();       // get fresh joy states

    // Sprite shit
    // TODO: Convert this to DMA
    volatile u32* spr = spriteRamBase;
    u8 i = 0;
    SetVRAMWriteAddress(VRAM_SAT);
    // player1
    for(i = 0; i < 4 * 2; i++) WRITE_DATAREG32(*spr++);
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
    // moon and sun!
    spr = (u32*)&moon;
    for(i = 0; i < 2; i++) WRITE_DATAREG32(*spr++);
    spr = (u32*)&sunn;
    for(i = 0; i < 2; i++) WRITE_DATAREG32(*spr++);

    // hitbox (always)
    //spr = (u32*)&hitbox;
    //for(i = 0; i < 2; i++) WRITE_DATAREG32(*spr++);
    

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

u8 __attribute__((optimize("O3"))) FindFirstEmptyBullet()
{
    u8 pp = 0;
    if(frameFlip)
    {
        for(pp = 0; pp < PLAYER_BULLET_MAX; pp++)
        {
            if(playerBulletObj[pp].myspr == null) return pp;
        }
    }
    else {
        for(pp = PLAYER_BULLET_MAX; pp < PLAYER_BULLET_MAX*2; pp++)
        {
            if(playerBulletObj[pp].myspr == null) return pp;
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
        // todo: check optimization here
        u8 nextBulletSpot = FindFirstEmptyBullet();
        if(nextBulletSpot != 0xff)
        {
            volatile Sprite* bulletsat;
            s16 x_ofs = p1ship[2]->x_pos + 4;
            if(frameFlip) {
                bulletsat = &playerBullets[nextBulletSpot];
                //bullet level 0
            }
            else { 
                x_ofs += 16;
                //bullet level 0
                bulletsat = &playerBulletsTwo[nextBulletSpot-PLAYER_BULLET_MAX];
            }
            x_ofs += (facingLeft * -24);
            playerBulletObj[nextBulletSpot].myspr = AddSprite(bulletsat, \
                    p1ship[2]->y_pos + 8,\
                    SPRSIZE(1,1), \
                    SPR_ATTR(140, facingLeft, 0, 1, 0), \
                    x_ofs);
            if(facingLeft) playerBulletObj[nextBulletSpot].spd = -16;
            else playerBulletObj[nextBulletSpot].spd = 16;
        }
    }

    // FOCUS MODE
    if(Joy1Down(BTN_B_PRESSED))
    {
        pSpeed = 1;
        focused = true;
        hitbox.x_pos = p1ship[0]->x_pos + 20;
        hitbox.y_pos = p1ship[0]->y_pos + 20;
    }
    else if(Joy1Up(BTN_B_PRESSED))
    {
        pSpeed = 3;
        focused = false;
        hitbox.x_pos = 511;
        hitbox.y_pos = 511;
    }

    // SWITCH FACING
    if(Joy1Down(BTN_C_PRESSED))
    {
        if(facingLeft == false)
        {
            facingLeft = true;
            p1ship[0]->spr_attr = SPR_ATTR(PLAYER1_SPR + 48, 0, 0, 0, 0);
            p1ship[0]->x_pos -= 16;
            p1ship[2]->spr_attr = SPR_ATTR(PLAYER1_SPR + 64, 0, 0, 0, 0);
            p1ship[2]->x_pos -= 16;
        } else { 
            facingLeft = false;
            p1ship[0]->spr_attr = SPR_ATTR(PLAYER1_SPR, 0, 0, 0, 0);
            p1ship[0]->x_pos += 16;
            p1ship[2]->spr_attr = SPR_ATTR(PLAYER1_SPR + 32, 0, 0, 0, 0);
            p1ship[2]->x_pos += 16;
        }
    }
}
