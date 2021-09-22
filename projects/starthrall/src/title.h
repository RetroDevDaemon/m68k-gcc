bool title_intro_done;
bool go_intro;
extern u32 last_joyState1, joyState1;
extern void (*ProcessInput)(void);
const char str_pressStart[] = "PRESS START";
extern void NullInputHandler(void);
extern Sprite SPRITES[80];
extern Sprite* spr_selector;

#define SELECTORTILE 128

#include "maptest2.h"

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

void InitTitleScreen(void)
{
    // start title music
    LoadSong(&caustic_love[0]);
    // scroll map
    go_intro = false;
    title_intro_done = false;
    bgb_hscroll_pos = 180;
    UpdateBGScroll();
    LoadDungeonMap((const u8*)&maptest2);
    CUR_SCREEN_MODE = TITLE;
    curPaletteSet[3] = (u16*)&titlePalette;
    LoadPalette(3, curPaletteSet[3]);    
    // Make map
    tileindex = VDPLoadTiles(tileindex, (u32*)&title_test_0, 605);
    // title_bg_tile_index = 129
    DrawBGMap(129, (u16*)&title_test_map, 40, 28, (u16*)VRAM_BG_B, 3);

    timer_3 = 0;
    // sprite engine (already on)
    // set joy handler
    ProcessInput = TitleInputHandler;
}


void TITLE_DRAW(void)
{
    int c;
    if(!title_intro_done)
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
                print(BG_A, 9, 21, str_pressStart);
                
                title_intro_done = true;
            }
        }
    }
    else
    {
        if(go_intro)
        {
            if(unflashAnimPlaying){
                FillVRAM((u16)' ', 0, (u16*)VRAM_BG_B, (64*32));
                //InitIntro();
                
                go_intro = false;
            }
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
