# Makefile 
# CSCI 480
# Assignment 3

# we assume the pic directory locates one level above,
# change PIC_PATH if this is not the case
PIC_PATH = $(abspath $(CURDIR)/../Eigen)

INCLUDE = -I$(PIC_PATH)
LIBRARIES = -L$(PIC_PATH)


COMPILER = g++
COMPILERFLAGS = -O3 $(INCLUDE)

PROGRAM = eigen
SOURCE = eigen.cpp
OBJECT = eigen.o

.cpp.o: 
	$(COMPILER) -c $(COMPILERFLAGS) $<

all: $(PROGRAM)

$(PROGRAM): $(OBJECT)
	$(COMPILER) $(COMPILERFLAGS) -o $(PROGRAM) $(OBJECT) 

clean:
	-rm -rf core *.o *~ "#"*"#" $(PROGRAM)