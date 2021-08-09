z80-asm z80prg.z80 0:z80prg.bin
python3 ../../tools/fronttrim.py ./z80prg.bin 10
python3 ../../tools/bin2c.py ./z80prg.bintrimmed > z80prg.h
rm z80prg.bin 
rm z80prg.bintrimmed 
echo "z80prg.h written."