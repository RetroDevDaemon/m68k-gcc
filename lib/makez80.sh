python3 ./tools/macroize.py lib/vgmplay2.z80 > lib/_vgmplay.asm
~/Downloads/z80-asm-2.4.1/z80-asm -l lib/_vgmplay.asm 0:lib/vgmplayer.bin > lib/vgmplay.lst
python3 ./tools/fronttrim.py lib/vgmplayer.bin 10
python3 ./tools/bin2c.py lib/vgmplayer.bintrimmed > lib/vgmplayer.h
rm -f lib/vgmplayer.bin 
#rm -f vgmplay.lst 
rm -f lib/_vgmplay.asm
rm -f lib/vgmplayer.bintrimmed 
#echo "vgmplayer.h written."