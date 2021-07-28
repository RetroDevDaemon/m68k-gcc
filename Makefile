# Makefile for MD / Genesis ROM
# @RetroDevDiscord
#==============================
CC=m68k-elf-gcc 
CFLAGS=-nostdlib -O0 -m68000 -std=gnu11 -fno-pie -no-pie -fno-use-linker-plugin\
	-fomit-frame-pointer -fno-inline -fno-builtin-inline 
AS=m68k-elf-as
ASFLAGS=-march=68000 --register-prefix-optional 
LD=m68k-elf-ld 
LDFLAGS=
MKDIR=mkdir 
BUILDDIR=build
OUTDIR=out
PYTHON=python3
PROJECT=
ifeq ($(PROJECT),)
.ONESHELL:
default:
	$(info PROJECT var not set!)
	@$(exit)
endif
SRCDIR=${PROJECT}
LINKSCR=rom.ld
ROMFILE=out.md

# Link order should be sega.s, main.s, everything else
main: DIRs
	${CC} ${CFLAGS} -v \
		-Ilib \
		-I${PROJECT}/src \
		-I${PROJECT}/res\
		-I${PROJECT}/res/maps\
		-S ${SRCDIR}/main.c \
		-o ${BUILDDIR}/main.s
	${AS} ${ASFLAGS} -g -als=${OUTDIR}/listing.lst \
		-o ${BUILDDIR}/main.o \
		lib/sega.s \
		${BUILDDIR}/main.s \
		lib/68kmath.s  
	${LD} -s -Tlib/${LINKSCR} -o ${OUTDIR}/_main.rom ${BUILDDIR}/main.o -Map=map.txt
	${PYTHON} tools/padrom.py ${OUTDIR}/_main.rom ./$(ROMFILE)
	rm -rf ${OUTDIR}/_main.rom 

run: main 
	dgen $(ROMFILE)

# Create build directory	
DIRs:
	@if [ -d "${BUILDDIR}" ]; then echo "dir exists"; \
		else ${MKDIR} ${BUILDDIR}; fi
	@if [ -d "${OUTDIR}" ]; then echo "dir exists"; \
		else ${MKDIR} ${OUTDIR}; fi

clean:
	rm -rf ${BUILDDIR} 
	rm -rf ${OUTDIR}
	rm -rf $(ROMFILE)
