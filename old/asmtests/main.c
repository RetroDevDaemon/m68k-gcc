//main.c
#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800
#define VRAM_SCROLL 0xfc00

#define BG_WIDTH 64
#define BG_HEIGHT 32

// My own genesis stuff 
#include "bentgen.h"

#include "z80prg.h"

int addr_a = 0;
int addr_b = 0;


int main()
{
	while(1){}
    
	// Z80 START-UP SEQUENCE: 
	// BUS REQ ON
	// BUS RESET OFF
        asm("\n\t"\
	"movea.l #0xa11100,%%a2\n\t"\
	"move.w  #0x100,(%%a2)\n\t"\
	"movea.l #0xa11200,%%a3\n\t"\
	"move.w  #0x100,(%%a3)\n\t"\
	:::"a2","a3");
	// WAIT FOR BUS TO BE READY
	asm(".Z80BUSWAIT:\n\t"\
	"move.b  (0xa11100),%%d1\n\t"\
	"btst    #0,%%d1\n\t" 
	"bne 	 .Z80BUSWAIT\n\t" 
	"movea.l %0,%%a0\n\t"\
	"movea.l #0xa00000,%%a1\n\t"\
	::"g"(&z80prg):"a0","a1","d1");
	// COPY z80prg[] INTO Z80 MEMORY (A00000+)
	asm(\
	"move.l %0,%%d1"\
	::"g"(sizeof(z80prg)):"d1");
	asm(".Z80COPYLOOP:\n\t"\
	"move.b (%%a0)+,%%d0\n\t"\
	"move.b %%d0,(%%a1)+\n\t"\
	"subq 	#1,%%d1\n\t"\
	"bne 	.Z80COPYLOOP\n\t"\
	:::"d0","d1","a0","a1");
	// BUS RESET ON 
	// BUS REQ OFF
	// BUS RESET OFF 
	asm("move.w #0,(%%a3)\n\t"\
	"move.w #0,(%%a2)\n\t"\
	"move.w #0x100,(%%a3)\n\t"\
	:::"a3","a2");
	
	return 0;
	
}

void GAME_DRAW()
{
	
}