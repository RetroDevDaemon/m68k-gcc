#use python3 
import os, sys
st = hex(os.path.getsize(sys.argv[1]))
if (st[len(st)-1:] != 'L'):
    print(st)
else:
    print(st[:-1])