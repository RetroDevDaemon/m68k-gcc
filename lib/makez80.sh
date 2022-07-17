python3 ./tools/macroize.py ./lib/vgmplay_m.z80 > ./lib/_vgmplay.asm
z80asm -v -mz80 -b -l ./lib/_vgmplay.asm
#python3 ./tools/fronttrim.py lib/vgmplayer.bin 10
python3 ./tools/bin2c.py lib/_vgmplay.bin > lib/vgmplayer.h
#rm -f ./lib/_v*