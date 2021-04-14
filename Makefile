# Makefile for MD / Genesis ROM
# @RetroDevDiscord
#==============================
CC=m68k-elf-gcc 
CFLAGS=-nostdlib -m68000 -std=gnu11 -fno-pie -no-pie -fno-use-linker-plugin -fomit-frame-pointer
AS=m68k-elf-as
OBJCOPY=m68k-elf-objcopy
#OBJCOPY=m68k-apple-macos-objcopy
LD=m68k-elf-ld 
#LD=m68k-apple-macos-ld
OBJDUMP=m68k-elf-objdump
#OBJDUMP=m68k-apple-macos-objdump
MKDIR=mkdir 
BUILDDIR=build
SHOWELF=1
SRCDIR=src
OUTDIR=out
PYTHON=python3
LINKSCR=rom.ld

%.bin: %.o
	${LD} -s -T${LINKSCR} -o ${OUTDIR}/$@ ${BUILDDIR}/$< 

%.o: %.s header.bin
	${CC} ${CFLAGS} -Wl,-Ttext=$(shell python3 ./tools/getfshx.py) -o ${BUILDDIR}/$@ ${BUILDDIR}/$<
	@if [ ${SHOWELF} -eq 1 ]; then ${OBJDUMP} -dS ${BUILDDIR}/$@ > ${BUILDDIR}/$@txt; fi

%.s: ${SRCDIR}/%.c
	${CC} ${CFLAGS} -O2 -S -c -o ${BUILDDIR}/$@ $<

# Create the main rom and pad it
main: DIRs main.bin header.bin
	cat ${OUTDIR}/header.bin ${OUTDIR}/main.bin > ./out.md 
	${PYTHON} tools/padrom.py	

# Assemble the header from byte listing
header.bin: 
	${AS} -o ${BUILDDIR}/header.o -march=68000 --register-prefix-optional ${SRCDIR}/header.s
	${LD} -s -T${LINKSCR} -o ${OUTDIR}/header.bin ${BUILDDIR}/header.o

# Create build directory	
DIRs:
	@if [ -d "${BUILDDIR}" ]; then echo "dir exists"; else ${MKDIR} ${BUILDDIR}; fi
	@if [ -d "${OUTDIR}" ]; then echo "dir exists"; else ${MKDIR} ${OUTDIR}; fi

clean:
	rm -rf ${BUILDDIR} 
	rm -rf ${OUTDIR}
	rm -rf out.md
