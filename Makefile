# Makefile for MD / Genesis ROM
# @RetroDevDiscord
#==============================
CC=m68k-elf-gcc 
CFLAGS=-nostdlib -m68000 -std=gnu11 -fno-pie -no-pie -fno-use-linker-plugin -fomit-frame-pointer 
AS=m68k-elf-as
ASFLAGS=-march=68000 --register-prefix-optional 
LD=m68k-elf-ld 
LDFLAGS=
MKDIR=mkdir 
BUILDDIR=build
SRCDIR=src
OUTDIR=out
PYTHON=python3
LINKSCR=rom.ld

# Link order should be sega.s, main.s, everything else
main: DIRs
	${CC} ${CFLAGS} -Isrc -fno-inline -fno-builtin-inline -S ${SRCDIR}/main.c -o ${BUILDDIR}/main.s
	${AS} ${ASFLAGS} -o ${BUILDDIR}/main.o ${SRCDIR}/sega.s ${BUILDDIR}/main.s ${SRCDIR}/68kmath.s  
	${LD} -s -Tsrc/${LINKSCR} -o _main.rom ${BUILDDIR}/main.o
	${PYTHON} tools/padrom.py _main.rom
	rm -rf _main.rom 

run: main 
	dgen out.md 

# Create build directory	
DIRs:
	@if [ -d "${BUILDDIR}" ]; then echo "dir exists"; else ${MKDIR} ${BUILDDIR}; fi
	@if [ -d "${OUTDIR}" ]; then echo "dir exists"; else ${MKDIR} ${OUTDIR}; fi

clean:
	rm -rf ${BUILDDIR} 
	rm -rf ${OUTDIR}
	rm -rf out.md
