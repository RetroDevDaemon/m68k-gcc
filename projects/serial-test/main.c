
//main.c
#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800
#define VRAM_SCROLL 0xfc00

#define BG_WIDTH 64
#define BG_HEIGHT 32

#include <bentgen.h>

#define IoCtrl2 0xa1000b
#define IoData2 0xa10005
#define IoSCtrl2 0xa10019
#define IoRxData2 0xa10017
#define IoTxData2 0xa10015

#include "font.h"
#include "palette.h"

static u16 tileindex;

const char hw[] = "Sending byte: '!' ...\000";
const char eg[] = "SENT\000";
const char egclr[]="    \000";

int main()
{       
        u16 c;
        //*((u8*)IoSCtrl2) = 0b00110000;
        //asm("move.b #0b01111111, (0xa1000b)"); // all output
        vu8* cl = (vu8*)IoCtrl2;
        *cl = 0b01111111;
        //asm("move.b #0b11110000, (0xa10019)"); //300bps
        cl = (vu8*)IoSCtrl2;
        *cl = 0b11110000;
	// Load palette
        LoadPalette(0, (u16*)&palette);
        
	// Load font
#define ASCIIBASETILE 32
        tileindex = VDPLoadTiles(ASCIIBASETILE, (u32*)&font_0, 96);

        //Print
        vdp_print(VRAM_BG_A, 5, 5, hw);

        
        int totalSent = 0;
        

        while(1)
        {
                char frameCtr = 0;
                while(frameCtr < 60){
                        WaitVBlank();
                        frameCtr++;
                }
                totalSent++;
                // forever wait for sctl to be ready and send data byte "!" through Tx
                asm("move.l #0xa10019,%%a0\n\t"\
                "move.l #0x00000021,%%d0\n\t"\
                "Wait%=:\n\t"\
                "btst #0, (%%a0)\n\t"\
                "bne.s Wait%=\n\t"\
                "move.b %%d0, (0xa10015)":::"d0","a0"); 
                vdp_print(VRAM_BG_A, 5, 8, eg);
                char bf[3] = { 0, 0, 0};
                byToHex(totalSent & 0xff, &bf);
                vdp_print(VRAM_BG_A, 5, 9, bf);
        }
        return 0;
}

void GAME_DRAW()
{}