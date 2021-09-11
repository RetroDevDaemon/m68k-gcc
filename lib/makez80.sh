python3 ../tools/macroize.py vgmplay_m.z80 > _vgmplay.asm
z80-asm -l _vgmplay.asm 0:vgmplayer.bin > vgmplay.lst
python3 ../tools/fronttrim.py ./vgmplayer.bin 10
python3 ../tools/bin2c.py ./vgmplayer.bintrimmed > vgmplayer.h
rm -f vgmplayer.bin 
rm -f vgmplay.lst 
rm -f _vgmplay.asm
rm -f vgmplayer.bintrimmed 
echo "vgmplayer.h written."
