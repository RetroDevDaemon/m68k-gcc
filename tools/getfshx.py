#use python3 
import os
st = hex(os.path.getsize('out/header.bin'))
if (st[len(st)-1:] != 'L'):
    print(st)
else:
    print(st[:-1])