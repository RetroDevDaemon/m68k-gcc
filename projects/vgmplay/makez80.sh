z80-asm -l vgmplay.asm 0:vgmplayer.bin > vgmplay.lst
python3 ../../tools/fronttrim.py ./vgmplayer.bin 10
python3 ../../tools/bin2c.py ./vgmplayer.bintrimmed > vgmplayer.h
rm vgmplayer.bin 
#rm vgmplayer.bintrimmed 
echo "vgmplayer.h written."
