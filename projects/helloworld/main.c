
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
	SetVRAMWriteAddress(VRAM_BG_A + (64*5*2) + (5*2)); // Screen address + 5 Y, 5 X (BG_A)
        u8* chr = (u8*)&hw[0];       // String address
        for(c = 0; c < sizeof(hw); c++) WRITE_DATAREG16((u16)*chr++); // Loop

        while(1)
        {}
        return 0;
}

void GAME_DRAW()
{}