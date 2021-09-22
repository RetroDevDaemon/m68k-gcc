#include <bentgen.h>

/*
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
*/
const struct Player pex = { (const char*)&playerNames[0],    30,    0,    0,    1,    fp16(0.5),    fp16(0.25),    fp16(0.3),    18,\
    NULL, /*weapon*/\
    NULL, /*body*/\
    NULL,\
    NULL,\
    NULL,\
    NULL, /*..body*/\
    NULL, /*grow*/\
    NULL, NULL, NULL,/*&hero01, &hero01_2, &hero01_3,*/\
    1
};

struct Player pex2 = {
    (const char*)&playerNames[1],
    30,
    0,
    0,
    1,
    fp16(0.5),
    fp16(0.25),
    fp16(0.4),
    18,
    NULL, //weapon
    NULL, //body..
    NULL,
    NULL,
    NULL,
    NULL, //..body
    NULL, //growth
    NULL, NULL, NULL,//&hero02, &hero02_2, &hero02_3,
    1
};

struct Player pex3 = {
    (const char*)&playerNames[2],
    30,
    0,
    0,
    1,
    fp16(0.5),
    fp16(0.25),
    fp16(0.5),
    18,
    NULL, //weapon
    NULL, //body..
    NULL,
    NULL,
    NULL,
    NULL, //..body
    NULL, //growth
    NULL, NULL, NULL,//&hero03,&hero03_2,&hero03_3,
    1
};

struct Player pex4 = { // copy of 3
    (const char*)&playerNames[3],
    30,
    0,
    0,
    1,
    fp16(0.5),
    fp16(0.25),
    fp16(0.6),
    18,
    NULL, //weapon
    NULL, //body..
    NULL,
    NULL,
    NULL,
    NULL, //..body
    NULL, //growth
    NULL, NULL, NULL,//&hero03,&hero03_2,&hero03_3,
    1
};
