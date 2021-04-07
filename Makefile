CC=m68k-elf-gcc 
OBJCOPY=m68k-elf-objcopy
LIBFIX=-nostdlib

main.bin: main.elf main.s
	${OBJCOPY} -O binary main.elf main.bin

%.elf: %.s
	${CC} ${LIBFIX} -o main.elf main.s

%.s: %.c
	${CC} -S ${LIBFIX} -o main.s main.c

clean:
	rm -rf *.bin *.elf *.s
