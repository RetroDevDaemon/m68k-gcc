
//#include <genesis.h>
//#include "bent-sgdk.h"
//#include "gfx.h"
//#include "sprite.h"
//#include "music.h"

#define false FALSE 
#define true TRUE 

#define NTSC 60
#define PAL 50

#define XOFS 17
#define YOFS 7
#define FLOOR_1 1
#define FLOOR_2 2
#define FLOOR_3 3
#define WALL_1 0x10
#define WALL_2 0x11

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define PARTY_TO_COMBAT(pno,slot) memcpy(&combatants.players[slot].stats, &party[pno], sizeof(struct Player));
#define ENEMY_TO_COMBAT(ename, no) memcpy(&combatants.enemies[no].stats, &ename, sizeof(ename));

#define TYPE_NPC 1 
#define TYPE_PC 0
#define TYPE_ADV 2

#define ADV_IMG_TILEINDEX bgBaseTileIndex[4]  

#define HERO_FACE_TILEINDEX bgBaseTileIndex[2]
#define NPC_FACE_TILEINDEX bgBaseTileIndex[3]

// GLOBALZ

struct xypos {
    s16 x;
    s16 y;
};

bool disp;
u8 framerate = NTSC;
void(*callWhenFlashed)();
//bool stepSwitch = false;
s8 menu_opts;
s8 select_number;
u8 selector_y_base;
u8 activePlayer;
s16 last_attack_dmg;
struct xypos player;
bool initBgMapDone = false;
u8 mapWidth = 16;
u8 mapHeight = 16;
bool logUpdated = false;
s8 playerFacing = WEST;
char blankLine[25] = ">                       \x00";
u8 map[1024];
bool slidingBattleGUIOut;
u8 CURRENT_BATTLE_FACING;
s8 battleIdleWait_Timer;
s16 timer_3;
Map* titleScr;
bool attackSelectProc;
bool scrollOutActive;
bool scrollInNext;
bool unflashDarkAnimPlaying;
bool startPlayingCombat;
bool sliding_out_attacking_player;
bool flashDarkAnimPlaying;
bool battleNextPlayerInput;
bool last_attack_hit;
enum Directions { UP, RIGHT, DOWN, LEFT } playerDirection;
bool playerWalking;
u8 currentSong = 0;
u16 testno;

const char** songnames[] = {
    "Neon Rider\x00",
    "Battle\x00",
    "Giger Train\x00",
    "Caustic Love\x00",
    "Tennis Man\x00",
    "Victory\x00",
    "Dungeon Explore B\x00",
    "Northern Lights\x00",
    "Freefall\x00",
    "Zelma\x00",
    "Mr. WobbleBones\x00"
};
typedef struct song { 
    //void* name;
    void* musdat;
} Song;
/*
Song ost[] = {
    &battle1,
    &boss1,
    &boss2,
    &title1,
    &tennisman,
    &victory,
    &wander1,
     &worldmap1,
    &opening,
    &zelma,
    &wobblebones,
};
*/
u8 numClouds = 0;
Sprite* wmClouds[4];
Sprite* wmShadows[4];
struct xypos cloudPos[4];

u16 palNegatives[16]; 

//#define CRAMRGB(R, G, B) (u16)((B<<8)|(G<<4)|(R<<0))


const u16 fullblackpal[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static u16* curPaletteSet[4];
// FUNCIONS
void CullMapProjection();
u16 UpdateDungeonScreen(u16 i, u16 s);
bool screenUpdated = false;
void GetMapProjection();
void dungeonJoyHandler(u16 JOYNO, u16 RELEASED, u16 PRESSED);
void noJoyHandler(u16 JOYNO, u16 RELEASED, u16 PRESSED);
void UnflashAllPalettes();
void UpdateTextLog(const char* toAdd);
void LoadDungeonMap(const u8* dungeon);
void ResetPalettes();
void waitForButton(u16 JOYNO, u16 RELEASED, u16 PRESSED);
void Dungeon_UpdateLog(const char** l);
bool CheckDungeonCollision(u8 backwards);
void OpenADVScreen(void* env[], TileSet* img);
void LoadFace(TileSet* image, u8 type);
void InitWorldMapState();
void(*worldMapActiveFunc)();
bool walkingInGrass;
struct xypos selectorpos;


#define fp32tos8(n) (s8)(n >> 16) // Truncates top byte!


// Sprite stuff
u16** sprTileIndexes[6]; // animation index table for enemies (static VRAM loading)
u16 bgBaseTileIndex[9]; // bg start tile index
Map* gui;
Map* gui2;
Map* worldmap;
Sprite* wm_player;
enum screenModes { DUNGEON, BATTLE, TITLE, WORLDMAP, INTRO, ADVENTURE, MUSICTEST };
enum screenModes CUR_SCREEN_MODE;// = DUNGEON;
enum screenModes NEXT_SCREEN_MODE;
s32 ticker;
fp32 spriteSlideTimer;
u8 spriteSlideTick;
bool scrollInCombatants;
s8 battleGUISlideFrame;
fp32 timer_1;
fp32 timer_2;
u8 MAP1_FLAG1;
bool stepSwitch = false;
bool closingNPCWindow;
bool flashAnimPlaying = false;
fix32 flashStepTimer = 0;
//float flashStepTimer = 0;
u8 flashStep = 0;
bool unflashAnimPlaying = false;
bool executeTile = false;
bool backgroundUpdated = false;
bool normalBattBgScroll = false;
bool battleTest = false;
u16 tileindex;
u16 numTile;
u16 sprBaseInd;
bool battleMenuOpen = false;
bool battleNewTurn;
bool openingNPCWindow;
bool culled[25];
u8 viewProjection[25];
u8 npcwindow_tick;
s8 currentEnemySelected;
Sprite* numbertest;
s8 last_tohit;
bool sliding_out_tgtplayer;
bool doNextActor;
bool playing_playerBattleAction;
bool introTxtFadeOut;
u8 introTxtTimer = 0;
bool introTxtFadeIn = false;
Map* bgmusictestmap;

//#define frameDelta fp32(1/60)
#define frameDelta_f32 1092

Sprite* selector;
u8 selectorTick;

struct MapEvent* currentMapEvent;
struct NPCData* currentNPC;
char debug[10];
char textLog[2500];
// SCRIPT BYTE CODES:
#define PRINT '\x81'
#define WAIT_FOR_BTN '\x82'
#define PHAESTA_NAME '\x91'
#define P2_NAME '\x92'
#define P3_NAME '\x93'
#define P4_NAME '\x94'
#define P5_NAME '\x95'
#define P6_NAME '\x96'
#define P7_NAME '\x97'
#define P8_NAME '\x98'

const char playerNames[4][16] = {
    "Phaesta\x00       ", 
    "Midge\x00         ", 
    "Ardra\x00         ",
    "Butt\x00          "
};

char procLine[25];
char waitingToPrint[25];
u8 printLoc; 
bool NPC_waitingForButton;
struct DamageNumber { 
    //struct xypos pos;
    Sprite* spr;
    u8 mytick; 
} damageNumbers[4];
// ENMS
enum ElementTypes { Phtn,
                    Wave, 
                    Cold, 
                    Qntm } elementTypes;
enum SpecialMods { NONE, 
                   BLIND, 
                   STUN, 
                   FREEZE, 
                   WARP, 
                   DEXPLUS1, 
                   DEXPLUS2, 
                   DEXPLUS3 } specialMods; 

enum eventType { COMBAT = 0, NPC_CHAT = 1, NPC_SHOP = 2, ADV_SCENE = 3 };

// Weapons are 1 of 2 equipment types
struct Weapon { 
    char* name;
    u8 equippable; // bit field 00001111
    u8 power;
    enum ElementTypes elem; 
};

// Soldier - increases war skill
// Ranger - increases survival skill
// Hacker - increases techno skill
struct BodyPiece {
    char* name;
    u8 soldier;
    u8 ranger;
    u8 hacker;
    u8 def;
    enum ElementTypes elem;
    enum SpecialMods mod;
};

struct PlayerBody {
    struct BodyPiece* arm;
    struct BodyPiece* leg;
    struct BodyPiece* body;
    struct BodyPiece* core;
    struct BodyPiece* mind;
};

/*
e.g.
arm = 
Replicant Arm, 2, 1, 0, 1, elem.phtn, mod.none
*/
///////////////////////////////////
///////////////// * ADV STRUCTS * /
// All three structs are identical, but are separate for
// purposes of coding cleanliness.
// Functions are generally used for adding other objects
//  to the current available in the scene.
// Actionees:
/*
struct ADVExamine { // for ADVObject / ADVRoom
    // Print description when examined. 
    //  Run func(params[0], params[1]) if they are not NULL.
    const char* name;
    const char* description;
    void* func;     // script if necessary
    u8 params[2]; //set flag, check if flag etc.
};
struct ADVDialogue { // for ADVNPC
    // Print line when asked about subject.
    //  As above, run func if not NULL
    const char* subject;
    const char* line; 
    void* func;
    u8 params[2];
};
*/
#define ADVACT_EXAMINE 0
#define ADVACT_DIALOGUE 1
#define ADVACT_ACTION 2

struct ADVEvent { 
    // Print desc when action is performend and run func if not NULL.
    u8 actionType;  
    const char** name;
    const char** desc;
    void* func;
    u8 params[2];
};
// Objects:
struct ADVRoom {
    // Just a list of near rooms. Makes it super efficient.
    const struct ADVRoom* exits[4];
    // You can always examine your environment.
    const struct ADVEvent* look;
    const void* contains[4]; //npc or objects
};
struct ADVNPC {
    // NPCs display their pic, have an examine and dialogue options.
    TileSet* image;
    struct ADVEvent* look;
    struct ADVEvent* talks[4];
};
struct ADVObject {
    struct ADVEvent* action;
    struct ADVEvent* look;
};



struct LvlUp { // defined per character 
    fp16 hpmod;
    fp16 mpmod;
    fp16 xpmod; // multiplier for next level
    fp16 strmod;
    fp16 intmod;
    fp16 dexmod;
    fp16 thacomod;
};

struct Skill {
    char* name;
    char* desc;
    u8 cost;
    u8 req_sold;
    u8 req_hack;
    u8 req_rang;
    u8* script; // byte field for scripting engine
};

struct Spell {
    char* name;
    char* desc;
    u8 cost;
    u8* script;
};

struct Player {
    const char* name; 
    u16 baseHP;
    u16 baseMP;
    u32 xp;
    u8 lvl;
    fp16 strength;
    fp16 intellect;
    fp16 dexterity; // multiplier - starts at 0, display is *100
    u8 thaco;
    struct Weapon* weap;
    struct PlayerBody body;
    struct LvlUp* growth;
    const SpriteDefinition* spr[3];
    u8 palette;
};
s8 GetArmorClass(struct Player* p);
//s8 GetEnemyAC(struct Enemy* e);

struct Enemy {
    const char* name;
    u16 hp;
    //u16 maxhp;
    u16 xp;
    fp16 strength;
    fp16 intellect;
    fp16 dexterity;
    u8 defense;
    enum ElementTypes elem;
    u8 thaco;
    const SpriteDefinition* spr;
    u8 palette;
    void* ai_loop[5];
    u8 ai_type; // 0 = random, 1 = linear
    void* animScript;
    void* treasureItem;
    u8 dropRate; // 0-255 (255 = 2.55x)
    s8 a;
}; 
struct playerCombatant {
    struct xypos pos;
    bool slidingIn;
    u8 slideSpd;
    u8 facing; // 0-3 NESW
    Sprite* spr;
    struct Player stats; // party 0-3
};

struct enemyCombatant {
    struct xypos pos;
    bool slidingIn;
    u8 slideSpd;
    u8 facing; // 0-3 NESW
    Sprite* spr;
    struct Enemy stats;
};

static struct Player party[4];
struct Combatants { 
    struct playerCombatant players[(s8)4];
    struct enemyCombatant enemies[(s8)12];
} combatants;
void* initiative_arr[(s8)16]; // playerCombatant or enemyCombatant
fp32 initiative_rolls[(s8)16];
u8 nextCombatActor;
bool playing_enemyBattleAction;
/*
    e.g. 
    
    baseHP = 20
    LvlUp x 2 of hpmod = 1.2
    28 or 29
    
    str = 0.8, strmod = 1.1
    = 0.88 * 1.1 = 0.97, 1.06, 1.17, 1.28, 1.41
    
    thaco = 20
    thacomod is added each level e.g. -0.75
    20 - 0.75 - 0.75 = 18.5 (max it)

*/
#define SET_FLAG(flag, group) (group = group | (1 << flag))
#define CLEAR_FLAG(flag, group) (group = group & (0xff ^ (1 << flag)))
#define TOGGLE_FLAG(flag, group) (group = group ^ (1 << flag))

struct MapEvent {
    struct xypos pos; // pos on map
    const void* function; // FUNCTION poniter
    const void* params[2]; //2 ptrs
};

struct NPCData {
    //struct xypos tilemappos;
    const TileSet* face;
    const char** script; // define....
    u8 scriptLines;
};

void OpenNPCChat(struct NPCData* npc);

// Used in draw routine iteration
struct viewSprite {
    Sprite* spr;
    u16** index;
};

struct viewSprite dungeonview[25];

struct dungeonTile {
    const SpriteDefinition* data;
    const struct xypos pos;
};
        

struct CombatCommands { 
    void* cmdfunc;
    u8 source;
    u8 target; // & 0x80 for monsters.
} command_arr[16];


// Hard definitions:
/*
const SpriteDefinition* floor_tiles_1[20] = {
    &floor1_9, &floor1_7, &floor1_3, &floor1_7, &floor1_9,
    &floor1_8, &floor1_6, &floor1_2, &floor1_6, &floor1_8,
    NULL, &floor1_5, &floor1_1, &floor1_5, NULL,
    NULL, &floor1_4, &floor1_0, &floor1_4, NULL
};

const SpriteDefinition* floor_tiles_2[20] = {
    &floor2_9, &floor2_7, &floor2_3, &floor2_7, &floor2_9,
    &floor2_8, &floor2_6, &floor2_2, &floor2_6, &floor2_8,
    NULL, &floor2_5, &floor2_1, &floor2_5, NULL,
    NULL, &floor2_4, &floor2_0, &floor2_4, NULL
};

const SpriteDefinition* floor_tiles_3[20] = {
    &floor3_9, &floor3_7, &floor3_3, &floor3_7, &floor3_9,
    &floor3_8, &floor3_6, &floor3_2, &floor3_6, &floor3_8,
    NULL, &floor3_5, &floor3_1, &floor3_5, NULL,
    NULL, &floor3_4, &floor3_0, &floor3_4, NULL
};

struct dungeonTile floorDisplay[20] = { // add 5
    {NULL, 0, 63},
    {NULL, 14, 63},
    {NULL, 42, 63},
    {NULL, 60, 63},
    {NULL, 82, 63},

    {NULL, 0, 70},
    {NULL, 0, 70},
    {NULL, 32, 70},
    {NULL, 66, 70},
    {NULL, 97, 70},
    
    {NULL, 0, 0}, // x
    {NULL, 0, 80},
    {NULL, 14, 80},
    {NULL, 81, 80},
    {NULL, 0, 0}, // x
    
    {NULL, 0, 0}, // x
    {NULL, 0, 97},
    {NULL, 1, 97},
    {NULL, 97, 97},
    {NULL, 0, 0} // x
};

const SpriteDefinition* wall_tiles_1_front[20] = {
    &wall1a_6, &wall1a_6, &wall1a_6, &wall1a_6, &wall1a_6,
    &wall1b_2, &wall1b_1, &wall1b_1, &wall1b_1, &wall1b_2,
    NULL, &wall1b_3, &wall1b_0, &wall1b_3, NULL,
    NULL, &wall1c_1, &wall1c_0, &wall1c_1, NULL
};

struct dungeonTile frontWallDisp[20] = { //SUB five
    {NULL, 0, 49},
    {NULL, 22, 49},
    {NULL, 44, 49},
    {NULL, 64, 49},
    {NULL, 86, 49},

    {NULL, 0, 42}, //cropme
    {NULL, 14, 42},
    {NULL, 41, 42},
    {NULL, 65, 42},
    {NULL, 112-17, 42},

    {NULL, 0, 0}, //x
    {NULL, 0, 32},
    {NULL, 32, 32},
    {NULL, 79, 32},
    {NULL, 0, 0}, //x
    
    {NULL, 0, 0}, //x
    {NULL, -1, 13},
    {NULL, 15, 14},
    {NULL, 95, 13},
    {NULL, 0, 0}, //x

};

const SpriteDefinition* wall_tiles_1_side[20] = {
    &wall1a_5, &wall1a_3, NULL, &wall1a_3, &wall1a_5,
    &wall1a_4, &wall1a_2, NULL, &wall1a_2, &wall1a_4,
    NULL, &wall1a_1, NULL, &wall1a_1, NULL,
    NULL, &wall1a_0, NULL, &wall1a_0, NULL
};
struct dungeonTile sideWallDisp[20] = { //add 5
    
    {NULL, 10, 42},
    {NULL, 42, 42},
    {NULL, 0, 0}, // x
    {NULL, 64, 42},
    {NULL, 86, 42},

    {NULL, -2, 33},
    {NULL, 33, 33},
    {NULL, 0, 0}, // x
    {NULL, 65, 32},
    {NULL, 97, 33},
    
    {NULL, 0, 0}, //x
    {NULL, 15, 13},
    {NULL, 80, 14}, // x
    {NULL, 73, 13},
    {NULL, 0, 0}, //x
    
    {NULL, 0, 0}, //x
    {NULL, 0, 0},
    {NULL, 0, 0}, // x
    {NULL, 112-16, 0},
    {NULL, 0, 0} //x
};


void ResetPalettes()
{
    for(u8 i = 0; i < 4; i ++)
        PAL_setPalette(i, *curPaletteSet[i]);
}

bool animatingDmgTxt;
s8 dmgTextFrame = 0;

const SpriteDefinition* digits[] = {
    &digit_0, &digit_1, &digit_2, &digit_3, 
    &digit_4, &digit_5, &digit_6, &digit_7, 
    &digit_8, &digit_9, &digit_m, &digit_i,
    &digit_s, &digit_ep
};
*/
#ifndef null
#define null NULL
#endif
/*
s16 GetPlayerPhysicalAtkDmg(struct Player* p, struct Enemy* e)
{
    fp32 dmg;
    // By default we'll do 1d6...
    if(p->weap == null)
        dmg = roll(6) * p->strength;
    else {
        // calculate weapon damage
    }
    dmg = dmg >> 8;
    dmg -= e->defense;
    if (dmg < 1) dmg = 1;
    return (s16)dmg;
}

s16 GetPhysicalAtkDmg(struct Enemy* e, struct Player* p)
{
    fp32 dmg;
    // By default we'll do 1d6...
    dmg = roll(6) * e->strength;
    dmg = dmg >> 8;
    struct BodyPiece* bp = &p->body;
    for(u8 c = 0; c < 5; c++){
        if (bp->name != null){
            if(bp->def != 0){
                dmg -= bp->def;
            }
        }
    }
    if (dmg < 1) dmg = 1;
    return (s16)dmg;
}

void Enemy_AttackDo()
{
    //VDP_drawText("enemy attack", 0, 1);
    struct enemyCombatant* e = &combatants.enemies[nextCombatActor-4];
    struct playerCombatant* p = &combatants.players[command_arr[nextCombatActor].target];
    // Roll attack!
    last_tohit = e->stats.thaco - roll(20);

    s8 pac = GetArmorClass(&p->stats);
    
    // Roll damage!
    if (pac <= last_tohit)
    {
        last_attack_hit = true;
        last_attack_dmg = GetPhysicalAtkDmg(&e->stats, &p->stats);
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

// FlowTextRow will process inline script commands - wait, color change, etc
void FlowTextRow()
{
    if(waitingToPrint[printLoc] != NULL)
    {
        if(waitingToPrint[printLoc] == WAIT_FOR_BTN) { // Wait for input!
            NPC_waitingForButton = true;
            printLoc++;
        }
        else { // Normal character
            memcpy(&textLog[2475+printLoc], &waitingToPrint[0+printLoc], 1);
            printLoc++;
            logUpdated = false;
        }
    }
    else {
        for(u8 y = 0; y < 25; y++) waitingToPrint[y] = NULL;
        printLoc = 0;
    }
}


char* ProcessScriptCodes(const char* line)
{
    u8 d = 0; 
    for(char c = 0; c < 25; c ++){
        if ((line[c] >= PHAESTA_NAME) && (line[c] <= '\x98')) { // Player name?
            char* n = &playerNames[(line[c] & 0b00000111) - 1];
            memcpy(&procLine[d], n, strsize(n));
            d += strsize(n)-1;
        }
        else if((line[c] == WAIT_FOR_BTN)) {
            NPC_waitingForButton = true;
        }
        else 
        { 
            memcpy(&procLine[d], line + c, 1);
        }
        d++;
    }
    return &procLine;
}


// UpdateTextLog processes script replacement codes (i.e. names, vars)
void UpdateTextLog(const char* toAdd){
    // shift everything
    u8 len = strsize(toAdd);
    s8 blen = 25 - len;
    for(u16 p = 25; p <= 2475; p = p + 25){
        memcpy(&textLog[p-25], &textLog[p], 25);
    }
    if( (printLoc == 0) && (waitingToPrint[0] == NULL) ){ 
        strcpy(&textLog[2475], &blankLine); // clear bottom row
        // Process script replacement stuff 
        char * f = ProcessScriptCodes(toAdd);
        memcpy(&waitingToPrint, f, 25);
    }
    else {
        // Finish up waitingToPrint and then fresh line
        printLoc = 0;
        // TODO - finish process!
        memcpy(&textLog[2450], &waitingToPrint[0], 25);
        strcpy(&textLog[2475], &blankLine);
        // and process script replacement stuff 
        char * f = ProcessScriptCodes(toAdd);
        memcpy(&waitingToPrint, f, 25);
    }
}
*/

void LoadDungeonMap(const u8* dungeon)
{
    u16 mapctr = 0;
    u8 mwid = *(dungeon + 0);
    u16 dlen = *(dungeon + sizeof(dungeon));
    for(u16 i = 1; i < dlen; i++)
    {
        u8 nb = *(dungeon + i);
        if(nb == 0xfe){
            u8 ct = *(dungeon + i + 2);
            nb = *(dungeon + i + 1);
            for(u8 j = 0; j < ct; j++){
                map[mapctr++] = nb;
            }
            i = i + 2;
        }
        else {
            map[mapctr++] = nb;   
        }
    }
}

#define TYPE_BATBG 3
/*
void LoadBattBG(const TileSet* image, bool not_first, u8 frame)
{
    DMA_transfer(DMA_QUEUE, DMA_VRAM, (void*)(image->tiles + (20*32*frame)), (0x100 * 32) + (80*32*frame), 80 * 16, 2);
    if(not_first == false){
        vu16 *pw;
        vu32 *pl;
        u16 addr;
        u16 tdat;
        pw = (u16 *) GFX_DATA_PORT;
        pl = (u32 *) GFX_CTRL_PORT;
        u16 loop = 320;
        for(u16 i = 0; i < loop; i ++){
            addr = VDP_getPlaneAddress(BG_B, (i % 40), ((u16)i/40) + 12);
            tdat = TILE_ATTR_FULL(PAL0, 0, 0, 0, 0x0100 + i);
            *pl = GFX_WRITE_VRAM_ADDR((u32) addr);
            *pw = tdat;
        }
    }
}
*/

/*
void LoadFace(TileSet* image, u8 type)
{
    if(type == TYPE_PC) VDP_loadTileSet(image, HERO_FACE_TILEINDEX, DMA_QUEUE);
      else if(type == TYPE_NPC) {
          //VDP_fillTileData(0, NPC_FACE_TILEINDEX, 100*2, false);
          VDP_loadTileSet(image, NPC_FACE_TILEINDEX, DMA);
      }
      else if(type == TYPE_ADV) VDP_loadTileSet(image, ADV_IMG_TILEINDEX, DMA_QUEUE);
    vu16 *pw;
    vu32 *pl;
    u16 addr;
    u16 tdat;
    pw = (u16 *) GFX_DATA_PORT;
    pl = (u32 *) GFX_CTRL_PORT;
    u8 loop;
    if(type == TYPE_PC) loop = 64;
      else if(type == TYPE_NPC) loop = 100;
      else if(type == TYPE_ADV) loop = 196;
    for(u8 i = 0; i < loop; i ++){
        if(type == TYPE_PC) addr = VDP_getPlaneAddress(BG_A, (i % 8) + 23, ((u8)i/8) + 1);
          else if(type == TYPE_NPC) addr = VDP_getPlaneAddress(BG_A, (i % 10) + 4, ((u8)i/10) + 3); 
          else if(type == TYPE_ADV) addr = VDP_getPlaneAddress(BG_A, (i % 14) + 2, ((u8)i/14) + 1);
        // TODO fix palette loading...?
        if(type == TYPE_PC) tdat = TILE_ATTR_FULL(PAL2, 1, 0, 0, HERO_FACE_TILEINDEX + i);
          else if(type == TYPE_NPC) {
              tdat = TILE_ATTR_FULL(PAL0, 1, 0, 0, NPC_FACE_TILEINDEX + i); 
            }
          else if(type == TYPE_ADV) tdat = TILE_ATTR_FULL(PAL1, 0, 0, 0, ADV_IMG_TILEINDEX + i);
        *pl = GFX_WRITE_VRAM_ADDR((u32) addr);
        *pw = tdat;
    }
}
*/


void LoadPalettestoTemp()
{
    u32 a;
    u8 f;
    u32* p = &tempPalettes[0][0];
    WRITE_CTRLREG(read_cram | cram_pal0);
    for(f = 0; f < 8*4; f++)
    //0bBBB0GGG0RRR0
    {
        READ_DATAREG32(a);
        *p++ = a;
    }
}

void UnflashAllPalettes() 
{
    LoadPalettestoTemp();
    
    u8 f, s;
    u16 a = 0;
    u16 c = 0;
    
    for(s = 0; s < 4; s++){
        u16* tr = (u16*)curPaletteSet[s];
        for(f = 0; f < 16; f++)
        {
            if((tempPalettes[s][f] & 0x0f00) > (*tr & 0x0f00))
            {
                s8 t = (tempPalettes[s][f] & 0x0f00) >> 8;
                t -= 2;
                if (t < 0) t = 0;
                t = t & 0b1111110;
                tempPalettes[s][f] = (tempPalettes[s][f] & 0x00ff) | (t << 8);
            }
            if((tempPalettes[s][f] & 0x00f0) > (*tr & 0x00f0))
            {
                s8 t = (tempPalettes[s][f] & 0x00f0) >> 4;
                t -= 2;
                if (t < 0) t = 0;
                t = t & 0b1111110;
                tempPalettes[s][f] = (tempPalettes[s][f] & 0x0f0f) | (t << 4);
            }
            if((tempPalettes[s][f] & 0x000f) > (*tr & 0x000f))
            {
                s8 t = tempPalettes[s][f] & 0x000f;
                t -= 2;
                if (t < 0) t = 0;
                t = t & 0b1111110;
                tempPalettes[s][f] = (tempPalettes[s][f] & 0x0ff0) | (t);
            }
            tr++;
        }
    }
    for(f = 0; f < 4; f++){
        LoadPalette(f, &tempPalettes[f]);
    }
}

void FlashAllPalettes()
{
    // increase the current palette values by 2
    // store in temp palettes
    // flush all temp palletes
    LoadPalettestoTemp();
    u16 a;
    u8 f;
    u16* p = &tempPalettes[0][0];
    WRITE_CTRLREG(read_cram | cram_pal0);
    for(f = 0; f < 16*4; f++)
    //0bBBB0GGG0RRR0
    {
        READ_DATAREG16(a);
        u16 b = (a >> 8);
        u16 g = (a >> 4) & 0b00001110;
        u8 r = a & 0b1110;
        b += 2;
        g += 2;
        r += 2;
        if(b > 15) b = 15;
        if(g > 15) g = 15;
        if(r > 15) r = 15;
        a = (b << 8) | (g << 4) | r;
        *p++ = a;
    }
    
    for(f = 0; f < 4; f++)
        LoadPalette(f, &tempPalettes[f]);

}

#ifndef QSYS
#define QSYS

#define QUEUE_SIZE 50
void* function_q[QUEUE_SIZE];
void* q_args[QUEUE_SIZE];
u8 q_in = 0;
u8 q_ofs = 0;
s16 GLOBALWAIT = 0;
void AddQueue(void *q, void* a);
#define AddQ(s) AddQueue(s, null)
void DoQ();
#define secs(s) (u16)(s*60)

void AddQueue(void *q, void* a)
{
    function_q[q_ofs] = q;
    q_args[q_ofs] = a;
    q_ofs ++;
    if(q_ofs >= QUEUE_SIZE) q_ofs = 0;
}
void DoQ()
{

    void(*f)() = (void*)function_q[q_in];
    if(function_q[q_in] != NULL){ 
        f(q_args[q_in]);
        function_q[q_in] = null;
        q_args[q_in] = null;
        q_in ++;
        if(q_in >= QUEUE_SIZE) q_in = 0;
    }
}
void Wait(int a)
{
    GLOBALWAIT += a;
}

#endif