CC=m68k-elf-gcc -nostdlib
OBJCOPY=m68k-elf-objcopy
MKDIR=mkdir 
BUILDDIR=build

main: main.elf main.s header
	${OBJCOPY} -O binary ${BUILDDIR}/main.elf ./main.bin

%.elf: %.s
	${CC} -o ${BUILDDIR}/main.elf ${BUILDDIR}/main.s

%.s: %.c tmp
	${CC} -S -o ${BUILDDIR}/main.s main.c

header: header.s tmp
	${CC} -o ${BUILDDIR}/header.elf header.s
	@echo --But it's the header, so it's ok!
	${OBJCOPY} -O binary ${BUILDDIR}/header.elf ${BUILDDIR}/header.bin
	
tmp:
	@if [ -d "${BUILDDIR}" ]; then echo "dir exists"; else ${MKDIR} ${BUILDDIR}; fi
	@touch ${BUILDDIR}/tmp 

clean:
	rm -rf ${BUILDDIR} 
	rm -rf main.bin 
