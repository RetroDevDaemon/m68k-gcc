
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
	u8 layer;
} queuedTile;
queuedTile queued_tiles[32];
queuedTile* cur_qt; // = &queued_tiles[0];
bool tileQueue = true;

bool DMA_TEST;

typedef u16 VDPTile; // use TILEATTR()
// testing with 30x20 tile view port
#define METATILESIZE 2 // 2x2 tiles 
#define METAMAP_DISPLAY_WIDTH (32 * METATILESIZE) 
#define METAMAP_DISPLAY_HEIGHT (16 * METATILESIZE) 
static VDPTile mapBuffer[METAMAP_DISPLAY_WIDTH*METAMAP_DISPLAY_HEIGHT]; // 64x32

static Sprite sprites[80];
static metaTile wmtiles[150];

void PopulateMetatileList(u16 st_t, u16 en_t, u8 sz, metaTile* mt, u8 pal); 
void DrawMetaTile(metaTile* mt, u8 layer, u8 tx_ofs, u8 ty_ofs);
void QueueMetaTile(metaTile* mt, u8 layer, u8 tx, u8 ty, u8 priority);
void LoadSong(u8* son);
void PlaySong();
void DMADisplayMap(u8 layer);

static s32 cycles;
static s32 vcycles;

static bool printUpdate;

#define BGBH_CAM_OFFSET (-16)
#define BGBV_CAM_OFFSET (16)

static u8 BYTOHEXWORK[3] = "  ";
void byToHex(u8 by, u8* ar)
{
	//u8 BYTOHEXWORK[3] = "  ";
    u8 a = by & 0xf;
    u8 b = (by & 0xf0) >> 4;
    if (a > 9) a += 7;
    if (b > 9) b += 7;
    a += 0x30;
    b += 0x30;
    ar[0] = b; ar[1] = a; ar[2] = 0x20;
}
static bool flipping;
static u8 right_tile_q;
static u8 left_tile_q;
static u8 up_tile_q;
static u8 down_tile_q;
static s8 target_row;
static u8 LAST_DIR_PRESSED;
static s8 target_col;
#define FRAMERATE 60
static s8 frameCounter;
static bool debug_text_enabled;
static s8 thirtyFrameCounter;

static u8 vch[3];
static u8 vcl[3];
static u8 _zero = 0;
static u8 ch[3];
static u8 cl[3];

void UpdateDebugText()
{
	byToHex(vcycles >> 8, &vch);
	byToHex(vcycles & 0xff, &vcl);
	byToHex(cycles >> 8, &ch);
	byToHex(cycles & 0xff, &cl);
	// Every word write to the VDP is ~2 cycles.
	print(BG_A, 5, 0, (String*)"CPU Cycles left:");
	print(BG_A, 5, 1, ch);
	print(BG_A, 7, 1, cl);
	print(BG_A, 5, 2, (String*)"VDP Cycles left:");
	print(BG_A, 5, 3, vch);
	print(BG_A, 7, 3, vcl);
}


// *** MAIN *** //
void main()
{       
	u32 c;
	u16* zp;
	u32* cr; 
	u8 z;
	// Configure bg size 
	WriteVDPRegister(WRITE|REG(0x10)|0b00000001);
	// VSRAM location
	//WriteVDPRegister(WRITE|REG(13)|0b00111111);
	//WriteVDPRegister(WRITE|REG(14)|0);
	bga_hscroll_pos = 0;
	bga_vscroll_pos = 0;
	bgb_hscroll_pos = 0;
	bgb_vscroll_pos = 0;
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
	bga_hscroll_pos = 0; //BGAH_CAM_OFFSET;
	bga_vscroll_pos = 0; //BGAV_CAM_OFFSET;
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
	flipping = false;
	debug_text_enabled = false;
	for(u16 v = 0; v < (64*32); v++)
	{
		mapBuffer[v] = 0x0;
	}
	// sprites 
	spriteRamBase = &sprites[0];
	LinkAllSpriteData();

	// Enable VBlank
	WriteVDPRegister(WRITE|REG(1)|(MODE|VBLIRQ_ON|DMA_OFF|NTSC|Video_ON));
	right_tile_q = 0;
	left_tile_q = 0;
	up_tile_q = 0;
	down_tile_q = 0;
	target_row = 0;
	target_col = 0;
	z = 0;

	printUpdate = true;
	// tile test for scrolling bigmap
	WaitVBlank();
	
	QueueMetaTile(&wmtiles[3], BG_B,  5, 5, PRIORITY_LOW);
	QueueMetaTile(&wmtiles[4], BG_B,  38, 24, PRIORITY_LOW);

	while(1)
	{
		// Wait for VBL, count the cycles we wait until VBL
		VBL_DONE = false;
		cycles = 0;
		while(!VBL_DONE)
			cycles++;
		// DRAW interrupt occurs here
		frameCounter++;
		thirtyFrameCounter++;
		if(frameCounter > 60) 
		{
			frameCounter = 0;
		}
		if(thirtyFrameCounter > 30)
		{
			thirtyFrameCounter = 0;
			if(debug_text_enabled) UpdateDebugText();
		}

		PlaySong();
		
		last_joyState1 = joyState1;
    	GETJOYSTATE1(joyState1);
		
		// BACKGROUND SCROLLING TEST
#define SCROLLSPEED 1
		if(joyState1 & BTN_RIGHT_PRESSED) 
		{
			bgb_hscroll_pos -= SCROLLSPEED;
			if((bgb_hscroll_pos) % 16 ==0) target_col += 2;
			if(target_col < 0) target_col += 64;
			for(z = 0; z < SCROLLSPEED; z++){	
				if(right_tile_q > 64) right_tile_q -= 64; 
				QueueMetaTile(&wmtiles[3], BG_B, target_col-2, right_tile_q*2, 0);
				right_tile_q++;
			}
			LAST_DIR_PRESSED = BTN_RIGHT_PRESSED;
		}
		if(joyState1 & BTN_LEFT_PRESSED) {
			bgb_hscroll_pos += SCROLLSPEED;
			if((bgb_hscroll_pos) % 16 ==0) target_col -= 2;
			if(target_col > 64) target_col -= 64;
			for(z = 0; z < SCROLLSPEED; z++){
				if(left_tile_q > 64) left_tile_q -= 64;
				QueueMetaTile(&wmtiles[3], BG_B, target_col-4, left_tile_q*2, 0);
				left_tile_q++;
			}
			LAST_DIR_PRESSED = BTN_RIGHT_PRESSED;
		}
		if(joyState1 & BTN_UP_PRESSED) {
			//if(LAST_DIR_PRESSED != BTN_UP_PRESSED) target_row = (bgb_vscroll_pos/16) + 28;
			bgb_vscroll_pos -= SCROLLSPEED;
			if((bgb_vscroll_pos)%16==0) target_row-=2;
			if(target_row < 0) target_row += 32;
			for(z = 0; z < SCROLLSPEED; z++){
				if(up_tile_q >= 32) up_tile_q -= 32;
				QueueMetaTile(&wmtiles[3], BG_B, up_tile_q*2, target_row-4, 0);
				up_tile_q++;
				QueueMetaTile(&wmtiles[3], BG_B, up_tile_q*2, target_row-4, 0);
				up_tile_q++;
			}
			LAST_DIR_PRESSED = BTN_UP_PRESSED;
		}
		if(joyState1 & BTN_DOWN_PRESSED)
		{
			//if(LAST_DIR_PRESSED != BTN_DOWN_PRESSED) target_row = (bgb_vscroll_pos/16) - 2;
			bgb_vscroll_pos += SCROLLSPEED;
			if((bgb_vscroll_pos) % 16 == 0) target_row+=2;
			if(target_row >= 32) target_row -= 32;
			for(z = 0; z < SCROLLSPEED; z++){	
				if(down_tile_q >= 32) down_tile_q -= 32;
				QueueMetaTile(&wmtiles[3], BG_B, down_tile_q*2, target_row-2, 0);
				down_tile_q++;
				QueueMetaTile(&wmtiles[3], BG_B, down_tile_q*2, target_row-2, 0);
				down_tile_q++;
			}
			LAST_DIR_PRESSED = BTN_DOWN_PRESSED;
		}
		// end bg test
		/* Debug Menu */
		
		if(Joy1Down(BTN_START_PRESSED))
		{
			if(debug_text_enabled) 
			{
				debug_text_enabled = false;
				print(BG_A, 5, 0, "                ");
				print(BG_A, 5, 1, "         ");
				print(BG_A, 5, 2, "                ");
				print(BG_A, 5, 3, "         ");
				
			}
			else 
			{
				debug_text_enabled = true;
				UpdateDebugText();
			}
		}
		
	} // end main game loop 
} // end main()

// todo: convert all prints to DMA, maybe 
void GAME_DRAW()
{
	DMADisplayMap(BG_B);
	UpdateBGScroll();	// update background position
	
	// Idle and count remaining vblank cycles
	vcycles = 0;
	while(*((u32*)0xc00004) & 0b1000)
		vcycles++;
	
	VBL_DONE = true;
}

void QueueMetaTile(metaTile* mt, u8 layer, u8 tx, u8 ty, u8 priority)
{
	while(tx >= 64) tx -= 64;
	while(ty >= 32) ty -= 32;

	u16 flags = TILEATTR(0, NO_HFLIP, NO_VFLIP, mt->pal, priority);
	u16 ofs = (u16)(ty * 64) + tx;
		
	switch(mt->size)
	{
		case(2):
		
			mapBuffer[ofs] = (u16)(mt->ia | flags);
			mapBuffer[ofs+1] = mt->ia+1 | flags;
			ofs += 64;
			mapBuffer[ofs] = mt->ib | flags;
			mapBuffer[ofs+1] = mt->ib+1 | flags;
			break;
		case(3): // FIXME
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
	
	for(i = 0; i < 150; i++)
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

void DrawMetaTile(metaTile* mt, u8 layer, u8 tx_ofs, u8 ty_ofs)
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

#define REPT10(n) n;n;n;n;n;n;n;n;n;n;
#define REPT4(n) n;n;n;n;
#define REPT8(n) REPT4(n);REPT4(n);
#define REPT3(n) n;n;n;
#define REPT9(n) REPT3(REPT3(n))
#define REPT20(n) REPT10(n);REPT10(n);

#define DMA_SIZE 2048

void DMADisplayMap(u8 layer)
{
	u32 TARGET;
	u32 src;
	u32 dest;
	if (layer == BG_A)
		TARGET = (VRAM_BG_A);// + 256; // calculate offset here 
	else 
		TARGET = (VRAM_BG_B);// + 256; 

	src = (u32)&mapBuffer;
	WriteVDPRegister(WRITE|REG(1)|(MODE|VBLIRQ_OFF|DMA_ON|NTSC|Video_OFF));
	SetDMALength(DMA_SIZE); // Amount (in words) to transfer over DMA (1kbmax?)
	SetDMASourceAddress(src); // source (in words)
	dest = 0x40000000|(((TARGET & 0x3fff)<<16)|((TARGET & 0xc000)>>14));
	dest |= (u32)(0x80); // BIT 7 MUST BE SET (d5) FOR DMA!
	WRITE_CTRLREG(dest); // This triggers DMA
	while(*((vu8*)0xc00004) & 0b10000000){};
	TARGET += DMA_SIZE*2; // bytes
	src += DMA_SIZE*2; // bytes
	WriteVDPRegister(WRITE|REG(1)|(MODE|VBLIRQ_ON|DMA_OFF|NTSC|Video_ON));
	
	DMA_TEST = true;
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