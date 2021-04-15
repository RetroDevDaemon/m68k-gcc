CC=gcc 
LDFLAGS=-lm -lc
XGMTOOL_FILES =	psg.o \
				gd3.o \
				samplebank.o \
				util.o \
				vgm.o \
				vgmcom.o \
				xgc.o \
				xgccom.o \
				xgm.o \
				xgmcom.o \
				xgmsmp.o \
				xgmtool.o \
				ym2612.o 

%.o: 
	gcc -c src/*.c

default: ${XGMTOOL_FILES}
	gcc ${LDFLAGS} -o xgmtool ${XGMTOOL_FILES} 
	rm -rf *.o
