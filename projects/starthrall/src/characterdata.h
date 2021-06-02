#include <bentgen.h>


struct Player pex = {
    &playerNames[0],
    30,
    0,
    0,
    1,
    fp16(0.5),
    fp16(0.25),
    fp16(0.3),
    18,
    NULL, //weapon
    NULL, //body..
    NULL,
    NULL,
    NULL,
    NULL, //..body
    NULL, //growth
    NULL, NULL, NULL,//&hero01, &hero01_2, &hero01_3,
    1
};

struct Player pex2 = {
    &playerNames[1],
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
    &playerNames[2],
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
    &playerNames[3],
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
