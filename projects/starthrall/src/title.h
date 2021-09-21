bool title_intro_done;
bool go_intro;
extern u32 last_joyState1, joyState1;
extern int (*ProcessInput)(void);
const char str_pressStart[] = "PRESS START";
extern void NullInputHandler(void);

#include "maptest2.h"

void FillVRAM(u16 ti, u8 pal, u16* startaddr, u16 len);
int TitleInputHandler(void);
void InitTitleScreen(void);
void TITLE_DRAW(void);

void FillVRAM(u16 ti, u8 pal, u16* startaddr, u16 len)
{
    SetVRAMWriteAddress(startaddr);
    for(int i = 0; i < len; i++)
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
    u16 c;
    u32* cr;
    // start title music
    // TODO 
    // scroll map
    go_intro = false;
    title_intro_done = false;
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
                // if done, print :
                SetVRAMWriteAddress(0xc000 + (64*21*2) + (9*2)); // Screen address + 21 Y, 9 X (BG_A)
                // (16bit, 64 chars/map)
                u8* chp = (u8*)&str_pressStart[0];       // String address
                for(c = 0; c < sizeof(str_pressStart); c++) WRITE_DATAREG16((u16)*chp++); // Loop
                //SetInputCallback(&GetInput);
                title_intro_done = true;
            }
        }
    }
    else
    {
        if(go_intro)
        {
            if(unflashAnimPlaying){
                FillVRAM((u16)' ', 0, VRAM_BG_B, (64*32));
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
