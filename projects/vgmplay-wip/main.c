
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

#include "pcmdata.h"
#include "vgmdata.h"

static u16 tileindex;
static u16 joyState1;
static u16 last_joyState1;
static bool VBL_DONE = false;

extern const char nullsong;

static struct _counters \
{ 
    u8 sixtyFrameCounter;
    u8 thirtyFrameCounter;
    u8 twentyFrameCounter;
    u8 tenFrameCounter;
} Counters;

///// DEBUG 
void DO_DEBUG(void);
static struct _debugvars \
{ 
    s32 cycles;
    s32 vcycles;
    u8 vch[3];
    u8 vcl[3];
    u8 _zero;
    u8 ch[3];
    u8 cl[3];
    u8 _zeroa;
    u8 zh[3];
    u8 zl[3];
    bool debug_text_enabled;
} debugVars;
void __attribute__((optimize("O3"))) UpdateDebugText()
{
	extern u8 zcyclesl;
	extern u8 zcyclesh;

	// Every word write to the VDP is ~2 cycles. This takes up 24c!
	//vdp_print(VRAM_BG_A, 5, 1, (u8*)debugVars.ch);
    vdp_print(VRAM_BG_A, 5, 1, (char*)debugVars.ch);
	vdp_print(VRAM_BG_A, 7, 1, (char*)debugVars.cl);
    vdp_print(VRAM_BG_A, 5, 3, (char*)debugVars.vch);
	vdp_print(VRAM_BG_A, 7, 3, (char*)debugVars.vcl);
	vdp_print(VRAM_BG_A, 5, 5, (char*)debugVars.zh);
	vdp_print(VRAM_BG_A, 7, 5, (char*)debugVars.zl);
	
}
void __attribute__((optimize("O3"))) DO_DEBUG(void)
{
    // Debug Menu 
    UpdateDebugText();
    debugVars.vcycles = 0;
    while(*((u32*)0xc00004) & 0b1000)
        debugVars.vcycles++;
}
//////////// END DEBUG 


u16 frameCounter = 0;

int main()
{       
	u32 c;
	u16* zp;
	
	LoadPalette(0, (u16*)&palette);
#define ASCIIBASETILE 32
	tileindex = VDPLoadTiles(ASCIIBASETILE, (u32*)&font_0, 96);

	LoadSong((u32)&vgmdata[256]);
	//LoadSong(&nullsong);
	EnableVBlankIRQ();
	
    u32* vp = (u32*)&pcmdata[0];
    for(int i = 0; i < 64; i++)
        SFX[i] = (u32)&pcmdata[0] + (u32)*vp++;
    
	u8 sfxno = 0;
	//
	debugVars.debug_text_enabled = true;

	while(1)
	{
		VBL_DONE = false;
		// Print cycle debug
		debugVars.cycles = 0;
		while(!VBL_DONE)
			debugVars.cycles++;
		if(debugVars.debug_text_enabled)
		{
			byToHex(debugVars.vcycles >> 8, (u8*)&debugVars.vch);
			byToHex(debugVars.vcycles & 0xff, (u8*)&debugVars.vcl);
			byToHex(debugVars.cycles >> 8, (u8*)&debugVars.ch);
			byToHex(debugVars.cycles & 0xff, (u8*)&debugVars.cl);
			byToHex(zcyclesh, (u8*)&debugVars.zh);
			byToHex(zcyclesl, (u8*)&debugVars.zl);           
		}
		// Counter-y stuff
        Counters.twentyFrameCounter++;
        if(Counters.twentyFrameCounter > 19) Counters.twentyFrameCounter = 0;
        Counters.tenFrameCounter++;
        if(Counters.tenFrameCounter > 9) Counters.tenFrameCounter = 0;
        Counters.thirtyFrameCounter++;
        if(Counters.thirtyFrameCounter > 29) Counters.thirtyFrameCounter = 0;
		
	}
	return 0;
}

void GAME_DRAW()
{
	GETJOYSTATE1(joyState1);
	if(Joy1Down(BTN_C_PRESSED)) // Is (C) pressed?
		bga_hscroll_pos++;

	PlaySong();
	// Play cymbal crash every second
	frameCounter++;
	if (frameCounter == 60) {
		PlaySFX(SFX[0], SFX[0+1]);
		frameCounter = 0;
	}
	UpdateBGScroll();
	
	vdp_print(VRAM_BG_A, 8, 8, "Hello DAC!\x00");
	//if(Counters.thirtyFrameCounter == 0)
	{
        if(debugVars.debug_text_enabled)
            DO_DEBUG();
    }

	VBL_DONE = true;
}
