# png2gen.py
import numpy,sys
from PIL import Image,ImageDraw
# open image 
f = Image.open(sys.argv[1])
px = f.load()
imsize = f.size
f.close()
outcstr = ''
tile = 0
h = 0
while h < imsize[1]:
    w = 0
    while w < imsize[0]:
        tilestr = 'static const u32 tile_' + str(tile) + '[8] = {'
        y = 0
        while y < 8:
            lwstr = '\n0x'
            x = 0
            while x < 8:
                lwstr = lwstr + hex(px[x+w,y+h])[2:]
                x += 1
            tilestr += lwstr + ','
            y += 1
        outcstr += tilestr + '\n};'
        tile += 1
        w += 8
    h += 8
print(sys.argv[1],imsize[0],imsize[1])
f = open('tiles.h', 'w')
f.write(outcstr)
f.close()
