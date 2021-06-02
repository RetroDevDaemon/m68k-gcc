#!/usr/bin/python3 
# pal2gen.py
# 0x0BGR - values of only 02468ACE
import os,numpy,sys,math
from PIL import Image,ImageDraw
f = Image.open(sys.argv[1]).convert('RGBA')
px = f.load()
f.close()
bn = os.path.basename(sys.argv[1])
bn = bn.split('.')[0]
outcstr = 'const u16 ' + bn + '[] = { \n\t'
i = 0
while i < 16:
    palw = 0
    cr = int(px[i,0][0] / 32) * 2
    cg = int(px[i,0][1] / 32) * 2
    cb = int(px[i,0][2] / 32) * 2
    palw = cb << 8
    palw |= cg << 4
    palw |= cr
    outcstr += hex(palw) + ', '
    if (i+1) % 4 == 0:
        outcstr += '\n\t'
    i += 1
outcstr += '\n};\n'
f = open(bn + '.h', 'w')
f.write(outcstr)
f.close()
