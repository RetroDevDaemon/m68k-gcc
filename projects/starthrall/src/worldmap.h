//worldmap.h
#include "bentgen.h"

const char wmeventnames[3][18] = {
    "ADV Scene test\x00",
    "Dungeon test\x00",
    "TK Nurse-21\x00"
};

struct WMEvent {
    struct xypos pos; // in tile, so x/16 y/16
    const char* name;
    const void* func;
};

extern enum screenModes;
void InitWorldMapState()
{
    // Set mode
    CUR_SCREEN_MODE = WORLDMAP;
    unflashAnimPlaying = true;
    flashStep = 0;
    // Palette
/*
    curPaletteSet[0] = &pal01.data;
    curPaletteSet[1] = &worldmappal.data;
    curPaletteSet[2] = &phaesta_wm_pal.data;
    ResetPalettes();
    XGM_startPlay(&worldmap1);
    VDP_setTextPlane(BG_B);
    //VDP_setTextPalette(2);
    // Make map
    tileindex = TILE_USERINDEX;   
    bgBaseTileIndex[0] = tileindex;
    VDP_loadTileSet(&worldmaptest, tileindex, DMA);
    tileindex += worldmaptest.numTile;
    worldmap = MAP_create(&worldmaptest_map, BG_A, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, bgBaseTileIndex[0]));
    
    wm_player = SPR_addSprite(&phaesta_wm, 160, 116, TILE_ATTR(PAL2, 0, 0, 0));

    MAP_scrollTo(worldmap, 160, 116);
    mapWidth = 100;
    mapHeight = 100;
    // set off screen real quick
    player.x = 320;
    player.y = 224;
    SPR_setPosition(wm_player, player.x, player.y);
    SPR_update();
    //SPR_setDepth(wm_player, 20);
    timer_1 = 0;
    timer_2 = 0;
    timer_3 = 0;

    cloudPos[0].x = random() % 320;
    cloudPos[0].y = random() % 200;
    wmClouds[0] = SPR_addSprite(&cloud, cloudPos[numClouds].x, cloudPos[numClouds].y, TILE_ATTR(PAL1, 1, 0, 0));
    wmShadows[0] = SPR_addSprite(&cloudshadow, cloudPos[numClouds].x, cloudPos[numClouds].y + 80, TILE_ATTR(PAL1, 1, 0, 0));
    SPR_setDepth(wmClouds[numClouds], SPR_MIN_DEPTH);
    SPR_setDepth(wmShadows[numClouds], SPR_MIN_DEPTH);
    numClouds++;
*/
    return;
}

/*
void LoadHillADVScene()
{
    JOY_setEventHandler(null);
    playerWalking = false;
    //MEM_free(worldmap);
    flashDarkAnimPlaying = true;
    flashStep = 0;
    callWhenFlashed = &InitNewADVScene;
    
    return;
}

void DrawLocationText(const char* name)
{
    u8 i = 0;
    while(name[i] != '\x00') i++;
    VDP_drawText(name, 19-(i/2), 10);
}

void LoadMtnDungeon()
{
    //VDP_drawText("dungeon scene", 12, 12);
    player.x = 7;
    player.y = 7;
    JOY_setEventHandler(null);
    playerWalking = false;
    MEM_free(worldmap);
    flashDarkAnimPlaying = true;
    flashStep = 0;
    callWhenFlashed = &InitDungeonState;
    return;
}

const struct WMEvent hill_demo = { 
    16, 13,
    wmeventnames[0],
    &LoadHillADVScene
};

const struct WMEvent hill_demo2 = { 
    17, 13,
    wmeventnames[0],
    &LoadHillADVScene
};

const struct WMEvent mtn_demo = {
    27, 12,
    wmeventnames[1],
    &LoadMtnDungeon
};

const struct WMEvent* worldmapevents[3] = {
    &hill_demo, &hill_demo2,
    &mtn_demo
};


void WorldMapCollision()
{
    for(u8 i = 0; i < sizeof(worldmapevents) / sizeof(void*); i++)
    {
        struct WMEvent* wm = worldmapevents[i];
        s16 tx = wm->pos.x * 16;
        s16 ty = wm->pos.y * 16;
        if ( ((player.x+8) >= tx) && (player.y >= ty) ){
            if( ((player.x+8) <= (tx+16)) && (player.y <= (ty+16)) ){
                //void(*worldMapActiveFunc)() = wm->func;
                worldMapActiveFunc = wm->func;
                DrawLocationText(wm->name);
                return;
            }
        }
    }
    // fall through, no event:
    VDP_clearTextArea(5, 5, 55, 10);
    worldMapActiveFunc = null;
    return;
}

const u8 collisiontiletypes[] = {
    16, 80, 90, 100, // water, tree
    115, 117, 116, // 
    72, 77, 63, 70, 78, 67, 76, 61, 71, // mountain
    82, 81, 92, 91 // big tree
};

bool IsCollisionTile(u16 t)
{
    for(u8 c = 0; c < sizeof(collisiontiletypes); c++)
        if (t == collisiontiletypes[c]) return true;
        
    if ( (t >= 120) && (t <= 128) )
        walkingInGrass = true;
    else walkingInGrass = false;
    return false;
}

void WorldMapUpdate()
{
    //// player and map scroll:
    // Tick timer for frame animation offset
    u8 f = 0;
    timer_1++;
    if(playerWalking) timer_1++;
    if(timer_1 < 30) f = 0;
    else if (timer_1 < 60) f = 1;
    else timer_1 = 0;
    // set direction 
    if(playerDirection == UP) f += 4;
    else if(playerDirection == RIGHT) f += 6;
    else if(playerDirection == LEFT) f += 2;
    // Move player 
    if(playerWalking)
    {
        // get tile number player is facing
        u16 t = 30000;    
        if(playerDirection == UP)
        {
            t = MAP_getMetaTile(worldmap, (player.x / 16), ((player.y-1) / 16));
            if(!IsCollisionTile(t)) {
                t = MAP_getMetaTile(worldmap, ((player.x+15) / 16), ((player.y-1) / 16));
                if(!IsCollisionTile(t)) {
                    player.y--;
                    for(u8 i = 0; i < 4; i ++) cloudPos[i].y++;
                }
            }
        }
        else if (playerDirection == DOWN)
        {
            t = MAP_getMetaTile(worldmap, (player.x / 16), (player.y+8) / 16);
            if(!IsCollisionTile(t)) {
                t = MAP_getMetaTile(worldmap, ((player.x+15) / 16), (player.y+8) / 16);
                if(!IsCollisionTile(t)){
                    player.y++;
                    for(u8 i = 0; i < 4; i ++) cloudPos[i].y--;
                }
            }
        }
        else if (playerDirection == RIGHT)
        {
            t = MAP_getMetaTile(worldmap, ((player.x+16) / 16), player.y / 16);
            if(!IsCollisionTile(t)) {
                t = MAP_getMetaTile(worldmap, ((player.x+16) / 16), (player.y+7) / 16);
                if(!IsCollisionTile(t)) {
                    player.x++;
                    for(u8 i = 0; i < 4; i ++) cloudPos[i].x--;
                }
            }
        }
        else if (playerDirection == LEFT)
        {
            t = MAP_getMetaTile(worldmap, ((player.x-1) / 16), player.y / 16);
            if(!IsCollisionTile(t)){
                t = MAP_getMetaTile(worldmap, ((player.x-1) / 16), (player.y+7) / 16);
                if(!IsCollisionTile(t)) {
                    player.x--;   
                    for(u8 i = 0; i < 4; i ++) cloudPos[i].x++;
                }
            }
        } // and map
        MAP_scrollTo(worldmap, player.x - 160, player.y - 112);
        WorldMapCollision(); // encounter if something is there TODO fix collision area
        uintToStr(t, &debug, 3);
        VDP_drawText(debug, 0, 0);
        
    }
    if(walkingInGrass) f += 8;
    SPR_setFrame(wm_player, f); // and set player frame 
    //// make clouds:
    timer_2++;
    if(timer_2 > 500){ 
        timer_2 = 0; 
        if(numClouds < 4){
            // spawn new cloud
            cloudPos[numClouds].x = 320;
            cloudPos[numClouds].y = random() % 200;
            wmClouds[numClouds] = SPR_addSprite(&cloud, cloudPos[numClouds].x, cloudPos[numClouds].y, TILE_ATTR(PAL1, 1, 0, 0));
            wmShadows[numClouds] = SPR_addSprite(&cloudshadow, cloudPos[numClouds].x, cloudPos[numClouds].y + 80, TILE_ATTR(PAL1, 1, 0, 0));
            SPR_setDepth(wmClouds[numClouds], SPR_MIN_DEPTH);
            SPR_setDepth(wm_player, 50);
            SPR_setPriorityAttribut(wm_player, TRUE);
            SPR_setDepth(wmShadows[numClouds], 100);
            //inc counter
            numClouds++;
        }
    }
    timer_3++;
    if(timer_3 > 3){
        timer_3 = 0;
        for(u8 i = 0; i < 4; i++)
        {
            if(wmClouds[i] != null){
                cloudPos[i].x--;
            }
        }
    }
    for(u8 i = 0; i < 4; i++) {
        if(wmClouds[i] != null){
            SPR_setPosition(wmClouds[i], cloudPos[i].x, cloudPos[i].y);
            SPR_setPosition(wmShadows[i], cloudPos[i].x, cloudPos[i].y+80);
        }
        if(( cloudPos[i].y < -88) || (cloudPos[i].y > 250) ){
            if(wmClouds[i] != null){
                SPR_releaseSprite(wmClouds[i]);
                SPR_releaseSprite(wmShadows[i]);
                wmClouds[i] = null;
                wmShadows[i] = null;
                numClouds--;
                cloudPos[i].y = 0; 
                cloudPos[i].x = 0;
            }
        }
    }
}
*/