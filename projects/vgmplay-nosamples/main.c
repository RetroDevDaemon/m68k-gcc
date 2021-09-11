
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

#include "bmaster.h"

static u16 tileindex;
static u16 joyState1;
static u16 last_joyState1;

const char hw[] = "?Deflemask?";

static bool VBL_DONE = false;

u8* songStart = (u8*)0x00008000;

int main()
{       
	u32 c;
	u16* zp;
	// Load palette
	LoadPalette(0, (u16*)&palette);
        
	// Load font
#define ASCIIBASETILE 32
	tileindex = VDPLoadTiles(ASCIIBASETILE, (u32*)&font_0, 96);

	LoadSong(song);

	// Enable VBlank on VDP 
	WriteVDPRegister(WRITE|REG(1)|0x64);

	while(1)
	{
		VBL_DONE = false;
		while(!VBL_DONE){}
		//Print
		SetVRAMWriteAddress(VRAM_BG_A + (64*5*2) + (5*2)); // Screen address + 5 Y, 5 X (BG_A)
		u8* chp = (u8*)&hw[0];       // String address
		for(c = 0; c < sizeof(hw); c++) WRITE_DATAREG16((u16)*chp++); // Loop
	}
	return 0;
}

void GAME_DRAW()
{
	GETJOYSTATE1(joyState1);
	if(joyState1 & BTN_C_PRESSED) // Is (C) pressed?
		bga_hscroll_pos++;

	PlaySong();

	VBL_DONE = true;
	UpdateBGScroll();	// update background position
}
