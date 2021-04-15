#!/usr/bin/python3
# padrom.py
import sys  
ALIGN = 131072
f = open(sys.argv[1], 'rb')
by = f.read()
f.close()
fsiz = len(by)
banks = 0
if(fsiz > ALIGN):
    banks = int(fsiz / ALIGN)
else:
    banks = 0
ofs = ALIGN - (fsiz % ALIGN) 
ofs = ofs + (banks * ALIGN)
f = open('out.md', 'wb')
f.write(by)
i = ofs 
while i > 0:
    f.write(bytes([0]))
    i -= 1
f.close()