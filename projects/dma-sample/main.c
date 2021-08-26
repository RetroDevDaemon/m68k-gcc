
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
u8* songStart = (u8*)0x00008000;
static bool VBL_DONE = false;

static u16 tileindex;
static u16 joyState1;
static u16 last_joyState1;

const char hw[] = " Free Cycles!";

#define EXAMPLE_WIDTH 64
#define EXAMPLE_HEIGHT 32
u16 testBuffer[EXAMPLE_WIDTH*EXAMPLE_HEIGHT];
bool DMA_TEST;
u32 cycles;

u8* byToHex(u8 by);

// *** MAIN *** //
void main()
{       
	u32 c;
	u16* zp;
	u32* cr; 
	// Configure bg size 
	WriteVDPRegister(WRITE|REG(0x10)|0b00000001);
	// Load palette
	LoadPalette(0, (u16*)&palette);
	// Load font
	tileindex = VDPLoadTiles(ASCIIBASETILE, (u32*)&font_0, 96);
	
	for(u16 p = 0; p < EXAMPLE_WIDTH*EXAMPLE_HEIGHT; p++)
	{
		testBuffer[p] = '!';
	}
	DMA_TEST = false;
	// Enable VBlank
	WriteVDPRegister(WRITE|REG(1)|(MODE|VBLIRQ_ON|DMA_OFF|NTSC|Video_ON));
	while(1)
	{
		cycles = 0;
		// Wait for VBL to finish
		VBL_DONE = false;
		while(!VBL_DONE)
		{ 
			cycles++;
		}
		//Print test string @ 16X 5Y
		//if(DMA_TEST) {
			SetVRAMWriteAddress(VRAM_BG_B + (BG_WIDTH*5*2) + (16*2)); 
			u8* chp = (u8*)byToHex((u8)((cycles & 0xff00)>>8));  
			for(c = 0; c < 2; c++) WRITE_DATAREG16((u16)*chp++); 
			chp = (u8*)byToHex((u8)((cycles & 0xff)));  
			for(c = 0; c < 2; c++) WRITE_DATAREG16((u16)*chp++);
			chp = (u8*)hw;
			for(c = 0; c < sizeof(hw); c++) WRITE_DATAREG16((u16)*chp++);
		//}
	}
}

static u8 BYTOHEXWORK[3] = "  ";
u8* byToHex(u8 by)
{
	//u8 BYTOHEXWORK[3] = "  ";
    u8 a = by & 0xf;
    u8 b = (by & 0xf0) >> 4;
    if (a > 9) a += 7;
    if (b > 9) b += 7;
    a += 0x30;
    b += 0x30;
    BYTOHEXWORK[0] = b; BYTOHEXWORK[1] = a; BYTOHEXWORK[2] = 0;
    return &BYTOHEXWORK;
}

void DMAExample(u16* buffer, u8 layer)
{
	u32 TARGET;
	if (layer == BG_A)
		TARGET = (VRAM_BG_A); // calculate offset here 
	else 
		TARGET = (VRAM_BG_B); 
	
	u32 dest = 0x40000000+((TARGET & 0x3fff)<<16)+((TARGET & 0xc000)>>14);
	dest |= (u32)(0x80); // BIT 7 MUST BE SET (CD5) FOR DMA RAM->VRAM
	
	// DMA on
	WriteVDPRegister(WRITE|REG(1)|(MODE|VBLIRQ_ON|DMA_ON|NTSC|Video_ON));
	//WriteVDPRegister(WRITE|REG(0xf)|2); // Auto-increment to 2, if needed
	SetDMALength(2048); // Amount (in words) to transfer over DMA
	// Write source address (in WORD): 
	SetDMASourceAddress((u32)buffer);
	WRITE_CTRLREG(dest); // This triggers DMA
	// Done! DMA off 
	WriteVDPRegister(WRITE|REG(1)|(MODE|VBLIRQ_ON|DMA_OFF|NTSC|Video_ON));

	DMA_TEST = true;
}

void GAME_DRAW()
{	
	//if(!DMA_TEST)
		DMAExample(&testBuffer, BG_A);

	VBL_DONE = true;
}

//////////////////////////
// DMA Loop version
void DMAExampleLoop(u16* buffer, u8 layer)
{
	u32 TARGET;
	u32 src;
	u32 dest;
	if (layer == BG_A)
		TARGET = (VRAM_BG_A); // calculate offset here 
	else 
		TARGET = (VRAM_BG_B); 

	src = (u32)buffer;
	WriteVDPRegister(WRITE|REG(1)|(MODE|VBLIRQ_ON|DMA_ON|NTSC|Video_ON));
	//WriteVDPRegister(WRITE|REG(0xf)|2); // Auto-increment to 2 if needed
	for(u8 i = 0; i < 32; i++)
	{
		SetDMALength(64); // Amount (in words) to transfer over DMA
		SetDMASourceAddress(src); // source (in words)
		dest = 0x40000000|(((TARGET & 0x3fff)<<16)|((TARGET & 0xc000)>>14));
		dest |= (u32)(0x80); // BIT 7 MUST BE SET (d5) FOR DMA!
		WRITE_CTRLREG(dest); // This triggers DMA
		src += 64*2;
		TARGET += 64*2; 
	}
	WriteVDPRegister(WRITE|REG(1)|(MODE|VBLIRQ_ON|DMA_OFF|NTSC|Video_ON));
		
	DMA_TEST = true;
}
