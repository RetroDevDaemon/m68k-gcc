//intro.h
extern u16 blankpalette[16];
extern struct xypos selectorpos;

// INTRO RAM USAGE
u8 introLine = 0;
u8 introScreen;


const char introtxt[] = { 
"          It's the year 3199.\n"
};
const char introtxt2[] = {
"      For the last five centuries,\n\
     humankind has spread steadily\n\
      among the stars, thanks to\n\
       replicative technology.\x00"
};
const char introtxt3[] = {
"       This means the bodies we\n\
      own are nothing more than\n\
            fabrications.\n\
 \n\
       Ordered arrangements made\n\
       of unordered atoms, drawn\n\
        to the specification of\n\
             our DNA.\x00"
};

const char introtxt4[] = {
"       The alternative, of course,\n\
        is to be physically bound\n\
       by your own matter for the\n\
     brief duration of its lifespan.\n\
\n\
       Needless to say, only the\n\
      poor or stupid choose this.\x00"
};
const char introtxt5[] = {
"                  Me?\n\
\n\
         I travel those stars.\n\
\n\
              I question.\n\
                I seek.\x00"
};
const char introtxt6[] = {
"            I can't explain\n\
           why, but Im driven\n\
          harder, further than\n\
         the others of my kind.\x00"
};
const char introtxt7[] = {
"         What is it to be human?\x00"
};
const char introtxt8[] = {
"          What really happened\n\
         to that little girl the\n\
         first time she stepped\n\
           into a teletranser?\x00"
};
const char introtxt9[] = {
"       When she was stripped down\n\
         into ten billion pieces,\n\
       reassembled again from simple\n\
         data, light years away...\x00"
};
const char introtxt10[] = {
"           Who did she become?\x00"
};
const char introtxt11[] = {
"          My name is Phaesta.\n\
         I'm what people call\n\
            a 'scrapper'.\n\
\n\
           Despite the name,\n\
            it's noble work.\x00"
};
const char introtxt12[] = {
"      I go to places other people\n\
      can't. Or places they're too\n\
        scared to. I do jobs for\n\
       them -- I collect, I fight.\n\
\n\
              I explore.\x00"
};
const char introtxt13[] = {
"      The last thing I remember,\n\
     I was entering the atmosphere\n\
          of the planet Urs.\n\
\n\
       I must have hit some kind\n\
        of electrical storm and\n\
             blacked out...\x00"
};

extern struct _textsys ScriptSys;

void printscript(const char* str)
{
    // add to print buffer that putc n times per frame
    u8* c = (u8*)str;
    while(*c != '\x00')
    {
        // Increment text script buffer pointer and assign char with color 
        ScriptSys.text_buffer[ScriptSys.buffer_ptr++] = (u16)((*c++) | pal_no(TEXT_PALETTE));

        // reset the buffer pointer if it might overflow 
        if(ScriptSys.buffer_ptr == 1024) 
            ScriptSys.buffer_ptr = 0;
    }
    
}

#define SetupScriptWin(sx, sy, ex, ey) \
    ScriptSys.startx = sx;\
    ScriptSys.starty = sy;\
    ScriptSys.txt_x = sx;\
    ScriptSys.txt_y = sy;\
    ScriptSys.x_bound = ex;\
    ScriptSys.y_bound = ey;

void DrawIntroTxt(const char* txt)
{
    // Sets the upper left and lower right boundaries on screen in tile size:
    SetupScriptWin(0, introLine, 35, 23);
    // one char per frame 
    ScriptSys.textspeed = 1;

    printscript(txt); // < actually adds to buffer
    
    introLine++;
}

void IntroTxtPart2()
{
    //if(!spr_selector)
        
}

void _introtxtfadein(bool tf){
    introTxtFadeIn = tf;
}

#define INTROLINE_BASE 18
#define INTROSEL_BASEY 150

signed int negr, negg;
signed long negb;

#define RESET_BGA_SCROLL bga_hscroll_pos = 0; bga_vscroll_pos = 0;
#define RESET_BGB_SCROLL bgb_hscroll_pos = 0; bgb_vscroll_pos = 0;


TileSet* introscrs[] = {
    (TileSet*)&introimg0_0, (TileSet*)&introimg1_0, (TileSet*)&introimg2_0, 
    (TileSet*)&introimg3_0, (TileSet*)&introimg4_0, (TileSet*)&introimg5_0, 
    (TileSet*)&introimg6_0, (TileSet*)&introimg7_0, (TileSet*)&introimg8_0, (TileSet*)&introimg9_0
};
u8 introsizes[] = {
    49, 89, 141, 161, 165, 165, 155, 166, 170, 170
};
void* intromaps[] = {
    (void*)&introimg0_map, (void*)&introimg1_map, (void*)&introimg2_map, 
    (void*)&introimg3_map, (void*)&introimg4_map, (void*)&introimg5_map, 
    (void*)&introimg6_map, (void*)&introimg7_map, (void*)&introimg8_map, (void*)&introimg9_map
};

void InitIntro()
{
    WaitVBlank();
    
    flashAnimPlaying = false;
    unflashAnimPlaying = false;

    curPaletteSet[3] = (u16*)&intropal;
    LoadPalette(3, curPaletteSet[3]);
    
    CUR_SCREEN_MODE = INTRO;
    //ProcessInput = NullInputHandler;
    DisableVBlankIRQ();
    
    LoadSong(freefall);

    tileindex = 129;
    tileindex = VDPLoadTiles(tileindex, (u32*)&introimg0_0, 49);
    TEXT_PALETTE = 0;
    
    
    introLine = INTROLINE_BASE+2;
    selectorpos.x = 250;
    selectorpos.y = INTROSEL_BASEY+10;    
    introTxtFadeIn = true;

    CLEAR_BG(VRAM_BG_B);

#define INTROIMGWIDTH 14

    for(u16 y = 0; y < 14; y++){
        SetVRAMWriteAddress(VRAM_BG_A + (64 * 2 * (5 + y) + (2 * 10)));
        for(u16 x = 0; x < INTROIMGWIDTH; x++)
            WRITE_DATAREG16((u16)((introimg0_map[(y * INTROIMGWIDTH) + x] + 129) | pal_no(3)));
    }

    EnableVBlankIRQ();

    AddQueue(&Wait, secs(1));
    AddQueue(&_introtxtfadein, 1);
    AddQueue(&DrawIntroTxt, &introtxt);
    AddQueue(&Wait, secs(2));
    AddQueue(&IntroTxtPart2, 0);
}


void AdvanceIntroFrom(u8 i)
{
    //WaitVBlank();
    //VDPLoadTiles(128, (u32*)&led_green_0, 1);
    tileindex = 129; //TILE_USERINDEX;   
    VDPLoadTiles(tileindex, intromaps[i], introsizes[i]);
    for(u8 y = 0; y < 12; y++)
    {
        SetVRAMWriteAddress(VRAM_BG_B + (64 * 2 * introLine) + 0);
        for(u8 c = 0; c < 12; c++)
        {
            WRITE_DATAREG16((u16)(129 + introimg0_map[(y*12)+c])|(pal_no(2)));
        }
    }
}

void DoIntro(u8 i)
{
    FillVRAM((u16)' ', 0, (u16*)VRAM_BG_A, (64*32));
    
    if(i <= 10) AdvanceIntroFrom(i-1);
    
    AddQueue(&_introtxtfadein, 1);

    if(i == 1){
        introLine = INTROLINE_BASE;
        selectorpos.x += 10;
        selectorpos.y += 5;

        //for(u8 c = 0; c < 4; c++)
            AddQueue(&DrawIntroTxt, (void*)introtxt2);
    }
    else if (i == 2){
        introLine = INTROLINE_BASE-1;
        selectorpos.y = INTROSEL_BASEY+55;
        //for(u8 d = 0; d < 8; d++)
            AddQueue(&DrawIntroTxt, (void*)introtxt3);
    }
    else if (i == 3){
        introLine = INTROLINE_BASE-2;
        selectorpos.y = INTROSEL_BASEY+40;
        //for(u8 e = 0; e < 7; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt4);
    }
    else if (i == 4){
        introLine = INTROLINE_BASE-2;
        selectorpos.y = INTROSEL_BASEY+20;
        //for(u8 e = 0; e < 6; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt5);
    }else if (i == 5){
        introLine = INTROLINE_BASE;
        selectorpos.y = INTROSEL_BASEY+20;
        //for(u8 e = 0; e < 4; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt6);
    }else if (i == 6){
        introLine = INTROLINE_BASE+2;
        selectorpos.y = INTROSEL_BASEY+25;
        AddQueue(&DrawIntroTxt, (void*)introtxt7);
    }else if (i == 7){
        introLine = INTROLINE_BASE-1;
        selectorpos.y = INTROSEL_BASEY+30;
        //for(u8 e = 0; e < 4; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt8);
    }else if (i == 8){
        introLine = INTROLINE_BASE-1;
        selectorpos.y = INTROSEL_BASEY+30;
        //for(u8 e = 0; e < 4; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt9);
    }else if (i == 9){
        introLine = INTROLINE_BASE+2;
        selectorpos.y = INTROSEL_BASEY+15;
        AddQueue(&DrawIntroTxt, (void*)introtxt10);
    }else if (i == 10){
        introLine = INTROLINE_BASE-1;
        selectorpos.y = INTROSEL_BASEY+30;
        //for(u8 e = 0; e < 6; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt11);
    }else if (i == 11){
        introLine = INTROLINE_BASE-1;
        selectorpos.y = INTROSEL_BASEY+35;
        //for(u8 e = 0; e < 6; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt12);
    }else if (i == 12){
        introLine = INTROLINE_BASE-1;
        selectorpos.y = INTROSEL_BASEY+35;
        
        //for(u8 e = 0; e < 7; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt13);
    }else if (i == 13)
    {
        introTxtFadeIn = false;
        CUR_SCREEN_MODE = WORLDMAP;   
        NEXT_SCREEN_MODE = WORLDMAP;
        AddQueue(&Wait, secs(1.5));
        AddQueue(&InitWorldMapState, 0);
        return;
    }
    AddQueue(&Wait, secs(2));
    AddQueue(&IntroTxtPart2, 0);
    
}


void JOY_ContinueIntroText()
{
    if(Joy1Down(BTN_C_PRESSED)){
        introTxtTimer = 0;
        introTxtFadeIn = false;
        introTxtFadeOut = true;
        // fixme?
        spr_selector->y_pos = 212+128;
        ScriptSys.buffer_ptr = 0;
        ScriptSys.print_ptr = 0;
        ProcessInput = NullInputHandler;
    }
}


void INTRO_DRAW()
{
    
    if(introTxtFadeIn)
        {
            // tick up the timer
            introTxtTimer++;
            // if its over x, swap palette
            if(introTxtTimer == 1){
                curPaletteSet[1] = curPaletteSet[0];
                curPaletteSet[0] = (u16*)&blankpalette;
                ResetPalettes();
            }
            if(introTxtTimer == 120){
                unflashAnimPlaying = false;
                unflashDarkAnimPlaying = false;
                curPaletteSet[0] = (u16*)&introtxtpal_a;// (const u16**)&textfade_a.data;
                ResetPalettes();
            }
            else if(introTxtTimer == 150){
                curPaletteSet[0] = (u16*)&introtxtpal_b;//(const u16**)&textfade_b.data;
                ResetPalettes();
            }
            else if(introTxtTimer == 180){
                ProcessInput = JOY_ContinueIntroText;
                curPaletteSet[0] = (u16*)&introtxtpal_c;//(const u16**)&textfade_c.data;
                ResetPalettes();
            }
            else if (introTxtTimer == 190) introTxtTimer--;
        }
        if(introTxtFadeOut){
            // joy handler sets timer to 0
            introTxtTimer++;
            if(introTxtTimer == 1){
                curPaletteSet[0] = (u16*)&introtxtpal_b; //(const u16**)&textfade_b.data;
                ResetPalettes();
            }else if (introTxtTimer == 30){
                curPaletteSet[0] = (u16*)&introtxtpal_a; //(const u16**)&textfade_a.data;
                ResetPalettes();
            }else if (introTxtTimer == 60){
                curPaletteSet[0] = (u16*)&blankpalette;
                ResetPalettes();
                introScreen++;
                introTxtFadeOut = false;
                introTxtFadeIn = true;
                DoIntro(introScreen);
                
            }else if (introTxtTimer == 90){
                introTxtTimer--;
            }
        }
        /*
        if(selector != null){
            
            SPR_setPosition(selector, selectorpos.x, selectorpos.y);
            if(ticker == 10){
                SPR_releaseSprite(selector);
                selector = SPR_addSprite(&redled, selectorpos.x, selectorpos.y, TILE_ATTR(1, 1, 0, 0));
            }else if (ticker == 20){
                SPR_releaseSprite(selector);
                selector = SPR_addSprite(&greenled, selectorpos.x, selectorpos.y, TILE_ATTR(1, 1, 0, 0));
                ticker = 0;
            }
        }
        // swap dat ticky sprite
        SPR_update();
        */
}