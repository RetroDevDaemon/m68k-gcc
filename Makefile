CC=m68k-elf-gcc -nostdlib
OBJCOPY=m68k-elf-objcopy
OBJDUMP=m68k-elf-objdump
MKDIR=mkdir 
BUILDDIR=build
SHOWELF=0
SRCDIR=src
OUTDIR=out

%.bin: %.elf
	${OBJCOPY} -O binary ${BUILDDIR}/$< ${OUTDIR}/$@

%.elf: %.s
	${CC} -o ${BUILDDIR}/$@ ${BUILDDIR}/$<
	@if [ ${SHOWELF} -eq 1 ]; then ${OBJDUMP} -dS build/$@ > build/$@txt; fi

%.s: ${SRCDIR}/%.c
	${CC} -S -o ${BUILDDIR}/$@ $<

# Create the main binary, listing, and ROM header
main: header main.bin 
	@echo $^ created successfully.

# Assemble the header from byte listing
header: DIRs
	${CC} -o ${BUILDDIR}/header.elf src/header.s
	@echo --But it's the header, so it's ok!
	${OBJCOPY} -O binary ${BUILDDIR}/header.elf ${OUTDIR}/header.bin

# Create build directory	
DIRs:
	@if [ -d "${BUILDDIR}" ]; then echo "dir exists"; else ${MKDIR} ${BUILDDIR}; fi
	@if [ -d "${OUTDIR}" ]; then echo "dir exists"; else ${MKDIR} ${OUTDIR}; fi

clean:
	rm -rf ${BUILDDIR} 
	rm -rf ${OUTDIR}
	rm -rf *.bin 
