# Makefile for MD / Genesis ROM
# @RetroDevDiscord
#==============================
CC=m68k-elf-gcc 
CFLAGS=-nostdlib -O0 -m68000 -std=gnu11 -fno-pie -no-pie -fno-use-linker-plugin -fomit-frame-pointer -fno-inline -fno-builtin-inline 
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
	${CC} ${CFLAGS} -v -Isrc -Ires -S ${SRCDIR}/main.c -o ${BUILDDIR}/main.s
	${AS} ${ASFLAGS} -als=${OUTDIR}/listing.lst -o ${BUILDDIR}/main.o ${SRCDIR}/sega.s ${BUILDDIR}/main.s ${SRCDIR}/68kmath.s  
	${LD} -s -Tsrc/${LINKSCR} -o ${OUTDIR}/_main.rom ${BUILDDIR}/main.o 
	${PYTHON} tools/padrom.py ${OUTDIR}/_main.rom
	rm -rf ${OUTDIR}/_main.rom 

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
