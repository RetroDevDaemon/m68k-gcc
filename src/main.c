//main.c
#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800
#define VRAM_SCROLL 0xfc00

#define BG_WIDTH 64
#define BG_HEIGHT 32

// My own genesis stuff 
#include "bentgen.h"

// required defs
typedef s32 fp32;
typedef s16 fp16;
typedef unsigned char Map[]; // FIXME
typedef u8 TileSet[];
typedef u8 SpriteDefinition[];

typedef s32 fix32;
typedef s16 fix16;
#define fp(n) (fix16)(n*(1<<8))
#define fp32(n) (fix32)(n*(1<<16))

// Assets
#include "gfx.h"
// Game stuff
#include "starthrall.h"
#include "characterdata.h"

#include "maptest2.h"

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
String hw = "Hello STARTHRALL!";

static u16 joyState1;
static u16 last_joyState1;
static u16 joyState2;
static u16 last_joyState2;

bool REORDER_SPRITES = false;
u8 sprites_destroyed = 0;

static u16 tempPalette[] = { \
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00 
};     

// Sprite definitions
static Sprite empty[80];

u8 sixtyFrameCounter = 0;
u8 twentyFrameCounter = 0;
u8 tenFrameCounter = 0;
Sprite* test;
u8 NUM_SPRITES;
u32* spriteRamPtr;
u16 tileindex = 0;

// contains the tile_attr 
u16 bg_a_map[64*32];


void __attribute__((optimize("Os"))) stdcpy(u32* src, u32* dst, u32 siz)
{
    for(u32 cs = 0; cs < siz; cs++) *dst++ = *src++;
}

void __attribute__((optimize("Os"))) stdfill(u32 chr, u32* dst, u32 siz)
{
    for(u32 a = 0; a < siz; a++) *dst++ = chr;
}


void InitGameStuff()
{
    stdcpy(&party[0], &pex, sizeof(struct Player));
    stdcpy(&party[1], &pex2, sizeof(struct Player));
    stdcpy(&party[2], &pex3, sizeof(struct Player));
    stdcpy(&party[3], &pex4, sizeof(struct Player));
}

void DrawBGMap(u16 ti, u16* tiledefs, u16 width, u16 height, u16* startaddr)
{
    for(u8 y = 0; y < height; y++)
    {
        SetVRAMWriteAddress(startaddr + (0x40 * y));
        for(u8 c = 0; c < width; c++)
        {
            WRITE_DATAREG16((u16)ti + tiledefs[(y*40)+c]|(pal_no(1)));
        }
    }
}

void InitTitleScreen()
{
    u16 c;
    u32* cr;
    // start title music
    // TODO 

    LoadDungeonMap(&maptest2);
    CUR_SCREEN_MODE = TITLE;

    LoadPalette(1, (u16*)&titlePalette);    
    // Make map
    tileindex = 128;
    tileindex = VDPLoadTiles(tileindex, (u32*)&title_test_0, 605);
    DrawBGMap(128, &title_test_map, 40, 28, (u16*)VRAM_BG_B);

    timer_3 = 192;
    // scroll map
    bgb_hscroll_pos = timer_3;
    //vbl once
    //WaitVBlank();
}

void main()
{   
    /////////////////////////////
    ///////////
    ///          GAME SETUP
    ////////
    //////////////////////////////
    // Auto-increment to 2
    //WriteVDPRegister(WRITE|REG(0xf)|2);
    //WaitVBlank();                           // wait 1 frame just because
    //stdcpy(0x00ff0000, 0x00ff0001, 100);
    stdfill((u32)0, &function_q[0], 202);   // clear the function queue
    
    spriteRamBase = &empty[0];
    LinkAllSpriteData();
    LoadPalette(0, (u16*)&palette);


    u8 i = 0;
    u16 c = 0; 
    u32* cr;
    u8 r = 0;
    pSpeed = 3;
    NUM_SPRITES = 0;

    InitGameStuff();
    player.x = 7;
    player.y = 7;

    // RESET YM2612
    // TODO

    // RESET PSG
    // TODO

    // INIT TITLE SCREEN


/// INITIALIZE VRAM GRAPHICS ///
    // 0-400h is empty for now
    // Copy in our font!
    // Tile number 32 (start of ascii table) * 32 bytes per tile = 1024 = $400
#define ASCIIBASETILE 32
    tileindex = VDPLoadTiles(32, (u32*)&font_0, 96);

    InitTitleScreen();

// PRINT HW
    SetVRAMWriteAddress(0xc000); // Screen address
    u8* chp = (u8*)&hw[0];       // String address
    for(c = 0; c < sizeof(hw); c++) WRITE_DATAREG16((u16)*chp++); // Loop
    // + c00 = 1000
///

    // Enable VBlank on VDP 
    WriteVDPRegister(WRITE|REG(1)|0x64);

    bgb_vscroll_pos = 0;//p1ship[0]->y_pos / 16;
    bgb_hscroll_pos += 0;


    while(TRUE)
    { 
        ticker = ticker + 1;
        if (ticker > framerate) ticker = 0;
        
        //Fade in/out - Make me less ugly, please!
        if(flashAnimPlaying){
            flashStepTimer += frameDelta;
            if(flashStepTimer - fp32(0.5) > (frameDelta*5)) {
                FlashAllPalettes();
                flashStepTimer -= (frameDelta*5);
                flashStep++;
            }
        }
        /*
        if(unflashAnimPlaying){
            flashStepTimer += frameDelta;
            if(flashStepTimer - 0.5 > (frameDelta*5)) {
                UnflashAllPalettes();
                flashStepTimer -= (frameDelta*5);
                flashStep++;
            }
        }
        if(flashDarkAnimPlaying){
            flashStepTimer += 1;
            if(flashStepTimer > 5) { 
                UnflashAllPalettes();
                flashStepTimer = 0;
                flashStep++;
            }
        }
        if(unflashDarkAnimPlaying){
            flashStepTimer += 1;
            if(flashStepTimer > 1) { 
                UndarkAllPalettes();
                flashStepTimer = 0;
                flashStep++;
            }
        }
        */

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
