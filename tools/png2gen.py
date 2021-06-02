# png2gen.py
import numpy,sys,os
from PIL import Image,ImageDraw
# open image 
f = Image.open(sys.argv[1])
px = f.load()
bgmode = False
if (type(px[0,0]) == tuple):
    print('not indexed image. quitting...')
    sys.exit()
if(len(sys.argv) > 2):
    if(sys.argv[2] == '-b'):
        bgmode = True
        print('using left-to-right (background) mode...')
imsize = f.size
f.close()
outcstr = ''
padd = False
bn = os.path.basename(sys.argv[1])
bn = bn.split('.')[0]
tile = 0
if(bgmode == False):
    w = 0 # go width first so we go top to bottom, left to right
    while w < imsize[0]:
        h = 0
        while h < imsize[1]:
            tilestr = 'const u32 ' + bn + '_' + str(tile) + '[] = {'
            y = 0
            while y < 8:
                lwstr = '\n\t0x'
                x = 0
                while x < 8:
                    if ((h+y) < imsize[1]) and ((w+x) < imsize[0]):
                        lwstr = lwstr + hex(int(px[x+w,y+h]))[2:]
                    else:
                        lwstr = lwstr + '0'
                        padd = True
                    x += 1
                tilestr += lwstr + ','
                y += 1
            outcstr += tilestr + '\n};\n'
            tile += 1
            h += 8
        w += 8
else:
    h = 0
    while h < imsize[1]:
        w = 0
        while w < imsize[0]:
            tilestr = 'const u32 ' + bn + '_' + str(tile) + '[] = {'
            y = 0
            while y < 8:
                lwstr = '\n\t0x'
                x = 0
                while x < 8:
                    if((h+y) < imsize[1]) and ((w+x) < imsize[0]):
                        lwstr = lwstr + hex(int(px[x+w,y+h]))[2:]
                    else:
                        lwstr = lwstr + '0'
                        padd = True
                    x += 1
                tilestr += lwstr + ','
                y += 1
            outcstr += tilestr + '\n};\n'
            tile += 1
            w += 8
        h += 8

print(bn + '.h written. image size:',imsize[0],imsize[1], 'tiles written',tile)
if(padd):
    print('warning: image size not multiple of 8, tiles padded!')
f = open(bn + '.h', 'w')
f.write(outcstr)
f.close()
