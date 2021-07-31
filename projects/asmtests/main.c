//main.c
#define VRAM_BG_B 0xe000
#define VRAM_BG_A 0xc000
#define VRAM_SAT 0xd800
#define VRAM_SCROLL 0xfc00

#define BG_WIDTH 64
#define BG_HEIGHT 32

// My own genesis stuff 
#include "bentgen.h"

const unsigned char z80prg[] = {
	0x21, 0x27, 0x0, 0x1, 0x8, 0x0, 0xc5, 0x46, 0x23, 0x4e, 0x23, 0xe5, 0xcd, 0x19, 0x0, 0xe1, 
	0xc1, 0xb, 0x79, 0xb0, 0x20, 0xf0, 0xc3, 0x16, 0x0, 0x21, 0x0, 0x40, 0x70, 0x23, 0x71, 0x3a, 
	0x0, 0x40, 0xcb, 0x7f, 0x20, 0xf9, 0xc9, 0x22, 0x0, 0x27, 0x0, 0x28, 0x0, 0x2b, 0x0, 
};

int addr_a;
int addr_b;

void main()
{
	
        asm("\n\t"\
	"movea.l #0xa11100,%%a2\n\t"\
	"move.w  #0x100,(%%a2)\n\t"\
	"movea.l #0xa11200,%%a3\n\t"\
	"move.w  #0x100,(%%a3)\n\t":::"a2","a3");
	/*
	asm("\n\t"\
	"move.w #0x100,0xa11100\n\t"\
	"move.w #0x100,0xa11200\n\t"\
	);
	*/
	asm(".Z80BUSWAIT:\n\t"\
	"move.b  (0xa11100),%%d1\n\t"\
	"btst    #0,%%d1\n\t" 
	"bne 	 .Z80BUSWAIT\n\t" 
	"movea.l %0,%%a0\n\t"\
	"movea.l #0xa00000,%%a1\n\t"\
	:\
	:"g"(&z80prg)\
	:"a2","a0","a1","d1");
	asm(\
	"move.l %0,%%d1"::"g"(sizeof(z80prg)):"d1");
	asm(".Z80COPYLOOP:\n\t"\
	"move.b (%%a0)+,%%d0\n\t"\
	"move.b %%d0,(%%a1)+\n\t"\
	"subq 	#1,%%d1\n\t"\
	"bne 	.Z80COPYLOOP\n\t"\
	:::"d0","d1","a0","a1");
	asm("move.w #0,(%%a3)\n\t"\
	"move.w #0,(%%a2)\n\t"\
	"move.w #0x100,(%%a3)\n\t":::"a3","a2");
	char stringy[4] = "Hw!";
	print(BG_A, 10, 10, stringy);
	while(1){}
	
}

void GAME_DRAW()
{

}