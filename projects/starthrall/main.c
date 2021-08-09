//main.c
#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800
#define VRAM_SCROLL 0xfc00

#define BG_WIDTH 64
#define BG_HEIGHT 32

// My own genesis stuff 
#include "bentgen.h"
// Assets

// Game stuff
#include "gfx.h"
#include "starthrall.h"
#include "characterdata.h"

#include "maptest2.h"

// game function defs 
void main();
int (*ProcessInput)(void);
// test
int GetInput();

// Global Vars
u8 fq;
u8 pSpeed;
u32 frameCounter;
bool frameFlip;
u16 vdpstat;
u32 hcount;
//String hw = "Press START!\x00";
//String okstr = "Okay\x00";
const char hw[] = "Hello Buttons!";

static u16 joyState1;
static u16 last_joyState1;
static u16 joyState2;
static u16 last_joyState2;
static bool VBL_DONE = false;

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
u16 tileindex = 0;

// contains the tile_attr 
u16 bg_a_map[64*32];
u16 blankpalette[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void __attribute__((optimize("Os"))) stdcpy(u32* src, u32* dst, u32 siz)
{
    for(u32 cs = 0; cs < siz; cs++) *dst++ = *src++;
}
/*
void __attribute__((optimize("Os"))) stdfill(u32 chr, u32* dst, u32 siz)
{
    for(u32 a = 0; a < siz; a++) *dst++ = chr;
}
*/

int TitleInputHandler()
{
    //FlashAllPalettes();
    return 0;
}

void InitGameStuff()
{
    u8 i;
    stdcpy(&party[0], &pex, sizeof(struct Player));
    stdcpy(&party[1], &pex2, sizeof(struct Player));
    stdcpy(&party[2], &pex3, sizeof(struct Player));
    stdcpy(&party[3], &pex4, sizeof(struct Player));
    u32* d = &tempPalettes[0];
    for(i = 0; i < 32; i++)
    {
        *d++ = 0;
    }
}

void DrawBGMap(u16 ti, u16* tiledefs, u16 width, u16 height, u16* startaddr, u8 pal)
{
    //WriteVDPRegister(WRITE|REG(0xf)|2);
    for(u8 y = 0; y < height; y++)
    {
        SetVRAMWriteAddress(startaddr + (0x40 * y));
        for(u8 c = 0; c < width; c++)
        {
            WRITE_DATAREG16((u16)ti + tiledefs[(y*40)+c]|(pal_no(pal)));
        }
    }
}


void InitTitleScreen()
{
    u16 c;
    u32* cr;
    // start title music
    // TODO 
    // scroll map
    bgb_hscroll_pos = 180;
    UpdateBGScroll();
    LoadDungeonMap(&maptest2);
    CUR_SCREEN_MODE = TITLE;
    curPaletteSet[3] = (u16*)&titlePalette;
    LoadPalette(3, curPaletteSet[3]);    
    // Make map
    tileindex = 128;
    tileindex = VDPLoadTiles(tileindex, (u32*)&title_test_0, 605);
    DrawBGMap(128, &title_test_map, 40, 28, (u16*)VRAM_BG_B, 3);

    timer_3 = 0;
    // sprite engine (already on)
    // set joy handler
    ProcessInput = TitleInputHandler;
}

void UndarkAllPalettes()
{

}

    
s16 second_counter_a = 0;
bool title_intro_done = false;


void main()
{   
    /////////////////////////////
    ///////////
    ///          GAME SETUP
    ////////
    //////////////////////////////
    u8 i = 0;
    u16 c = 0; 
    u32* cr;
    u8 r = 0;
    timer_3 = 0;
    spriteRamBase = &empty[0];
    LinkAllSpriteData();
    curPaletteSet[0] = (u16*)&palette;
    curPaletteSet[1] = (u16*)&blankpalette;
    curPaletteSet[2] = (u16*)&blankpalette;
    //LoadPalette(0, curPaletteSet[0]);
    for(i = 0; i < 4; i++) LoadPalette(i, curPaletteSet[i]);

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

    // Enable VBlank on VDP 
    WriteVDPRegister(WRITE|REG(1)|0x64);

    bgb_vscroll_pos = 0;
    bgb_hscroll_pos = 160;
    bga_vscroll_pos = 0;
    bga_hscroll_pos = 0;
    //WaitVBlank();
    flashAnimPlaying = true;
    //unflashAnimPlaying = true;
    fix32 frameDelta;
    flashStepTimer = 0;
    flashStep = 0;

#define realFrameDelta 0.0167
#define FrameDelta5 
    flashStepTimer = 0;
    frameDelta = fp32(realFrameDelta);
    title_intro_done = false;
    ticker = 0;
    // Clear and reset queue
    //q_in = 0;
    for(i = 0; i < QUEUE_SIZE; i++) function_q[i] = (void*)NULL;

    
    while(TRUE)
    { 
        
        ticker = ticker + 1;
        if (ticker > 60) 
        {
            second_counter_a++;
            ticker = 0;
        }

        // Counter-y stuff
        twentyFrameCounter++;
        if(twentyFrameCounter > 19) twentyFrameCounter = 0;
        tenFrameCounter++;
        if(tenFrameCounter > 9) tenFrameCounter = 0;

        //Fade in/out - Make me less ugly, please!
        
        UPDATE:
        if(GLOBALWAIT > 0)
        {
            GLOBALWAIT--;
            goto DRAW;
        }

        DoQ();
        
        if (CUR_SCREEN_MODE == BATTLE){
            ///////////////////////////
            ////      BATTLE CODE 
            ////////////////////////////
            //DoMainBattleLoop();
        } 
        else if (CUR_SCREEN_MODE == DUNGEON) 
        {
            ///////////////////////////////////////////
            //         DUNGEON NORMAL STATE
            ///////////////////////////////////////////
            // update dungeon
            //DUNGEON_UPDATE();
        }
        else if(CUR_SCREEN_MODE == WORLDMAP)
        {
            
            //WorldMapUpdate();
            ////////////////////////////
        }
        else if (CUR_SCREEN_MODE == TITLE)
        {
            //if(second_counter_a > 1) title_intro_done = true;
        }
        
        // MAIN GAME LOOP 
        ProcessInput();
        
        // ****
        //   VBLANK
        // ****
        DRAW:
        //WaitVBlank();       // Wait until draw is done DONT DO THIS IF VBL IRQ IS ON!
        VBL_DONE = false;
        while(!VBL_DONE){ };

        
    }
}

// Called during VBlank
void GAME_DRAW()
{   
    if(frameFlip == 0) frameFlip = 1;
    else frameFlip = 0;
    u16 c = 0;
    last_joyState1 = joyState1;
    GETJOYSTATE1(joyState1);
    //last_joyState2 = joyState2;
    //GETJOYSTATE2(joyState2);
    // Sprite shit
    // TODO: Convert this to DMA
    volatile u32* spr = spriteRamBase;
    u8 i = 0;
    //SetVRAMWriteAddress(VRAM_SAT);
    // player1
    //for(i = 0; i < 4 * 2; i++) WRITE_DATAREG32(*spr++);

    if(flashAnimPlaying)
    {
        flashStepTimer ++;
        if (flashStepTimer > 5)
        {
            FlashAllPalettes();
            flashStepTimer = 0;
            flashStep++;
        }
        if(flashStep > 7) {
            flashAnimPlaying = false;
            unflashAnimPlaying = true;
            flashStepTimer = 0;
        }
    }
    if(unflashAnimPlaying)
    {
        flashStepTimer++;
        if(flashStepTimer > 5)
        {
            UnflashAllPalettes();
            flashStepTimer = 0;
            flashStep--;
        }
        if(flashStep == 0) {
            unflashAnimPlaying = false;
            for(i = 0; i < 4; i++) LoadPalette(i, curPaletteSet[i]);
        }
    }

    if (CUR_SCREEN_MODE == TITLE)
    {
        if(timer_3 <= 160) { 
            // scroll the map in!
            //if(titleScr != null)
            //    MAP_scrollTo(titleScr, timer_3, 0);
            timer_3 += 20;
            bgb_hscroll_pos = 180 - timer_3;
            bgb_vscroll_pos = 0;
            //bgb_vscroll_pos = (u16)timer_3;
        }
        else { 
            if(!flashAnimPlaying){
                // if done, print :
                SetVRAMWriteAddress(0xc000 + (64*21*2) + (9*2)); // Screen address + 21 Y, 9 X (BG_A)
                // (16bit, 64 chars/map)
                u8* chp = (u8*)&hw[0];       // String address
                for(c = 0; c < sizeof(hw); c++) WRITE_DATAREG16((u16)*chp++); // Loop
                //SetInputCallback(&GetInput);
            }
        }
        if ((flashAnimPlaying) && (flashStep > 6)) { 
            if ( NEXT_SCREEN_MODE == WORLDMAP)
            {    
                //InitWorldMapState();
            }
            else if ( NEXT_SCREEN_MODE == INTRO)
            {    
                //InitIntro();
            }
        }
    }
    VBL_DONE = true;
        // end draw
    UpdateBGScroll();
}



//int GetInput()
//{
    //FlashAllPalettes();
//    return 0;
//}

//void SetInputCallback(void(* f)())
//{
//    void(*ProcessInput)() = &f;
//}

//void ProcessInput()
//{
 //   void(*f)() = &function;
 //   f();
//}
