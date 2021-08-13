
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
#include "vgmplayer.h"
#include "tacot.h"

static u16 tileindex;
static u16 joyState1;
static u16 last_joyState1;

const char hw[] = "clessy?";

static bool VBL_DONE = false;

u8* songStart = (u8*)0x00008000;

void main()
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

void PlaySong()
{
	asm("move.w #0x100,(z80_bus_request).l");
	asm("z80busreqwait:");
	asm("btst #0,(z80_bus_request).l");
	asm("bne.s z80busreqwait");
	asm("move.b #1,(0xa00080).l");
	asm("move.w #0,(z80_bus_request).l");
}

void LoadSong(u8* son)
{
	u32 stadr = &son[0];
	u8 bank = (u8)((stadr >> 15));
	//stadr += 0x80;
#define z80_base_ram 0xa00000 
#define z80_bus_request 0xa11100
#define z80_reset 0xa11200     
	asm("\n\t\
	nop \n\t\
	move.w	#0x100,(z80_bus_request).l\n\t\
	move.w	#0x100,(z80_reset).l\n\t");
// COPY z80prg[] INTO Z80 MEMORY (A00000+)
	asm(\
	"movea.l %0,%%a0\n\t"\
	"movea.l %1,%%a1\n\t"\
	::"g"(&vgmplayer), "g"(z80_base_ram):"a0","a1","d1");
	asm(\
	"move.l %0,%%d1"\
	::"g"(sizeof(vgmplayer)):"d1");
	asm(".Z80COPYLOOP:\n\t"\
	"move.b (%%a0)+,%%d0\n\t"\
	"move.b %%d0,(%%a1)+\n\t"\
	"subq 	#1,%%d1\n\t"\
	"bne 	.Z80COPYLOOP\n\t"\
	:::"d0","d1","a0","a1");
// set up vgm address on the z80
	asm("move.b %0,(0xa00086).l"::"g"(bank)); // Rom bank # (A15-A23)
	// 0x82-0x85 = start address 32 bit 
	asm("move.b %0,(0xa00082).l"::"g"(stadr & 0xff));
	asm("move.b %0,(0xa00083).l"::"g"((stadr >> 8)));
	asm("move.b %0,(0xa00084).l"::"g"((stadr >>16)));
	asm("move.b %0,(0xa00085).l"::"g"((stadr >>24)));
	
// reset, start z80
	asm("\t\
	move.w #0,(z80_reset).l \n\t\
	nop \n\t\
	nop \n\t\
	nop \n\t\
	nop \n\t\
	move.w #0x100,(z80_reset).l \n\t\
	move.w #0,(z80_bus_request).l");
}