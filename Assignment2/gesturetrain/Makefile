CFLAGS=-g -I.
CC = gcc
LFLAGS=

gesturetrain: gesturetrain.o imagenet.o backprop_initr.o pgmimage.o
	$(CC) ${LFLAGS} gesturetrain.o imagenet.o backprop_initr.o pgmimage.o \
	      -o gesturetrain -lm

hidtopgm: hidtopgm.o pgmimage.o backprop_initr.o
	$(CC) ${LFLAGS} hidtopgm.o pgmimage.o backprop_initr.o \
	      -o hidtopgm -lm

outtopgm: outtopgm.o pgmimage.o backprop_initr.o
	$(CC) ${LFLAGS} outtopgm.o pgmimage.o backprop_initr.o \
	      -o outtopgm -lm

gesturetrain_init0: gesturetrain.o imagenet.o backprop_init0.o pgmimage.o
	$(CC) ${LFLAGS} gesturetrain.o imagenet.o backprop_init0.o pgmimage.o \
	      -o gesturetrain_init0 -lm

backprop_initr.o: backprop.c backprop.h
	$(CC) ${CFLAGS} -c backprop.c
	mv backprop.o backprop_initr.o

backprop_init0.o: backprop.c backprop.h
	$(CC) ${CFLAGS} -c -DINITZERO backprop.c
	mv backprop.o backprop_init0.o

rungesturetrain:
	rm -f gesture.net
	gesturetrain -n gesture.net -t downgesture_train.list -1 downgesture_test1.list -2 downgesture_test2.list -e 100

testgesturetrain:
	make gesturetrain
	make rungesturetrain
