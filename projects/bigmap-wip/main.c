
//main.c
#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800
#define VRAM_SCROLL 0xfc00

#define BG_WIDTH 64
#define BG_HEIGHT 32

#define ASCIIBASETILE 32

#include <bentgen.h>
// system
#include "font.h"
#include "palette.h"
#include "vgmplayer.h"
u8* songStart = (u8*)0x00008000;
static bool VBL_DONE = false;
// sound
#include "bmaster.h"
// gfx
#include "worldmap_tiles.h"
#include "wm_pal.h"

static u16 tileindex;
static u16 joyState1;
static u16 last_joyState1;
typedef struct meta_tile { 
	u8 size;
	u8 pal;
	u16 ia; // top left tile index 
	u16 ib; // optional - 2x2
	u16 ic; // optional - 3x3
	// 0, 1, [2]
	// 20, 21, [22]
	//[40, 41, 42]
} metaTile;
const char hw[] = "\x80Hello BigMap";

#define NO_HFLIP 0
#define YES_HFLIP 1
#define NO_VFLIP 0
#define YES_VFLIP 1
#define PAL0 0
#define PAL1 1
#define PAL2 2
#define PAL3 3
#define PRIORITY_LOW 0
#define PRIORITY_HI 1

u8 tileTestNo;
#define nullptr 0
typedef struct queued_tile { 
	metaTile* tile;
	u8 tx;
	u8 ty;
} queuedTile;
queuedTile queued_tiles[32];
queuedTile* cur_qt; // = &queued_tiles[0];

Sprite sprites[80];
metaTile wmtiles[150];

void PopulateMetatileList(u16 st_t, u16 en_t, u8 sz, metaTile* mt, u8 pal); 
void DrawMetaTile(metaTile* mt, u8 layer, u16 tx_ofs, u16 ty_ofs);
bool tileQueue = true;
void QueueMetaTile(metaTile* mt, u8 tx, u8 ty);

void main()
{       
	u32 c;
	u16* zp;
	u32* cr; 
	// Load palette
	LoadPalette(0, (u16*)&palette);
	LoadPalette(1, (u16*)&wm_pal);

#define WORLDMAPTILES 128
	// Load font
	tileindex = VDPLoadTiles(ASCIIBASETILE, (u32*)&font_0, 96);
	tileindex = VDPLoadTiles(WORLDMAPTILES, (u32*)&worldmap_tiles_0, 600);
	// and song!
	LoadSong(song);

	// bg position - center A because we will scroll it
	// We should load the next row or column of tiles based on how far into
	//  the 16 px we have scrolled. 2 tiles per pixel up or down, 1 tile per
	//  pixel left and right. 
	// when you hit the edge of a tile, copy from the vram scratch into the
	// bga area - entire row or column of tiles at once. (BLAST it!)
	// for up and down, you're dealing with the edges of the bg boundary
	bga_hscroll_pos = 16;
	bga_vscroll_pos = 16;
	//bga_hscroll_pos = (u8)(((BG_WIDTH  * 8)-320)/2); // 96
	//bga_vscroll_pos = (u8)(((BG_HEIGHT * 8)-212)/2); // 16
	bgb_hscroll_pos = 0;
	bgb_vscroll_pos = 0;
	// then, configure our 16x16 metatiles - map image is 10x15 by 2x2
	PopulateMetatileList(128, 128+600, 2, (metaTile*)&wmtiles, 1); // 600 tiles 2x2
	// then draw map based on metatile listing
	// initialize tile queue
	cur_qt = &queued_tiles[0];
	// tile test for scrolling bigmap
	QueueMetaTile(&wmtiles[3], 2, 2);
	QueueMetaTile(&wmtiles[4], 4, 2);
	QueueMetaTile(&wmtiles[5], 6, 2);
	QueueMetaTile(&wmtiles[6], 8, 2);
	
	// sprites 
	spriteRamBase = &sprites[0];
	LinkAllSpriteData();
	// Enable VBlank on VDP 
	WriteVDPRegister(WRITE|REG(1)|0x64);

	while(1)
	{
		// Wait for VBL to finish
		VBL_DONE = false;
		while(!VBL_DONE){}

		//Print test string
		SetVRAMWriteAddress(VRAM_BG_B + (64*5*2) + (5*2)); // Screen address + 5 Y, 5 X (BG_A)
		u8* chp = (u8*)&hw[0];       // String address
		for(c = 0; c < sizeof(hw); c++) WRITE_DATAREG16((u16)*chp++); // Loop
	}
}

void QueueMetaTile(metaTile* mt, u8 tx, u8 ty)
{
	queuedTile* nx = &queued_tiles[0];
	while(nx->tile != nullptr)
		nx++;
	
	nx->tile = mt;
	nx->tx = tx;
	nx->ty = ty;
}

// TODO bigger than 3x3
// TODO sizes other than 10x15
void PopulateMetatileList(u16 st_t, u16 en_t, u8 sz, metaTile* mt, u8 pal)
{
	// start tile, end tile, mt size in tiles (2 or 3)
	u16 i;
	metaTile* tile = mt;
	u16 d = st_t;
	u8 r = 0;
	u8 tilemap_w = 10;
	for(i = 0; i < 150; i++)
	{
		tile[i].pal = pal;
		tile[i].size = sz;
		tile[i].ia = d; // 138;
		tile[i].ib = d + 20; //158;
		tile[i].ic = d + 40;
		d += sz;
		r += 1;
		if((r%tilemap_w) == 0){
			if(sz == 2) 
				d += 20;
			else if(sz == 3)
				d += 40;
			r = 0;
		}
	}

}

void DrawMetaTile(metaTile* mt, u8 layer, u16 tx_ofs, u16 ty_ofs)
{
	// get the vram address 
	// write mt.size rows using macro 
	u32 addr = 0x00000000;
	u8 p = 0;
	switch(layer)
	{
		case(BG_A): 
			addr = VRAM_BG_A + (2 * ((BG_WIDTH * ty_ofs) + tx_ofs));
			break;
		case(BG_B):
			addr = VRAM_BG_B + (2 * ((BG_WIDTH * ty_ofs) + tx_ofs));
			break;
	}
	switch(mt->size)
	{
		case(2): 	//#TODO bg layer size 
			p = mt->pal;
			SetVRAMWriteAddress(addr);
			WRITE_DATAREG16(TILEATTR(mt->ia, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			WRITE_DATAREG16(TILEATTR(mt->ia+1, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			addr += (BG_WIDTH*2); // 128; // 128 - 4
			SetVRAMWriteAddress(addr);
			WRITE_DATAREG16(TILEATTR(mt->ib, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			WRITE_DATAREG16(TILEATTR(mt->ib+1, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			break;
		
		case(3):
			SetVRAMWriteAddress(addr);
			WRITE_DATAREG16(TILEATTR(mt->ia, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			WRITE_DATAREG16(TILEATTR(mt->ia+1, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			WRITE_DATAREG16(TILEATTR(mt->ia+2, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			addr += (BG_WIDTH*2);
			SetVRAMWriteAddress(addr);
			WRITE_DATAREG16(TILEATTR(mt->ib, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			WRITE_DATAREG16(TILEATTR(mt->ib+1, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			WRITE_DATAREG16(TILEATTR(mt->ib+2, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			addr += (BG_WIDTH*2);
			SetVRAMWriteAddress(addr);
			WRITE_DATAREG16(TILEATTR(mt->ic, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			WRITE_DATAREG16(TILEATTR(mt->ic+1, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			WRITE_DATAREG16(TILEATTR(mt->ic+2, NO_HFLIP, NO_VFLIP, p, PRIORITY_LOW));
			break;
	}
}

void GAME_DRAW()
{
	GETJOYSTATE1(joyState1);
	if(joyState1 & BTN_C_PRESSED) // Is (C) pressed?
		bgb_hscroll_pos++;

	// draw 2 queued tiles per frame for now
	for(u8 g = 0; g < 2; g++){
		if(cur_qt->tile != nullptr)
		{
			DrawMetaTile(cur_qt->tile, BG_A, cur_qt->tx, cur_qt->ty);
			cur_qt->tile = nullptr;
			cur_qt++;
			if(cur_qt > &queued_tiles[32])
				cur_qt = &queued_tiles[0];
		}
	}

	PlaySong();

	UpdateBGScroll();	// update background position
	VBL_DONE = true;
}

void PlaySong()
{
	asm("move.w #0x100,(z80_bus_request).l");
	asm("z80busreqwait:");
	asm("btst #0,(z80_bus_request).l");
	asm("bne.s z80busreqwait");
	asm("move.b #1,(0xa00100).l");
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
	asm("move.b %0,(0xa00106).l"::"g"(bank)); // Rom bank # (A15-A23)
	// 0x82-0x85 = start address 32 bit 
	asm("move.b %0,(0xa00102).l"::"g"(stadr & 0xff));
	asm("move.b %0,(0xa00103).l"::"g"((stadr >> 8)));
	asm("move.b %0,(0xa00104).l"::"g"((stadr >>16)));
	asm("move.b %0,(0xa00105).l"::"g"((stadr >>24)));
	
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