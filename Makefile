# Makefile for MD / Genesis ROM
# @RetroDevDiscord
#==============================
CC=m68k-elf-gcc 
CFLAGS=-nostdlib -m68000 -std=gnu11 -fno-pie -no-pie -fno-use-linker-plugin -fomit-frame-pointer 
AS=m68k-elf-as
ASFLAGS=-march=68000 --register-prefix-optional 
OBJCOPY=m68k-elf-objcopy
LD=m68k-elf-ld 
OBJDUMP=m68k-elf-objdump
MKDIR=mkdir 
BUILDDIR=build
SHOWELF=1
SRCDIR=src
OUTDIR=out
PYTHON=python3
LINKSCR=rom.ld
HEADERSZ=0x32c

main: DIRs
	${CC} ${CFLAGS} -Ttext=${HEADERSZ} -S ${SRCDIR}/main.c -o ${BUILDDIR}/main.s
	${AS} ${ASFLAGS} -o ${BUILDDIR}/main.o ${SRCDIR}/sega.s ${BUILDDIR}/main.s 
	${LD} -s -T${LINKSCR} -o _main.rom ${BUILDDIR}/main.o
	${PYTHON} tools/padrom.py _main.rom

# Create build directory	
DIRs:
	@if [ -d "${BUILDDIR}" ]; then echo "dir exists"; else ${MKDIR} ${BUILDDIR}; fi
	@if [ -d "${OUTDIR}" ]; then echo "dir exists"; else ${MKDIR} ${OUTDIR}; fi

clean:
	rm -rf ${BUILDDIR} 
	rm -rf ${OUTDIR}
	rm -rf out.md
