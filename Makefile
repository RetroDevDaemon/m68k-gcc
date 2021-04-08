# Makefile for MD / Genesis ROM
# @RetroDevDiscord
#==============================
CC=m68k-elf-gcc -nostdlib -std=gnu11 -fno-pie -no-pie 
OBJCOPY=m68k-elf-objcopy
LD=m68k-elf-ld 
OBJDUMP=m68k-elf-objdump
MKDIR=mkdir 
BUILDDIR=build
SHOWELF=0
SRCDIR=src
OUTDIR=out
PYTHON=python3
LINKSCR=rom.ld

%.bin: %.elf
	${LD} -s -T${LINKSCR} -o ${OUTDIR}/$@ ${BUILDDIR}/$< 

%.elf: %.s
	${CC} -c -o ${BUILDDIR}/$@ ${BUILDDIR}/$<
	@if [ ${SHOWELF} -eq 1 ]; then ${OBJDUMP} -dS build/$@ > build/$@txt; fi

%.s: ${SRCDIR}/%.c
	${CC} -S -c -o ${BUILDDIR}/$@ $<

# Create the main rom and pad it
main: header main.bin 
	cat ${OUTDIR}/header.bin ${OUTDIR}/main.bin > ./out.md 
	${PYTHON} tools/padrom.py	

# Assemble the header from byte listing
header: DIRs rom.ld 
	${CC} -o ${BUILDDIR}/header.elf ${SRCDIR}/header.s
	@echo --But it's the header, so it's ok!
	${OBJCOPY} -O binary ${BUILDDIR}/header.elf ${OUTDIR}/header.bin

# Create build directory	
DIRs:
	@if [ -d "${BUILDDIR}" ]; then echo "dir exists"; else ${MKDIR} ${BUILDDIR}; fi
	@if [ -d "${OUTDIR}" ]; then echo "dir exists"; else ${MKDIR} ${OUTDIR}; fi

clean:
	rm -rf ${BUILDDIR} 
	rm -rf ${OUTDIR}
	rm -rf out.md
