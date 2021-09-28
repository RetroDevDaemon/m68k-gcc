
//main.c
#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800
#define VRAM_SCROLL 0xfc00

#define BG_WIDTH 64
#define BG_HEIGHT 32

#include <bentgen.h>

#include "font.h"
#include "palette.h"

static u16 tileindex;

const char hw[] = "Hello World!";

int main()
{       
        u16 c;
	// Load palette
        LoadPalette(0, (u16*)&palette);
        
	// Load font
#define ASCIIBASETILE 32
        tileindex = VDPLoadTiles(ASCIIBASETILE, (u32*)&font_0, 96);

        //Print
        print(BG_A, 5, 5, hw);
	
        u8 sz_char, sz_int;
        u8 sz_long, sz_longlong;
        u8 sz_ptr, sz_short;
        
        const u8* sz_c_t = "Size of char: ";
        const u8* sz_s_t = "Size of short: ";
        const u8* sz_i_t = "Size of int: ";
        const u8* sz_l_t = "Size of long: ";
        const u8* sz_ll_t = "Size of longlong: ";
        const u8* sz_p_t = "Size of ptr: ";
        
        sz_char = sizeof(char);
        sz_short = sizeof(short);
        sz_int = sizeof(int);
        sz_long = sizeof(long);
        sz_longlong = sizeof(long long);
        sz_ptr = sizeof(void*);

        char o[3] = { 0,0,0 };
        
        byToHex(sz_char, &o);
        print(BG_A, 5, 6, sz_c_t);
        print(BG_A, 5, 7, &o);

        byToHex(sz_short, &o);
        print(BG_A, 5, 8, sz_s_t);
        print(BG_A, 5, 9, &o);

        byToHex(sz_int, &o);
        print(BG_A, 5, 10, sz_i_t);
        print(BG_A, 5, 11, &o);

        byToHex(sz_long, &o);
        print(BG_A, 5, 12, sz_l_t);
        print(BG_A, 5, 13, &o);

        byToHex(sz_longlong, &o);
        print(BG_A, 5, 14, sz_ll_t);
        print(BG_A, 5, 15, &o);

        byToHex(sz_ptr, &o);
        print(BG_A, 5, 16, sz_p_t);
        print(BG_A, 5, 17, &o);

        while(1){}
        return 0;
}

void GAME_DRAW()
{}