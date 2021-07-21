import sys 

f = open(sys.argv[1], 'rb')
ib = f.read() 
f.close() 

f = open(sys.argv[1] + 'trimmed', 'wb')
i = int(sys.argv[2])
while i < len(ib):
        f.write(bytes([ib[i]])) 
        i += 1
f.close() 
print('file trimmed ' + sys.argv[2] + ' bytes off of start.')