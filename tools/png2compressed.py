#!/usr/bin/python3
# png2compressed.py
import sys,os
from PIL import Image

def FlipX(o):
    ii = 0
    f = []
    while ii < len(o):
        x = 7
        while x >= 0:
            f.append(o[ii+x])
            x -= 1
        ii += 8
    return f

def FlipY(o):
    ii = 0
    f = []
    y = 7
    while y >= 0:
        x = 0
        while x < 8:
            f.append(o[(y*8)+x])
            x += 1
        y -= 1
    return f 

# open image 
f = Image.open(sys.argv[1])
px = f.load()
bgmode = False
if (type(px[0,0]) == tuple):
    print('not indexed image. quitting...')
    sys.exit()
# BG MODE ONLY!
bgmode = True
print('using left-to-right (background) mode...')
imsize = f.size
f.close()
outcstr = ''
padd = False
bn = os.path.basename(sys.argv[1])
bn = bn.split('.')[0]
outtiles = []
tile = 0

# create initial tile array (ref png2gen.py)
h = 0
while h < imsize[1]:
    w = 0
    while w < imsize[0]:
        ottl = []
        y = 0
        while y < 8:
            x = 0
            while x < 8:
                if((h+y) < imsize[1]) and ((w+x) < imsize[0]):
                    ottl.append(hex(int(px[x+w,y+h]))[2:])
                else:
                    ottl.append(0)
                    padd = True
                x += 1
            y += 1
        outtiles.append(ottl)
        tile += 1
        w += 8
    h += 8

print(bn + ' image size:',imsize[0],imsize[1], 'total tiles',tile)
if(padd):
    print('warning: image size not multiple of 8, tiles padded!')

# make sure none are duplicate
i, ofs, afound, bfound, cfound, dfound = 0,0,0,0,0,0
tm = []
outfilter = []
while i < len(outtiles):
    j = 0
    found = False
    flipa = FlipX(outtiles[i])
    flipb = FlipY(outtiles[i])
    flipc = FlipX(FlipY(outtiles[i]))
    while j < len(outfilter):
        if(outfilter[j] == outtiles[i]):
            tm.append(j)
            ofs += 1
            found = True
            dfound += 1
            break
        elif(outfilter[j] == flipa):
            tm.append(hex(j | (1<<11)))
            ofs += 1
            found = True
            afound+=1
            break 
        elif(outfilter[j] == flipb):
            tm.append(hex(j | (1<<12)))
            ofs += 1
            found = True
            bfound+=1
            break
        elif(outfilter[j] == flipc):
            tm.append(hex(j | (1<<11) | (1<<12)))
            ofs += 1
            found = True
            cfound+=1
            break  
        j += 1
    if not found:
        tm.append(i-ofs)
        outfilter.append(outtiles[i])
    i += 1

print('compressing... reduced to ' + str(len(outfilter)) + ' tiles')
print('dupes:' + str(dfound) + ' xflips:' + str(afound) + ' yflips:' 
      + str(bfound) + ' xyflips:' + str(cfound))
outtiles = outfilter
outfilter = []

#create tilemap too.
i = 0
ttss = 'const u16 ' + bn + '_map[] = {\n\t'
while i < len(tm):
    ttss += str(tm[i]) + ', '
    if (i+1)%16 == 0:
        ttss += '\n\t'
    i += 1
ttss += '\n};\n'
f = open(bn + '_map.h', 'w')
f.write(ttss)
f.close()
print(bn + '_map.h written.')

tiles=outfilter
#now write the header file
i = 0
ts = ''
while i < len(outtiles):
    tilestr = 'const u32 ' + bn + '_' + str(i) + '[] = {'
    y = 0
    while y < 8:
        lwstr = '\n\t0x'
        x = 0
        while x < 8:
            lwstr = lwstr + str(outtiles[i][(y*8)+x])
            x += 1
        y += 1
        tilestr = tilestr + lwstr + ','
    ts += tilestr + '\n};\n'
    i += 1
f = open(bn + '.h', 'w')
f.write(ts)
f.close()
print(bn + '.h written.')
