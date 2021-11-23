//main.c
#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800
#define VRAM_SCROLL 0xfc00

#define BG_WIDTH 64
#define BG_HEIGHT 32

// My own genesis stuff 
#include "bentgen.h"

//static u16 joyState1;
static u32 joyState1;
//static u16 last_joyState1;
static u32 last_joyState1;
static u16 joyState2;
static u16 last_joyState2;
static bool VBL_DONE = false;
struct _textsys \
{
    u16 text_buffer[40*26]; // BUFFER IS SZ OF DISPLAY SCREEN. careful of buffered amount each frame
    u8 startx, starty;      // keep track of where to reset
    u8 txt_x, txt_y;           // Set to start position of text window and incremented each chr
    u8 x_bound, y_bound;    // Right- and bottom-boundaries
    u8 textspeed;           // characters per frame to display
    u16 buffer_ptr;         // location within text_buffer of where to add next chr
    u16 print_ptr;          // location within txt_buffer of next print chr
} ScriptSys;

static struct _counters \
{ 
    u8 sixtyFrameCounter;
    u8 thirtyFrameCounter;
    u8 twentyFrameCounter;
    u8 tenFrameCounter;
} Counters;

u32 OST[64];
u32 SFX[64];

void DrawBGMap(u16 ti, u16* tiledefs, u16 width, u16 height, u16* startaddr, u8 pal);
void PROCESS_FLASH(void);
int main(void);
void (*ProcessInput)(void);
int GetInput(void);

// Assets
#include "gfx.h"
//#include "music.h"
#include "pcmdata.h"
#include "vgmdata.h"
// Game stuff
#include "starthrall.h"
#include "characterdata.h"
#include "title.h"
#include "intro.h"
#include "worldmap.h"

extern struct _counters Counters;
void DO_DEBUG(void);
static struct _debugvars \
{ 
    s32 cycles;
    s32 vcycles;
    u8 vch[3];
    u8 vcl[3];
    u8 _zero;
    u8 ch[3];
    u8 cl[3];
    u8 _zeroa;
    u8 zh[3];
    u8 zl[3];
    bool debug_text_enabled;
} debugVars;
void __attribute__((optimize("O3"))) UpdateDebugText()
{
	extern u8 zcyclesl;
	extern u8 zcyclesh;

	// Every word write to the VDP is ~2 cycles. This takes up 24c!
	print(BG_A, 5, 1, (u8*)debugVars.ch);
	print(BG_A, 7, 1, (u8*)debugVars.cl);
    print(BG_A, 5, 3, (u8*)debugVars.vch);
	print(BG_A, 7, 3, (u8*)debugVars.vcl);
	print(BG_A, 5, 5, (u8*)debugVars.zh);
	print(BG_A, 7, 5, (u8*)debugVars.zl);
	
}
void __attribute__((optimize("O3"))) DO_DEBUG(void)
{
    // Debug Menu 
    UpdateDebugText();
    debugVars.vcycles = 0;
    while(*((u32*)0xc00004) & 0b1000)
        debugVars.vcycles++;
}

// Global Vars
u8 fq;
u8 pSpeed;
u32 frameCounter;
bool frameFlip;
u16 vdpstat;
u32 hcount;

bool REORDER_SPRITES = false;
u8 sprites_destroyed = 0;

// Sprite definitions
Sprite SPRITES[80];

Sprite* test;
u8 NUM_SPRITES;
u32* spriteRamPtr;
u16 tileindex = 0;

Sprite* spr_selector;

u16 bg_map[64*32];
u16 blankpalette[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


void InitGameStuff(void)
{
    u8 i;
    stdcpy((u32*)&party[0], (u32*)&pex, sizeof(struct Player));
    stdcpy((u32*)&party[1], (u32*)&pex2, sizeof(struct Player));
    stdcpy((u32*)&party[2], (u32*)&pex3, sizeof(struct Player));
    stdcpy((u32*)&party[3], (u32*)&pex4, sizeof(struct Player));
    u32* d = (u32*)&tempPalettes[0];
    for(i = 0; i < 32; i++)
    {
        *d++ = 0;
    }
    pSpeed = 3;
    NUM_SPRITES = 0;
    timer_3 = 0;
    player.x = 7;
    player.y = 7;
    selectorpos.x = 100;
    selectorpos.y = 100;
    //ScriptSys.pointer = 0;
    u32* sp = &vgmdata[0];
    u32* vp = &pcmdata[0];
    for(i = 0; i < 64; i++)
    {
        OST[i] = (u32)&vgmdata[0] + (u32)*sp++;
        SFX[i] = (u32)&pcmdata[0] + (u32)*vp++;
    }
    
}

void DrawBGMap(u16 ti, u16* tiledefs, u16 width, u16 height, u16* startaddr, u8 pal)
{
    u8 y, c;
    //WriteVDPRegister(WRITE|REG(0xf)|2);
    for(y = 0; y < height; y++)
    {
        SetVRAMWriteAddress( (u16) (startaddr + (0x40 * y)) );
        for(c = 0; c < width; c++)
        {
            WRITE_DATAREG16( (u16) (ti + tiledefs[(y*40)+c]) | (pal_no(pal)) );
        }
    }
}

void UpdatePrintBuffer(void)
{
    // boundaries defined in global ScriptSys

    // needed opcodes:
    // PRINT &STRINGS (like names)
    // CHANGE TEXT COLOR 
    // LINE BREAK
    // PAUSE FOR N FRAMES
    // SET txt_x, txt_y (bg/vram loc)
    /*
        u16 text_buffer[40*26]; 
        u8 txt_x, txt_y;
        u8 x_bound, y_bound;
        u8 textspeed;    
        u16 buffer_ptr;  
        ScriptSys;
    */
    // write speed x chars directly to vram (set vram addr each char/frame its ok)
    // increase the print_ptr and print chr*textspeed until print ptr == buffer_ptr 
    // if txt_x > x_bound reset
    // if txt_y > y_bound pause!

}

s16 second_counter_a = 0;

void placeholder(void)
{

}

int main(void)
{   
    /////////////////////////////
    ///////////
    ///          GAME SETUP
    ////////
    //////////////////////////////
    u8 i = 0;
    
    spriteRamBase = &SPRITES[0];
    LinkAllSpriteData();
    curPaletteSet[0] = (u16*)&palette;
    curPaletteSet[1] = (u16*)&blankpalette;
    curPaletteSet[2] = (u16*)&blankpalette;
    ResetPalettes(); //for(u8 p_i = 0; p_i < 4; p_i++) LoadPalette(p_i, curPaletteSet[p_i]);

    InitGameStuff();
    
    // INIT TITLE SCREEN
/// INITIALIZE VRAM GRAPHICS ///
    // 0-400h is empty for now
    // Copy in our font!
    // Tile number 32 (start of ascii table) * 32 bytes per tile = 1024 = $400
#define ASCIIBASETILE 32
    tileindex = VDPLoadTiles(ASCIIBASETILE, (u32*)&font_0, 96);
#define SELECTORTILE 128
    tileindex = VDPLoadTiles(128, (u32*)&led_green_0, 1);
#define TITLEBGTILE 129
    InitTitleScreen();

    // Enable VBlank IRQ on VDP 
    WriteVDPRegister(WRITE|REG(1)|0x64);

#define realFrameDelta 0.0167
#define FrameDelta5 

    bgb_vscroll_pos = 0;
    bgb_hscroll_pos = 160;
    bga_vscroll_pos = 0;
    bga_hscroll_pos = 0;
    //WaitVBlank();
    flashAnimPlaying = true;
    Counters.thirtyFrameCounter = 0;
    //unflashAnimPlaying = true;
    fix32 frameDelta;
    flashStepTimer = 0;
    flashStep = 0;
    ticker = 0;
    frameDelta = fp32(realFrameDelta);

    debugVars.debug_text_enabled = true;
    // Clear and reset queue
    //q_in = 0;
    for(i = 0; i < QUEUE_SIZE; i++) function_q[i] = (void*)NULL;
    
    while(TRUE)
    { 
        // Wait for VBL, count the cycles we wait until VBL
		VBL_DONE = false;
		debugVars.cycles = 0;
		while(!VBL_DONE)
			debugVars.cycles++;

        ticker = ticker + 1;
        if (ticker > 60) 
        {
            second_counter_a++;
            ticker = 0;
        }

        // Counter-y stuff
        Counters.twentyFrameCounter++;
        if(Counters.twentyFrameCounter > 19) Counters.twentyFrameCounter = 0;
        Counters.tenFrameCounter++;
        if(Counters.tenFrameCounter > 9) Counters.tenFrameCounter = 0;
        Counters.thirtyFrameCounter++;
        if(Counters.thirtyFrameCounter > 29) Counters.thirtyFrameCounter = 0;

        //UPDATE:
        if(GLOBALWAIT > 0)
        {
            GLOBALWAIT--;
        //    goto DRAW;
            placeholder();
        }
        else {
            DoQ();
            
            // MAIN GAME LOOP 
            PlaySong();
            
            ProcessInput();
            
            if(debugVars.debug_text_enabled)
            {
                byToHex(debugVars.vcycles >> 8, (u8*)&debugVars.vch);
                byToHex(debugVars.vcycles & 0xff, (u8*)&debugVars.vcl);
                byToHex(debugVars.cycles >> 8, (u8*)&debugVars.ch);
                byToHex(debugVars.cycles & 0xff, (u8*)&debugVars.cl);
                byToHex(zcyclesh, (u8*)&debugVars.zh);
                byToHex(zcyclesl, (u8*)&debugVars.zl);           
            }

            if (CUR_SCREEN_MODE == BATTLE){
                ///////////////////////////
                ////      BATTLE CODE 
                ////////////////////////////
                //DoMainBattleLoop();
            } 
            else if (CUR_SCREEN_MODE == TITLE)
            {
                TITLE_UPDATE();
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
        }
        // ****
        //   VBLANK
        // ****
        //DRAW:
        
        //WaitVBlank();       // Wait until draw is done DONT DO THIS IF VBL IRQ IS ON!
        
    }
    return 0;
}
bool asdf = false;
const char hw[] = "Press Start\x00";
// Called during VBlank
void GAME_DRAW(void)
{   
    int i;
    u16 ch;

    last_joyState1 = joyState1;
    GETJOYSTATE1(joyState1);
    //last_joyState2 = joyState2;
    //GETJOYSTATE2(joyState2);

    if(Counters.thirtyFrameCounter == 0){
        if(debugVars.debug_text_enabled)
            DO_DEBUG();
    }
    else {

        if(frameFlip == 0) frameFlip = 1;
        else frameFlip = 0;
        
        // Sprite shit
        // TODO: Convert this to DMA
        u32* spr = &SPRITES[0];
        SetVRAMWriteAddress(VRAM_SAT);
        // sprite count = 1
    #define SPR_COUNT 1
        for(i = 0; i < SPR_COUNT * 2; i++) WRITE_DATAREG32(*spr++);

        if(flashAnimPlaying || unflashAnimPlaying){
            PROCESS_FLASH();
            UpdateBGScroll();
        }
        else {
            if (CUR_SCREEN_MODE == TITLE)
            {
                if(!asdf){
                    //print(BG_A, 10, 15, hw);
                    SetVRAMWriteAddress(VRAM_BG_A + (128*20) + 20);
                    u8* ec = &hw[0];
                    for(u8 t = 0; t < sizeof(hw); t++)
                        WRITE_DATAREG16((u16)*ec++); 
                    asdf = true;
                }
                
            }
            else if (CUR_SCREEN_MODE == INTRO)
            {
                INTRO_DRAW();
            }
            UpdateBGScroll(); // Sets background position in VRAM

            UpdatePrintBuffer(); // If script is printing, process it
        }
        
        i = 0u;
    }        
        
    VBL_DONE = true;
    // end draw
}

void __attribute__((optimize("O3"))) PROCESS_FLASH(void)
{
    u8 i;
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
}