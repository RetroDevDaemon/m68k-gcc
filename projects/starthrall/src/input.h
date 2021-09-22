//#include "types.h"
//#include <genesis.h>
#include "bentgen.h"

#define null NULL
/////////////////////////////
///////
//////     INPUT HANDLERS 
/////
////////////////////////////
void battleJoyHandler(u16 JOYNO, u16 RELEASED, u16 PRESSED);
void waitForButton(u16 JOYNO, u16 RELEASED, u16 PRESSED);
void dungeonJoyHandler(u16 JOYNO, u16 RELEASED, u16 PRESSED);
void noJoyHandler(u16 JOYNO, u16 RELEASED, u16 PRESSED);
void selectEnemyToAttack(u16 JOYNO, u16 RELEASED, u16 PRESSED);
void worldMapJoyHandler(u16 JOYNO, u16 RELEASED, u16 PRESSED);

void Player_MoveCmd()
{
    return;
}

void Player_TechCmd()
{
    return;
}

void Player_ItemCmd()
{
    return;
}


/*
s8 GetEnemyAC(struct Enemy* e);

s8 GetEnemyAC(struct Enemy* e)
{
    fp32 ac = 0;
    ac = 10 * (256 - e->dexterity);
    return (s8)(ac >> 8);
}

void Player_AttackDo()
{
    struct enemyCombatant* e = &combatants.enemies[command_arr[nextCombatActor].target + 4];
    struct playerCombatant* p = &combatants.players[nextCombatActor];
    
    // Roll attack!
    last_tohit = p->stats.thaco - roll(20);

    s8 pac = GetEnemyAC(&e->stats);
    
    // Roll damage!
    if (pac <= last_tohit)
    {
        last_attack_hit = true;
        last_attack_dmg = GetPlayerPhysicalAtkDmg(&p->stats, &e->stats);
    } else { 
        last_attack_hit = false;
        last_attack_dmg = 0;
    }

    VDP_drawText("Roll:", 21, 22);
    intToStr(last_tohit, &debug, 2);
    VDP_drawText(&debug, 22, 23);

    VDP_drawText("vs", 22, 24);
    intToStr(pac, &debug, 2);
    VDP_drawText(&debug, 22, 25);

    return;
    
}

void Player_AttackSelected()
{
    VDP_clearTextArea(1, 11, 38, 1);
    for(u8 i = 0; i < 12; i++)
        if (combatants.enemies[i].facing == CURRENT_BATTLE_FACING)
            SPR_setPalette(combatants.enemies[i].spr, 0);
    combatants.players[activePlayer].pos.x = (160-32);
    combatants.players[activePlayer].slideSpd = 24;
    scrollOutActive = true;
    timer_1 = 0;
    attackSelectProc = false;
    command_arr[activePlayer].cmdfunc = &Player_AttackDo;
    command_arr[activePlayer].source = activePlayer;
    command_arr[activePlayer].target = (currentEnemySelected + (u8)(CURRENT_BATTLE_FACING*3)) + 4;// | 0x80;
    currentEnemySelected = 0;
    return;
}

void Player_AttackCmd()
{
    if(selector != null)
        SPR_releaseSprite(selector);
    selector = null;
    slidingBattleGUIOut = true;
    timer_1 = 0.0f;
    initBgMapDone = false;
    attackSelectProc = true;
    JOY_setEventHandler(&selectEnemyToAttack);
    return;
}

void* current_menu[20] = {
    &Player_AttackCmd,
    &Player_MoveCmd,
    &Player_TechCmd, 
    &Player_ItemCmd,
    NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL
};


void worldMapJoyHandler(u16 JOYNO, u16 RELEASED, u16 PRESSED)
{
    if(JOYNO == JOY_1)
    {
        if(PRESSED & BUTTON_DOWN)
        {
            playerWalking = true;
            playerDirection = DOWN;
        }
        else if ( (RELEASED & BUTTON_DOWN) && (playerDirection == DOWN) )
        {
            playerWalking = false;
        }
        if (PRESSED & BUTTON_UP)
        {
            playerWalking = true;
            playerDirection = UP;
        }
        else if ( (RELEASED & BUTTON_UP) && (playerDirection == UP) )
        {
            playerWalking = false;
        }
        if (PRESSED & BUTTON_RIGHT)
        {
            playerWalking = true;
            playerDirection = RIGHT;
        }
        else if ( (RELEASED & BUTTON_RIGHT) && (playerDirection == RIGHT) )
        {
            playerWalking = false;
        }
        if (PRESSED & BUTTON_LEFT)
        {
            playerWalking = true;
            playerDirection = LEFT;
        }
        else if ( (RELEASED & BUTTON_LEFT) && (playerDirection == LEFT) )
        {
            playerWalking = false;
        }
        if((PRESSED & BUTTON_C))
        {
            if(worldMapActiveFunc != null){
                worldMapActiveFunc();
                worldMapActiveFunc = null;
            }
        }
    }
}

void selectEnemyToAttack(u16 JOYNO, u16 RELEASED, u16 PRESSED)
{
    if(JOYNO == JOY_1)
    {
        if(PRESSED & BUTTON_C)
        { 
            JOY_setEventHandler(NULL);
            Player_AttackSelected();
        }
        else if (PRESSED & BUTTON_RIGHT)
        {
            VDP_clearTextArea(1, 11, 38, 1);
            currentEnemySelected++;
            if(currentEnemySelected > 2) currentEnemySelected = 0;
            while(combatants.enemies[currentEnemySelected].spr == null){
                currentEnemySelected++;
                if(currentEnemySelected > 2) currentEnemySelected = 0;
            }
            for(u8 i = 0; i < 3; i ++){
                SPR_setPalette(combatants.enemies[i].spr, 0);
            }
        }
        else if (PRESSED & BUTTON_LEFT)
        {
            VDP_clearTextArea(1, 11, 38, 1);
            currentEnemySelected--;
            if(currentEnemySelected < 0) currentEnemySelected = 2;
            while(combatants.enemies[currentEnemySelected].spr == null){
                currentEnemySelected--;
                if(currentEnemySelected < 0) currentEnemySelected = 2;
            }
            for(u8 i = 0; i < 3; i ++){
                SPR_setPalette(combatants.enemies[i].spr, 0);
            }
        }
    }
}

void battleJoyHandler(u16 JOYNO, u16 RELEASED, u16 PRESSED)
{
    if(JOYNO == JOY_1)
    {
        if(PRESSED & BUTTON_C)
        {
            void(*f)() = current_menu[select_number];
            f();
        } else if (PRESSED & BUTTON_DOWN) {
            select_number++;
            if (select_number >= menu_opts) { select_number = 0; }
            SPR_setPosition(selector, 8, selector_y_base + (8 * select_number));
        } else if (PRESSED & BUTTON_UP) {
            select_number--;
            if(select_number < 0) select_number = menu_opts-1;
            SPR_setPosition(selector, 8, selector_y_base + (8 * select_number));
        }
    }
    return;
}

void waitForButton(u16 JOYNO, u16 RELEASED, u16 PRESSED)
{
    if (JOYNO == JOY_1)
    {
        if (PRESSED & BUTTON_C) 
        { 
            if(selector!=null)
                SPR_releaseSprite(selector);
            selector = NULL;
            SPR_reset();
            if(NPC_waitingForButton){
                NPC_waitingForButton = false;
                if(CUR_SCREEN_MODE == DUNGEON)
                {
                    closingNPCWindow = true;
                    //screenUpdated = false;
                }
            }
        }
    }
}


void musicTestInput(u16 JOYNO, u16 RELEASED, u16 PRESSED)
{
    if(JOYNO == JOY_1)
    {
        if ( PRESSED & BUTTON_RIGHT)
        {
            if(currentSong < 10) currentSong++;
        }
        else if ( PRESSED & BUTTON_LEFT)
        {
            if(currentSong > 0) currentSong--;
        }
        else if ( (PRESSED & BUTTON_A) || (PRESSED & BUTTON_C) ) 
        {
            XGM_stopPlay();
            XGM_startPlay(ost[currentSong].musdat);
        }
    }
    return;
}

void titleJoyHandler(u16 JOYNO, u16 RELEASED, u16 PRESSED)
{
    if(JOYNO == JOY_1){ 
        if( PRESSED & BUTTON_START )
        {
            if ( PRESSED & BUTTON_Z ) {
                XGM_stopPlay();
                MAP_scrollTo(titleScr, 0, 0);
                MEM_free(titleScr);
                MEM_free(&titleSplashTest);
                CUR_SCREEN_MODE = MUSICTEST;
                VDP_clearPlane(BG_A, TRUE);
                VDP_clearPlane(BG_B, TRUE);
    
                tileindex = TILE_USERINDEX;   
                bgBaseTileIndex[0] = tileindex;
                VDP_loadTileSet(&soundtesttiles, tileindex, DMA);
                tileindex += soundtesttiles.numTile;
                curPaletteSet[1] = &soundtestpal.data;
                ResetPalettes();
                bgmusictestmap = MAP_create(&soundtest_map, BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, bgBaseTileIndex[0]));
                MAP_scrollTo(bgmusictestmap, 0, 0);

                JOY_setEventHandler(&musicTestInput);
                return;
            }
            JOY_setEventHandler(null);
            ///XGM_startPlay(&worldmap1);
            XGM_stopPlay();
            SYS_doVBlankProcess();
            flashAnimPlaying = true;
            NEXT_SCREEN_MODE = INTRO;
        }
        else if ( PRESSED & BUTTON_C )
        {
            JOY_setEventHandler(null);
            if(titleScr != null){
                MEM_free(titleScr);
                MEM_free(&titleSplashTest);
            }
            SYS_doVBlankProcess();
            flashAnimPlaying = true;
            NEXT_SCREEN_MODE = WORLDMAP;
        }
    }

}


void dungeonJoyHandler(u16 JOYNO, u16 RELEASED, u16 PRESSED)
{
    if (CUR_SCREEN_MODE == BATTLE) { 
        return;
    }
    struct xypos oldpos = player;
    if (JOYNO == JOY_1)
    {
        if (PRESSED & BUTTON_A) 
        { 
            flashAnimPlaying = true;
            // TODO change to CURRENT PLAYING SONG POINTER
            if(CUR_SCREEN_MODE == DUNGEON){
                MEM_free(&portgui);
                MEM_free(&wander1);
                if(gui != null)
                    MEM_free(gui);
                if(gui2 != null)
                    MEM_free(gui2);
            }
            CUR_SCREEN_MODE = BATTLE;
        } 
        if (PRESSED & BUTTON_UP) {
            if ( !CheckDungeonCollision(0) ) { //player, playerFacing);
                if (playerFacing == NORTH) {
                    if (player.y > 0) player.y--;
                } else if (playerFacing == EAST) {
                    if (player.x < (mapWidth-1)) player.x++;
                } else if (playerFacing == WEST) {
                    if (player.x > 0) player.x--;
                } else if (playerFacing == SOUTH) {
                    if(player.y < (mapHeight-1)) player.y++;
                }
                if ( (player.y != oldpos.y) || (player.x != oldpos.x) ) {
                    if (stepSwitch) stepSwitch = false;
                    else { stepSwitch = true; }
                    screenUpdated = false;
                    executeTile = true;
                    if(playerFacing == NORTH) UpdateTextLog("> North");
                    if(playerFacing == SOUTH) UpdateTextLog("> South");
                    if(playerFacing == EAST) UpdateTextLog("> East");
                    if(playerFacing == WEST) UpdateTextLog("> West");
                }
            } else {
                UpdateTextLog("\"Ouch!\"");
                logUpdated = false;
            }
        } 
        else if (PRESSED & BUTTON_DOWN) {
            if ( !CheckDungeonCollision(1) ) {
                if (playerFacing == NORTH) {
                    if (player.y < (mapHeight-1)) player.y++;
                } else if (playerFacing == EAST) {
                    if (player.x > 0) player.x--;
                } else if (playerFacing == WEST) {
                    if (player.x < (mapWidth-1)) player.x++;
                } else if (playerFacing == SOUTH) {
                    if(player.y > 0) player.y--;
                }
                if ((player.y != oldpos.y) || (player.x != oldpos.x)) {
                    if (stepSwitch) stepSwitch = false;
                    else { stepSwitch = true; }
                    screenUpdated = false;
                    executeTile = true;
                    UpdateTextLog("> Backstep");
                }
            } else {
                UpdateTextLog("\"Ouch!\"");
                logUpdated = false;
            }
        } 

        if (PRESSED & BUTTON_RIGHT) {
            
            if(playerFacing == WEST) { playerFacing = NORTH; }
            else { playerFacing ++; }
            screenUpdated = false;
        } else if (PRESSED & BUTTON_LEFT) {
            if (playerFacing == NORTH) { playerFacing = WEST; }
            else { playerFacing--; }
            screenUpdated = false;
        } 
    }
    
}

void noJoyHandler(u16 JOYNO, u16 RELEASED, u16 PRESSED)
{
    return;
}
*/