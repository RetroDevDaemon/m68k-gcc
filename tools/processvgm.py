#!/usr/bin/python3
import sys
from os import listdir 
from os.path import isfile, join 
from struct import *

allpcm = []
allvgm=[]
lastid = 0

class pcmObject:
    def __init__(self, by):
        self.bytes = by
        self.samprate = 0
        self.globalid = 0
####

class vgmfile:
    def __init__(self, fn, by):
        self.filename = fn
        self.bytes = by 
        self.header = unpack(struct_vgm_hdr_fmt, self.bytes[:128])
        self.ident = chr(self.header[0])+chr(self.header[1])+chr(self.header[2])+chr(self.header[3])
        if(self.ident != 'Vgm '):
            print('INVALID VGM FILE!\n')
        self.eof = self.header[4]
        print('total size: ', self.eof+4)
        self.version = self.header[5]
        if self.version != 0x160:
            print('VGM NOT VERSION 1.60!\n')
        self.sn76489c = self.header[6]
        if(self.sn76489c != 0):
            print('SN7 detected.')
        self.ym2413c = self.header[7]
        if(self.ym2413c != 0):
            print('YM2413 detected.\n')
        self.gd3 = self.header[8]
        self.totalsamples = self.header[9]
        self.loopofs = self.header[10]
        if(self.loopofs != 0):
            print('Loop enabed at', hex(self.loopofs + 0x1c))
        self.loopsamps = self.header[11]
        self.rate = self.header[12]
        if(self.rate == 60):
            print('NTSC detected.')
        elif(self.rate == 50):
            print('PAL detected.')
        self.sn7feedback = self.header[13]
        self.sn7sr = self.header[14]
        self.sn7flags = self.header[15]
        self.ym2612c = self.header[16]
        if(self.ym2612c != 0):
            print('YM2612 detected.')
        self.ym2151c = self.header[17]
        self.vgmofs = self.header[18]
        #print('vgm start: ',self.vgmofs+0x34)
        if(self.vgmofs+0x34 != 0x80):
            print('weird header size!')
        self.segapcm = self.header[19]
        self.segapcmir = self.header[20]
        self.rf5c68c = self.header[21]
        self.ym2203c = self.header[22]
        self.ym2608c = self.header[23]
        self.ym2610c = self.header[24]
        self.ym3812c = self.header[25]
        self.ym3526c = self.header[26]
        self.y8950c = self.header[27]
        self.ymf262c = self.header[28]
        self.ymf278bc = self.header[29]
        self.rf5c164c = self.header[30]
        self.pwmc = self.header[31]
        self.ay8910c = self.header[32]
        self.aytype = self.header[33]
        self.ayflags = self.header[34]
        self.ym22ayflags = self.header[35]
        self.ym26ayflags = self.header[36]
        self.volumemod = self.header[37]
        #print(self.volumemod)
        self.reserved = self.header[38]
        self.loopbase = self.header[39]
        self.loopmodifier = self.header[40]
        self.songbytes = []
        self.pcms = []
        return 

    def split_vgm(self):
        global allvgm 
        i = 0
        while i < len(self.songbytes):
            allvgm.append(bytes([self.songbytes[i]]))
            i += 1

    def split_pcm(self):
        global lastid
        global allpcm
        i = 0
        pcms = []
        pcmsize = 0
        num_db = 0
        datstart = 0
        while i < len(self.bytes):
            # find each PCM block 
            if(self.bytes[i] == 0x67)and(self.bytes[i+1] == 0x66):
                print('data block found at ', i, end='')
                i += 2
                if(self.bytes[i] == 0):
                    print('\ttype: ym2612 pcm', self.bytes[i], end='')
                else:
                    print('warning - not ym2612 pcm!\n', end='')
                i += 1
                dbsize = self.bytes[i] + (self.bytes[i+1]<<8) + (self.bytes[i+2]<<16) + (self.bytes[i+3]<<24)
                print('\tsize',dbsize, end=' ')
                pcmsize += dbsize
                thispcm = []
                i_t = i + dbsize + 4 - 1 # until end of pcm
                while i < i_t:
                    thispcm.append(self.bytes[i])
                    i += 1
                thispcm = pcmObject(thispcm)
                thispcm.globalid = lastid 
                thispcm.src_id = num_db 
                thispcm.src_song = self.filename
                print('db no: ', num_db)
                lastid += 1
                num_db += 1
                self.pcms.append(thispcm) 
                #   0x90 ss < ignore
                #   0x94 ss < ignore
                # look for [0x91][SS][self.src_id], and set self.streamid to SS
                # also look for [0x92] [self.streamid] [f][r][e][q] and set it on the pcm object ONCE! after finding 0x91
                # (self.pcms[i].streamid) needs to change to (self.pcms[i].id) within the song data: 
                #   change 0x93 [streamid] to 0x93 [globalid] < probably not used
                #   change 0x95 [ss] [bb bb] to: 0x95 [streamid] [global][id]
                allpcm.append(thispcm)
                datstart = i + 1
            # if a Gd3 block is found, skip it 
            #if(self.bytes[i] == 0x47):
            #    if(self.bytes[i+1] == 0x64):
            #        if(self.bytes[i+2] == 0x33):
            #            #+3 is " "
                        #+4,5,6,7 is 0.1.0.0
                        #+8, +9 (LE) is size of gd3 tag
            #            gdsize = self.bytes[i+8] + (self.bytes[i+9]<<8)
            #            i += 9 + gdsize 
            i += 1
        # register song data start position
        if datstart == 0:
            datstart = self.vgmofs+0x34 
        print('music data start @ ', hex(datstart))
        self.musicstart = datstart 
        # copy only VGM data into songbytes up until Gd3 tag
        i = self.musicstart 
        while i < len(self.bytes):
            if(self.bytes[i] == 0x47):
                if(self.bytes[i+1] == 0x64):
                    if(self.bytes[i+2] == 0x33):
                        print("Gd3 found, skipping ... ")
                        #+3 is " "
                        #+4,5,6,7 is 0.1.0.0
                        #+8, +9 (LE) is size of gd3 tag
                        gdsize = self.bytes[i+8] + (self.bytes[i+9]<<8)
                        i += 9 + gdsize 
            self.songbytes.append(self.bytes[i])
            i += 1
        print('total pcm size: ', pcmsize, '(' + str(int(pcmsize/1024)) + 'kB)', '[' + str(int(pcmsize/len(self.bytes)*100)) + '% total filesize]')
        print('vgm data only size: ', len(self.songbytes))
####


struct_vgm_hdr_fmt = 'BBBBIII'+\
                     'IIII'+\
                     'IIHBBI'+\
                     'IIII'+\
                     'IIII'+\
                     'IIII'+\
                     'IIII'+\
                     'IIBBBBBBBB'

if(len(sys.argv) == 2):
    print("Opening folder: " + sys.argv[1])
else:
    print('Argument expects a folder or subfolder.\n')
fn = sys.argv[1]
l = listdir(fn)
vgms = []
for k in l:
    e = k.split('.')
    e = e[len(e)-1]
    if(e.upper() == 'VGM'):
        vgms.append(k)
print(str(len(vgms)) + ' VGMs found.\n')#, vgms)


vgfiles = []

for vg in vgms:
    f = open(fn+vg, 'rb')
    vgby = f.read()
    f.close()
    
    this = vgmfile(vg, vgby)
    print('\n',vg)
    this.split_pcm()
    this.split_vgm()
    vgfiles.append(this)

dupes = 0

unique_pcm = []

i = 0
while i < len(allpcm):
    j = 0
    new = True 
    while j < len(unique_pcm):
        if(allpcm[i].bytes == unique_pcm[j].bytes):
            new = False
        j += 1
    if(new == True):
        unique_pcm.append(allpcm[i])
    i += 1

print('unique pcm samples: ', len(unique_pcm), '(duplicates: ' + str(len(allpcm)-len(unique_pcm))+')')

tsize = 0
i = 0
while i < len(unique_pcm):
    tsize += len(unique_pcm[i].bytes)
    print(unique_pcm[i].src_song, unique_pcm[i].src_id)
    i += 1
_otsize = 0
i = 0
while i < len(allpcm):
    _otsize += len(allpcm[i].bytes)
    i += 1

print('total sample size: ', tsize, '(' + str(int(tsize/1024))+'kB)')
print('(size saved: ', str(int((_otsize-tsize)/1024)) + 'kB)')

print("writing PCM block to pcmdata.bin... ", end =" ")

pcmbytes = []
# bytes 0-1 total number of samples
# bytes n...n+3 offset from pcmbytes+2 to sample bytes
# Disabled:
#pcmbytes.append(bytes([len(unique_pcm) >> 8]))
#pcmbytes.append(bytes([len(unique_pcm) & 0xff]))
# first entry is always 256 (4 bytes per vec * 64 samples)
pcmbytes.append(bytes([0]))
pcmbytes.append(bytes([0]))
pcmbytes.append(bytes([1]))
pcmbytes.append(bytes([0]))
# append pcm data offsets
_pbofs = 256
i = 0
while i < len(unique_pcm):
    pcmbytes.append(bytes([(_pbofs + len(unique_pcm[i].bytes) & 0xff000000) >> 24]))
    pcmbytes.append(bytes([(_pbofs + len(unique_pcm[i].bytes) & 0xff0000) >> 16]))
    pcmbytes.append(bytes([(_pbofs + len(unique_pcm[i].bytes) & 0xff00) >> 8]))
    pcmbytes.append(bytes([(_pbofs + len(unique_pcm[i].bytes) & 0xff)]))
    _pbofs += len(unique_pcm[i].bytes) 
    i += 1
while(len(pcmbytes) < 256):
    pcmbytes.append(bytes([0]))
# append pcm data itself
i = 0
while i < len(unique_pcm):
    j = 0
    while j < len(unique_pcm[i].bytes):
        pcmbytes.append(bytes([unique_pcm[i].bytes[j]]))
        j += 1
    i += 1
# write data to file
f = open('pcmdata.bin', "wb")
i = 0
while i < len(pcmbytes):
    f.write(pcmbytes[i])
    i += 1
f.close()

print("Done.")
print("writing song bytes to vgmdata.bin ... ")

# first entry is always 0x100
vgmbytes = []
vgmbytes.append(bytes([0]))
vgmbytes.append(bytes([0]))
vgmbytes.append(bytes([1]))
vgmbytes.append(bytes([0]))
_vofs = 256
# append offsets
i = 0
while i < len(vgfiles):
    vgmbytes.append(bytes([(_vofs + len(vgfiles[i].songbytes) & 0xff000000) >> 24]))
    vgmbytes.append(bytes([(_vofs + len(vgfiles[i].songbytes) & 0xff000) >> 16]))
    vgmbytes.append(bytes([(_vofs + len(vgfiles[i].songbytes) & 0xff00) >> 8]))
    vgmbytes.append(bytes([(_vofs + len(vgfiles[i].songbytes) & 0xff)]))
    _vofs += len(vgfiles[i].songbytes)
    i += 1
# pad to 256 bytes
while (len(vgmbytes) < 256):
    vgmbytes.append(bytes([0]))
# write trimmed song bytes 
i = 0
while i < len(vgfiles):
    j = 0
    while j < len(vgfiles[i].songbytes):
        vgmbytes.append(bytes([vgfiles[i].songbytes[j]]))
        j += 1
    i += 1
# write file 
f = open('vgmdata.bin', "wb")
i = 0
while i < len(vgmbytes):
    f.write(vgmbytes[i])
    i += 1
f.close()

print("Done.")