//title.h


u8 introLine = 0;
u8 introScreen;

const char* introtxt[] = { 
"          It's the year 3199.\x00"
};
const char* introtxt2[] = {
"      For the last five centuries,\x00",
"     humankind has spread steadily\x00",
"      among the stars, thanks to\x00",
"       replicative technology.\x00",
};
const char* introtxt3[] = {
"       This means the bodies we\x00",
"      own are nothing more than\x00",
"            fabrications.\x00",
" \x00",
"       Ordered arrangements made\x00",
"       of unordered atoms, drawn\x00",
"        to the specification of\x00",
"             our DNA.\x00"
};
const char* introtxt4[] = {
"       The alternative, of course,\x00",
"        is to be physically bound\x00",
"       by your own matter for the\x00",
"     brief duration of its lifespan.\x00",
" \x00",
"       Needless to say, only the\x00",
"      poor or stupid choose this.\x00"
};
const char* introtxt5[] = {
"                  Me?\x00",
"\x00",
"         I travel those stars.\x00",
" \x00",
"              I question.\x00",
"                I seek.\x00"
};
const char* introtxt6[] = {
"            I can't explain\x00",
"           why, but Im driven\x00",
"          harder, further than\x00",
"         the others of my kind.\x00"
};
const char* introtxt7[] = {
"         What is it to be human?\x00"
};
const char* introtxt8[] = {
"          What really happened\x00",
"         to that little girl the\x00",
"         first time she stepped\x00",
"           into a teletranser?\x00"
};
const char* introtxt9[] = {
"       When she was stripped down\x00",
"         into ten billion pieces,\x00",
"       reassembled again from simple\x00",
"         data, light years away...\x00"
};
const char* introtxt10[] = {
"           Who did she become?\x00"
};
const char* introtxt11[] = {
"          My name is Phaesta.\x00",
"         I'm what people call\x00",
"            a 'scrapper'.\x00",
"\x00",
"           Despite the name,\x00",
"            it's noble work.\x00"
};
const char* introtxt12[] = {
"      I go to places other people\x00",
"      can't. Or places they're too\x00",
"        scared to. I do jobs for\x00",
"       them -- I collect, I fight.\x00",
"\x00",
"              I explore.\x00"
};
const char* introtxt13[] = {
"      The last thing I remember, \x00",
"     I was entering the atmosphere\x00",
"          of the planet Urs.\x00",
"\x00",
"       I must have hit some kind\x00",
"        of electrical storm and\x00 ",
"             blacked out...\x00"
};

void DrawIntroTxt(void* txt)
{
    VDP_drawText((const char*)txt, 0, introLine);
    introLine++;
}


void IntroTxtPart2()
{
    selector = SPR_addSprite(&greenled, selectorpos.x, selectorpos.y, TILE_ATTR(PAL1, 1, 0, 0));
    SPR_update();
}

void _introtxtfadein(bool tf){
    introTxtFadeIn = tf;
}

#define INTROLINE_BASE 18
#define INTROSEL_BASEY 150

signed int negr, negg;
signed long negb;



void InitIntro()
{

    if(titleScr != null){
        MAP_scrollTo(titleScr, 0, 0);
        MEM_free(titleScr);
        MEM_free(&titleSplashTest);
        
        //titleScr = MAP_create(&introimage_map, BG_A, TILE_ATTR_FULL(0, 0, 0, 0, ))
    }
    tileindex = TILE_USERINDEX;   
    bgBaseTileIndex[0] = tileindex;
    //VDP_loadTileSet(&introimage_0, tileindex, DMA);
    //tileindex += introimage_0.numTile;
    //titleScr = MAP_create(&introimage_map0, BG_A, TILE_ATTR_FULL(3, 0, FALSE, FALSE, bgBaseTileIndex[0]));
    //MAP_scrollTo(titleScr, 0, 0);
     // fix tile index
    sprBaseInd = tileindex;
    
    //// Test 1: Programmatically reduce color value (looks too red)
    /*
    negr = -10;
    negb = -10;
    negg = -10;

    for(u8 i = 0; i < 16; i++){ // BGR
        // split into rgb 
        signed int nr, ng; signed long nb;
        nr = intropal.data[i] & 0x000f;
        ng = (intropal.data[i] & 0x00f0) >> 4;
        nb = (intropal.data[i] & 0x0f00) >> 8;
        nr += negr; nr = max(nr, 0);
        nb += negb; nb = max(nb, 0);
        ng += negg; ng = max(ng, 0);
        ng = (ng << 4);
        nb = (nb << 8);
        palNegatives[i] = nr | ng | nb;

    }
    u16* p = &palNegatives;
    */
    curPaletteSet[3] = &intropal.data;
    ResetPalettes();
    
    
    XGM_startPlay(&opening);
    
    flashAnimPlaying = false;    
    unflashAnimPlaying = true;
    flashStep = 0;
    flashStepTimer = 0;
    
    CUR_SCREEN_MODE = INTRO;
    
    VDP_clearTextArea(0, 0, 63, 31);
    introLine = INTROLINE_BASE+2;
    selectorpos.x = 250;
    selectorpos.y = INTROSEL_BASEY+10;    
    introTxtFadeIn = true;
    
    AddQueue(&Wait, secs(1));
    AddQueue(&_introtxtfadein, 1);
    AddQueue(&DrawIntroTxt, introtxt[0]);
    AddQueue(&Wait, secs(2));
    AddQ(&IntroTxtPart2);

}

TileSet* introscrs[] = {
    (TileSet*)&introimage_0, (TileSet*)&introimage_1, (TileSet*)&introimage_2, 
    (TileSet*)&introimage_3, (TileSet*)&introimage_4, (TileSet*)&introimage_5, 
    (TileSet*)&introimage_6, (TileSet*)&introimage_7, (TileSet*)&introimage_8, (TileSet*)&introimage_9
};
void* intromaps[] = {
    (void*)&introimage_map0, (void*)&introimage_map1, (void*)&introimage_map2, 
    (void*)&introimage_map3, (void*)&introimage_map4, (void*)&introimage_map5, 
    (void*)&introimage_map6, (void*)&introimage_map7, (void*)&introimage_map8, (void*)&introimage_map9
};

void AdvanceIntroFrom(u8 i)
{
    MEM_free(titleScr);
    MEM_free(intromaps[i-1]);
    tileindex = TILE_USERINDEX;   
    bgBaseTileIndex[0] = tileindex;
    VDP_loadTileSet(introscrs[i], tileindex, DMA_QUEUE);
    tileindex += introscrs[i]->numTile;
    titleScr = MAP_create(intromaps[i], BG_A, TILE_ATTR_FULL(3, 0, FALSE, FALSE, bgBaseTileIndex[0]));
    MAP_scrollTo(titleScr, 0, 0);
    sprBaseInd = tileindex;

}

void DoIntro(u8 i)
{
    VDP_clearTextArea(0, 17, 63, 31);
    if(i <= 10) AdvanceIntroFrom(i-1);
    AddQueue(&_introtxtfadein, 1);
    if(i == 1){
        introLine = INTROLINE_BASE;
        selectorpos.x += 10;
        selectorpos.y += 5;

        for(u8 c = 0; c < 4; c++)
            AddQueue(&DrawIntroTxt, (void*)introtxt2[c]);
    }
    else if (i == 2){
        introLine = INTROLINE_BASE-1;
        selectorpos.y = INTROSEL_BASEY+55;
        for(u8 d = 0; d < 8; d++)
            AddQueue(&DrawIntroTxt, (void*)introtxt3[d]);
    }
    else if (i == 3){
        introLine = INTROLINE_BASE-2;
        selectorpos.y = INTROSEL_BASEY+40;
        for(u8 e = 0; e < 7; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt4[e]);
    }
    else if (i == 4){
        introLine = INTROLINE_BASE-2;
        selectorpos.y = INTROSEL_BASEY+20;
        for(u8 e = 0; e < 6; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt5[e]);
    }else if (i == 5){
        introLine = INTROLINE_BASE;
        selectorpos.y = INTROSEL_BASEY+20;
        for(u8 e = 0; e < 4; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt6[e]);
    }else if (i == 6){
        introLine = INTROLINE_BASE+2;
        selectorpos.y = INTROSEL_BASEY+25;
        AddQueue(&DrawIntroTxt, (void*)introtxt7[0]);
    }else if (i == 7){
        introLine = INTROLINE_BASE-1;
        selectorpos.y = INTROSEL_BASEY+30;
        for(u8 e = 0; e < 4; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt8[e]);
    }else if (i == 8){
        introLine = INTROLINE_BASE-1;
        selectorpos.y = INTROSEL_BASEY+30;
        for(u8 e = 0; e < 4; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt9[e]);
    }else if (i == 9){
        introLine = INTROLINE_BASE+2;
        selectorpos.y = INTROSEL_BASEY+15;
        AddQueue(&DrawIntroTxt, (void*)introtxt10[0]);
    }else if (i == 10){
        introLine = INTROLINE_BASE-1;
        selectorpos.y = INTROSEL_BASEY+30;
        for(u8 e = 0; e < 6; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt11[e]);
    }else if (i == 11){
        introLine = INTROLINE_BASE-1;
        selectorpos.y = INTROSEL_BASEY+35;
        for(u8 e = 0; e < 6; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt12[e]);
    }else if (i == 12){
        introLine = INTROLINE_BASE-1;
        selectorpos.y = INTROSEL_BASEY+35;
        
        for(u8 e = 0; e < 7; e++)
            AddQueue(&DrawIntroTxt, (void*)introtxt13[e]);
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
    AddQ(&IntroTxtPart2);
    
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
                curPaletteSet[0] = &palette_black;
                ResetPalettes();
            }
            if(introTxtTimer == 120){
                unflashAnimPlaying = false;
                unflashDarkAnimPlaying = false;
                curPaletteSet[0] = (const u16**)&textfade_a.data;
                ResetPalettes();
            }
            else if(introTxtTimer == 150){
                curPaletteSet[0] = (const u16**)&textfade_b.data;
                ResetPalettes();
            }
            else if(introTxtTimer == 180){
                JOY_setEventHandler(&JOY_ContinueIntroText);
                curPaletteSet[0] = (const u16**)&textfade_c.data;
                ResetPalettes();
            }
            else if (introTxtTimer == 190) introTxtTimer--;
        }
        if(introTxtFadeOut){
            // joy handler sets timer to 0
            introTxtTimer++;
            if(introTxtTimer == 1){
                curPaletteSet[0] = (const u16**)&textfade_b.data;
                ResetPalettes();
            }else if (introTxtTimer == 30){
                curPaletteSet[0] = (const u16**)&textfade_a.data;
                ResetPalettes();
            }else if (introTxtTimer == 60){
                curPaletteSet[0] = &palette_black;
                ResetPalettes();
                introScreen++;
                introTxtFadeOut = false;
                introTxtFadeIn = true;
                DoIntro(introScreen);
                
            }else if (introTxtTimer == 90){
                introTxtTimer--;
            }
        }
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
}