# Makefile for MD / Genesis ROM
# @RetroDevDiscord
#==============================
CC=m68k-elf-gcc 
#CC=m68k-apple-macos-gcc
CFLAGS=-nostdlib -m68000 -std=gnu11 -fno-pie -no-pie -fno-use-linker-plugin -fomit-frame-pointer
AS=m68k-elf-as
#AS=m68k-apple-macos-as
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

%.o: %.s header2.bin
	${CC} ${CFLAGS} -Wl,-Ttext=$(shell python ./tools/getfshx.py) -o ${BUILDDIR}/$@ ${BUILDDIR}/$<
	@if [ ${SHOWELF} -eq 1 ]; then ${OBJDUMP} -dS build/$@ > build/$@txt; fi

%.s: ${SRCDIR}/%.c
	${CC} ${CFLAGS} -O2 -S -c -o ${BUILDDIR}/$@ $<

# Create the main rom and pad it
main: DIRs main.bin header2.bin
	cat ${OUTDIR}/header2.bin ${OUTDIR}/main.bin > ./out.md 
	${PYTHON} tools/padrom.py	

	
# Assemble the header from byte listing
header2.bin: 
	${AS} -o ${BUILDDIR}/header2.o -march=68000 --register-prefix-optional ${SRCDIR}/header2.s
	${LD} -s -T${LINKSCR} -o ${OUTDIR}/header2.bin ${BUILDDIR}/header2.o

# Create build directory	
DIRs:
	@if [ -d "${BUILDDIR}" ]; then echo "dir exists"; else ${MKDIR} ${BUILDDIR}; fi
	@if [ -d "${OUTDIR}" ]; then echo "dir exists"; else ${MKDIR} ${OUTDIR}; fi

clean:
	rm -rf ${BUILDDIR} 
	rm -rf ${OUTDIR}
	rm -rf out.md
