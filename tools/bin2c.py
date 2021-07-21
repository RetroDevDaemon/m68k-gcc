# bin2c.py
#  convert any binary file to C header
import sys,os 
f = open(sys.argv[1], 'rb')
inby = f.read()
f.close()
bn = os.path.basename(sys.argv[1]).split('.')[0]
ostr = 'const unsigned char ' + bn + '[] = {\n\t'
i = 0 
#while i < len(inby)-3:
#    a = inby[i]
#    b = inby[i+1] << 8
#    c = inby[i+2] << 16
#    d = inby[i+3] << 24
#    ostr += hex(a|b|c|d)
#    ostr += ', '
#    if((i+1)%16 == 0):
#        ostr += '\n\t'
#    i += 4
while i < len(inby):
    ostr += hex(inby[i]) + ', '
    if((i+1)%16 == 0):
        ostr += '\n\t'
    i += 1
ostr += '\n};\n'
#f = open(bn + '.h' , 'w')
#f.write(ostr)
#f.close()
print(ostr)