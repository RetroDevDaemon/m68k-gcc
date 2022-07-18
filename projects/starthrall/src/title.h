extern u32 last_joyState1, joyState1;
extern void (*ProcessInput)(void);
const char str_pressStart[] = "PRESS START";
extern void NullInputHandler(void);
extern Sprite SPRITES[80];
extern Sprite* spr_selector;

#define SELECTORTILE 128

#include "maptest2.h"

bool title_intro_done = false;
bool go_intro = false;
//bool go_intro;

void FillVRAM(u16 ti, u8 pal, u16* startaddr, u16 len);
int TitleInputHandler(void);
void InitTitleScreen(void);
void TITLE_DRAW(void);

void FillVRAM(u16 ti, u8 pal, u16* startaddr, u16 len)
{
    int i;
    SetVRAMWriteAddress((u32)startaddr);
    for(i = 0; i < len; i++)
    {
        WRITE_DATAREG16((u16)ti|(pal_no(pal)));
    }
}

int TitleInputHandler(void)
{
    if(title_intro_done)
    {
        if(Joy1Down(BTN_START_PRESSED))
        {
            flashAnimPlaying = true;
            flashStepTimer = 0;
            flashStep = 0;
            ticker = 0;
            go_intro = true;
            ProcessInput = NullInputHandler;
        }
    }
    return 0;
}

extern u32 OST[64];

void InitTitleScreen(void)
{
    // start title music
    // CURRENTLY SAMPLE LESS VGM PLAYER IS ENABLED. NEEDS VGM FILE WITH HEADER.
    LoadSong(freefall);
    //LoadSong(&nullsong);
    // scroll map
    
    //go_intro = false;
    title_intro_done = false;
    bgb_hscroll_pos = 180;
    UpdateBGScroll();
    // test!
    LoadDungeonMap((const u8*)&maptest2);
    
    CUR_SCREEN_MODE = TITLE;
    curPaletteSet[3] = (u16*)&titlePalette;
    LoadPalette(3, curPaletteSet[3]);    
    
    // Make map
    tileindex = VDPLoadTiles(tileindex, (u32*)&title_test_0, 605);
    
    // title_bg_tile_index = 129
    DrawBGMap(129, (u16*)&title_test_map, 40, 28, (u16*)VRAM_BG_B, PAL3);

    timer_3 = 0;
    // sprite engine (already on)
    // set joy handler
    ProcessInput = TitleInputHandler;
    
}

extern void InitIntro(void);

void TITLE_UPDATE(void)
{
    int c;
    if(!title_intro_done)
    {
        if(timer_3 <= 160) { 
            timer_3 += 20;
            bgb_hscroll_pos = 180 - timer_3;
            bgb_vscroll_pos = 0;
        }
        else {
            title_intro_done = true;
            //go_intro=true;
        }
    }
    if((go_intro == true))
    {

        InitIntro();
    }
}

