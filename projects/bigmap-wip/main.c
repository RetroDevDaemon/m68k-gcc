
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
	u16 tx;
	u16 ty;
	u8 layer;
} queuedTile;
queuedTile queued_tiles[32];
queuedTile* cur_qt; // = &queued_tiles[0];
bool tileQueue = true;

bool DMA_TEST;

typedef u16 VDPTile; // use TILEATTR()
// testing with 30x20 tile view port
#define METATILESIZE 2 // 2x2 tiles 
#define METAMAP_DISPLAY_WIDTH (15 * METATILESIZE) 
#define METAMAP_DISPLAY_HEIGHT (14 * METATILESIZE) 
#define METAMAP_X0 8
#define METAMAP_Y0 0
VDPTile mapBuffer[METAMAP_DISPLAY_HEIGHT * METAMAP_DISPLAY_WIDTH];

Sprite sprites[80];
metaTile wmtiles[150];

void PopulateMetatileList(u16 st_t, u16 en_t, u8 sz, metaTile* mt, u8 pal); 
void DrawMetaTile(metaTile* mt, u8 layer, u16 tx_ofs, u16 ty_ofs);
void QueueMetaTile(metaTile* mt, u8 layer, u16 tx, u16 ty, u8 priority);
//void ShiftDisplayMap(s8 x_shift, s8 y_shift);
void LoadSong(u8* son);
void PlaySong();
//u8 SetVRAMCall[0x4a];

u32 cycles;

// *** MAIN *** //

void main()
{       
	u32 c;
	u16* zp;
	u32* cr; 
	// Configure bg size 
	WriteVDPRegister(WRITE|REG(0x10)|0b00000001);
	//WriteVDPRegister(WRITE|REG(0x11)|0b10001111);
	//WriteVDPRegister(WRITE|REG(0x12)|0b10001111);
	
	// Load palette
	LoadPalette(0, (u16*)&palette);
	LoadPalette(1, (u16*)&wm_pal);

#define WORLDMAPTILES 128
	// Load font
	tileindex = VDPLoadTiles(ASCIIBASETILE, (u32*)&font_0, 96);
	// and world map tiles
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
	bga_hscroll_pos = 0;
	bga_vscroll_pos = 0;
	//bga_hscroll_pos = (u8)(((BG_WIDTH  * 8)-320)/2); // 96
	//bga_vscroll_pos = (u8)(((BG_HEIGHT * 8)-212)/2); // 16
	bgb_hscroll_pos = 0;
	bgb_vscroll_pos = 0;
	// then, configure our 16x16 metatiles - map image is 10x15 by 2x2
	PopulateMetatileList(128, 128+600, 2, (metaTile*)&wmtiles, 1); // 600 tiles 2x2
	// then draw map based on metatile listing
	// initialize tile queue
	cur_qt = &queued_tiles[0];
	DMA_TEST = false;
	for(u16 v = 0; v < (METAMAP_DISPLAY_HEIGHT * METAMAP_DISPLAY_WIDTH); v++)
	{
		mapBuffer[v] = 0;
	}
	// sprites 
	spriteRamBase = &sprites[0];
	LinkAllSpriteData();

	// Enable VBlank
	WriteVDPRegister(WRITE|REG(1)|(MODE|VBLIRQ_ON|DMA_OFF|NTSC|Video_ON));
	
	// tile test for scrolling bigmap
	WaitVBlank();
	QueueMetaTile(&wmtiles[3], BG_B,  28, 18, PRIORITY_LOW);
	QueueMetaTile(&wmtiles[3], BG_B,  8, 18, PRIORITY_LOW);
	QueueMetaTile(&wmtiles[3], BG_B,  28, 0, PRIORITY_LOW);
	QueueMetaTile(&wmtiles[3], BG_B,  16, 10, PRIORITY_LOW);

	QueueMetaTile(&wmtiles[15], BG_B,  8, 0, PRIORITY_LOW);
	QueueMetaTile(&wmtiles[3], BG_B,  10, 2, PRIORITY_LOW);
	QueueMetaTile(&wmtiles[45], BG_B,  12, 2, PRIORITY_LOW);
	
	while(1)
	{
		// Wait for VBL to finish
		VBL_DONE = false;
		while(!VBL_DONE){}

		PlaySong();
		
		GETJOYSTATE1(joyState1);

		// BACKGROUND SCROLLING TEST
		if(joyState1 & BTN_RIGHT_PRESSED) // Is (C) pressed?
			bgb_hscroll_pos-=4;
		if(joyState1 & BTN_LEFT_PRESSED) // Is (C) pressed?
			bgb_hscroll_pos+=4;
		if(joyState1 & BTN_UP_PRESSED) // Is (C) pressed?
			bgb_vscroll_pos-=4;
		if(joyState1 & BTN_DOWN_PRESSED) // Is (C) pressed?
			bgb_vscroll_pos+=4;

		if(bgb_hscroll_pos & 0b1000000000000000)
		{
			bgb_hscroll_pos = 15;
			ShiftDisplayMapRight();
			DMA_TEST = false;
		}
		else if(bgb_hscroll_pos >= (8*METATILESIZE))
		{
			bgb_hscroll_pos = 0;
			ShiftDisplayMapLeft();
			DMA_TEST = false;
		}
		if(bgb_vscroll_pos & 0b1000000000000000)
		{
			bgb_vscroll_pos = 15;
			ShiftDisplayMapDown();
			DMA_TEST = false;
		}
		else if(bgb_vscroll_pos >= (8*METATILESIZE))
		{
			bgb_vscroll_pos = 0;
			ShiftDisplayMapUp();
			DMA_TEST = false;
		}
		//

		//Print test string
		if(DMA_TEST){
			SetVRAMWriteAddress(VRAM_BG_A + (BG_WIDTH*5*2) + (5*2)); // Screen address + 5 Y, 5 X (BG_A)
			u8* chp = (u8*)&hw[0];       // String address
			for(c = 0; c < sizeof(hw); c++) WRITE_DATAREG16((u16)*chp++); // Loop
		}
	}
}

void QueueMetaTile(metaTile* mt, u8 layer, u16 tx, u16 ty, u8 priority)
{
	queuedTile* nx = &queued_tiles[0];
	while(nx->tile != nullptr)
		nx++;
	if (nx > &queued_tiles[32])
		nx = &queued_tiles[0];
	
	nx->tile = mt;
	nx->tx = tx;
	nx->ty = ty;
	nx->layer = layer;

	u16 flags = TILEATTR(0, NO_HFLIP, NO_VFLIP, mt->pal, priority);

	switch(mt->size)
	{
		case(2):
			mapBuffer[(ty * METAMAP_DISPLAY_WIDTH) + tx] = (u16)(mt->ia | flags);
			mapBuffer[(ty * METAMAP_DISPLAY_WIDTH) + tx + 1] = mt->ia+1 | flags;
			mapBuffer[((ty+1) * METAMAP_DISPLAY_WIDTH) + tx] = mt->ib | flags;
			mapBuffer[((ty+1) * METAMAP_DISPLAY_WIDTH) + tx + 1] = mt->ib+1 | flags;
			break;
		case(3):
			mapBuffer[(ty * METAMAP_DISPLAY_WIDTH) + tx] = mt->ia | flags;
			mapBuffer[(ty * METAMAP_DISPLAY_WIDTH) + tx + 1] = mt->ia+1 | flags;
			mapBuffer[(ty * METAMAP_DISPLAY_WIDTH) + tx + 2] = mt->ia+2 | flags;
			mapBuffer[((ty+1) * METAMAP_DISPLAY_WIDTH) + tx] = mt->ib | flags;
			mapBuffer[((ty+1) * METAMAP_DISPLAY_WIDTH) + tx + 1] = mt->ib+1 | flags;
			mapBuffer[((ty+1) * METAMAP_DISPLAY_WIDTH) + tx + 2] = mt->ib+2 | flags;
			mapBuffer[((ty+2) * METAMAP_DISPLAY_WIDTH) + tx] = mt->ic | flags;
			mapBuffer[((ty+2) * METAMAP_DISPLAY_WIDTH) + tx + 1] = mt->ic+1 | flags;
			mapBuffer[((ty+2) * METAMAP_DISPLAY_WIDTH) + tx + 2] = mt->ic+2 | flags;
			break;
	}
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
	//u8 tilemap_w = 10;
	for(i = 0; i < (METAMAP_DISPLAY_WIDTH*METAMAP_DISPLAY_HEIGHT); i++)
	{
		tile[i].pal = pal;
		tile[i].size = sz;
		tile[i].ia = d; // 138;
		tile[i].ib = d + 20; //158;
		tile[i].ic = d + 40;
		d += sz;
		r += 1;
		if(((r)%10) == 0){
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
	u16 flags = TILEATTR(0, NO_HFLIP, NO_VFLIP, mt->pal, PRIORITY_LOW);
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
		case(2): 	
			SetVRAMWriteAddress(addr);
			WRITE_DATAREG16(mt->ia | flags);
			WRITE_DATAREG16(mt->ia+1 | flags);
			addr += (BG_WIDTH*2); // 128; // 128 - 4
			SetVRAMWriteAddress(addr);
			WRITE_DATAREG16(mt->ib | flags);
			WRITE_DATAREG16(mt->ib+1 | flags);
			break;
		
		case(3):
			SetVRAMWriteAddress(addr);
			WRITE_DATAREG16(mt->ia | flags);
			WRITE_DATAREG16(mt->ia+1 | flags);
			WRITE_DATAREG16(mt->ia+2 | flags);
			addr += (BG_WIDTH*2);
			SetVRAMWriteAddress(addr);
			WRITE_DATAREG16(mt->ib | flags);
			WRITE_DATAREG16(mt->ib+1 | flags);
			WRITE_DATAREG16(mt->ib+2 | flags);
			addr += (BG_WIDTH*2);
			SetVRAMWriteAddress(addr);
			WRITE_DATAREG16(mt->ic | flags);
			WRITE_DATAREG16(mt->ic+1 | flags);
			WRITE_DATAREG16(mt->ic+2 | flags);
			break;
	}
}

void __attribute__((optimize("Os"))) ShiftDisplayMapRight()
{
	for(u16 i = 0; i < (METAMAP_DISPLAY_HEIGHT*2); i++)
	{
		for(u16 j = 0; j < (METAMAP_DISPLAY_WIDTH)-2; j+=2)
		{
			mapBuffer[(i*METAMAP_DISPLAY_WIDTH)+j] = \
				mapBuffer[(i*METAMAP_DISPLAY_WIDTH)+j+2]; 
			mapBuffer[(i*METAMAP_DISPLAY_WIDTH)+j+1] = \
				mapBuffer[(i*METAMAP_DISPLAY_WIDTH)+j+3]; 
		}
	}
	// Queue tile column on right
}

void __attribute__((optimize("Os"))) ShiftDisplayMapLeft()
{
	for(u16 i = 0; i < (METAMAP_DISPLAY_HEIGHT*2); i++)
	{
		for(s16 j = (METAMAP_DISPLAY_WIDTH)-4; j >= 0; j-=2)
		{
			mapBuffer[(i*METAMAP_DISPLAY_WIDTH)+j+2] = \
				mapBuffer[(i*METAMAP_DISPLAY_WIDTH)+j+0];	
			mapBuffer[(i*METAMAP_DISPLAY_WIDTH)+j+3] = \
				mapBuffer[(i*METAMAP_DISPLAY_WIDTH)+j+1];
		}
	}
	// Queue tile column on left 
}

void __attribute__((optimize("Os"))) ShiftDisplayMapUp()
{
	for(u16 i = 0; i < (METAMAP_DISPLAY_HEIGHT)-2; i+=2)
	{
		for(s16 j = 0; j < (METAMAP_DISPLAY_WIDTH); j++)
		{
			mapBuffer[(i*METAMAP_DISPLAY_WIDTH)+0+j] = \
				mapBuffer[(i*METAMAP_DISPLAY_WIDTH)+60+j];
			mapBuffer[((i+1)*METAMAP_DISPLAY_WIDTH)+j] = \
				mapBuffer[((i+1)*METAMAP_DISPLAY_WIDTH)+60+j];
		}
	}
	// queue tile row on bottom 
}

void __attribute__((optimize("Os"))) ShiftDisplayMapDown()
{
	for(s16 i = METAMAP_DISPLAY_HEIGHT-2; i >= 0; i-=2)
	{
		for(s16 j = 0; j < METAMAP_DISPLAY_WIDTH; j++)
		{
			mapBuffer[((i)*METAMAP_DISPLAY_WIDTH)+60+j] = \
				mapBuffer[(i*METAMAP_DISPLAY_WIDTH)+0+j];
			mapBuffer[((i)*METAMAP_DISPLAY_WIDTH)+90+j] = \
				mapBuffer[(i*METAMAP_DISPLAY_WIDTH)+30+j];
		}
	}
	// queue tile row on top 
}

void DMADisplayMap(u8 layer)
{
	u32 TARGET;
	if (layer == BG_A)
		TARGET = (VRAM_BG_A + (METAMAP_Y0*BG_WIDTH) + METAMAP_X0);
	else 
		TARGET = (VRAM_BG_B + (METAMAP_Y0*BG_WIDTH) + METAMAP_X0);
	
	u32 staddr = 0x40000000+((TARGET & 0x3fff)<<16)+((TARGET & 0xc000)>>14);
	staddr |= (u32)(0x80); // BIT 7 MUST BE SET (d5) FOR DMA!
	
	for(u8 i = 0; i < METAMAP_DISPLAY_HEIGHT*2; i++)
	{
		// DMA on
		WriteVDPRegister(WRITE|REG(1)|(MODE|VBLIRQ_ON|DMA_ON|NTSC|Video_ON));
		WriteVDPRegister(WRITE|REG(0xf)|2); // Auto-increment must be 2
		// TILE NUMBER TO COPY
		SetDMALength(METAMAP_DISPLAY_WIDTH); // Amount (in words) to transfer over DMA
		// Write source address (in WORD): 
		u32 ab = (u32)&mapBuffer + (METAMAP_DISPLAY_WIDTH*i);
		SetDMASourceAddress(ab);
		WRITE_CTRLREG(staddr); // This triggers DMA
		// Done! DMA off 
		WriteVDPRegister(WRITE|REG(1)|(MODE|VBLIRQ_ON|DMA_OFF|NTSC|Video_ON));
		staddr += (u32)(128<<16); // increment by 128 bytes
	}
	DMA_TEST = true;
}



void GAME_DRAW()
{
	// draw 8 queued tiles per frame for now
	for(u8 g = 0; g < 8; g++){
		if(cur_qt->tile != nullptr)
		{
			DrawMetaTile(cur_qt->tile, cur_qt->layer, cur_qt->tx, cur_qt->ty);
			cur_qt->tile = nullptr;
		}
		cur_qt++;
		if(cur_qt > &queued_tiles[32])
			cur_qt = &queued_tiles[0];
	}

	UpdateBGScroll();	// update background position

	// all DMAs at the end 
	if(!DMA_TEST)
		DMADisplayMap(BG_B);
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