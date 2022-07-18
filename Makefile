# Makefile for MD / Genesis ROM
# @RetroDevDiscord
#==============================
UNAME_:=$(shell uname -s)
ifeq ($(UNAME_),Darwin)
  TCDIR:=/usr/local/m68k/bin/m68k-elf-
else 
  TCDIR:=~/Downloads/m68k-toolchain/bin/m68k-elf-
endif
  CC:=$(TCDIR)gcc
  AS:=$(TCDIR)as
  LD:=$(TCDIR)ld

CFLAGS=-Wall -Werror=implicit-function-declaration \
	-nostdlib \
	-O0 -m68000 \
	-std=gnu99 -fno-pie -no-pie -fno-use-linker-plugin\
	-fomit-frame-pointer -fno-inline -fno-builtin-inline 
ASFLAGS=-march=68000 --register-prefix-optional 
LDFLAGS=
MKDIR=mkdir
BUILDDIR=build
OUTDIR=out
PYTHON=python3
## Set PROJECT var here, or on command line:
PROJECT=projects/starthrall
ifeq ($(PROJECT),)
.ONESHELL:
default:
	$(info PROJECT var not set!)
	@$(exit)
endif
SRCDIR:=${PROJECT}
EMUDIR:=
LIBDIR:=$(shell pwd)
LINKSCR=rom.ld
ROMFILE=out.md

# Link order should be sega.s, main.s, everything else
main: DIRs
	${CC} ${CFLAGS} -v \
		-I${LIBDIR}/lib \
		-I${PROJECT}/src \
		-I${PROJECT}/res\
		-I${PROJECT}/res/maps\
		-S ${SRCDIR}/main.c \
		-o ${BUILDDIR}/main.s
	${AS} ${ASFLAGS} -g -als=${OUTDIR}/listing.lst \
		-o ${BUILDDIR}/main.o \
		${LIBDIR}/lib/sega.s \
		${BUILDDIR}/main.s \
		${LIBDIR}/lib/68kmath.s  
	${LD} -s -Tlib/${LINKSCR} -o ${OUTDIR}/_main.rom ${BUILDDIR}/main.o -Map=${BUILDDIR}/rom.map
	${PYTHON} tools/padrom.py ${OUTDIR}/_main.rom ./$(ROMFILE)
	rm -rf ${OUTDIR}/_main.rom 

run:
	$(EMUDIR)/dgen $(ROMFILE) &>/dev/null

vgmplayer:
	lib/makez80.sh


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
