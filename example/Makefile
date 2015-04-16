# Which compiler to use
CC = g++

# What optimization level to use
OPTFLAGS = -O2 

# What options to be used by the compiler
COPTIONS = 

# What options to be used by the loader
LDOPTIONS = 

# What archiving to use
AR = ar rv






INCLUDES = -I../include
CFLAGS = $(COPTIONS) $(OPTFLAGS) $(INCLUDES)

LIBS = -lOpenCCL -lmetis -lm
LD = $(CC) $(LDOPTIONS) -L. -L../Lib -Llibsrc/ -Lmetis
METISLIB = ../Lib/libOpenCCL.a ../Lib/libmetis.a



MTESTOBJS = Demo.o 


.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp

default: Demo

Demo: $(MTESTOBJS) $(METISLIB)
	$(LD)  -o $@ $(MTESTOBJS) $(LIBS)
	chmod 755 $@

clean:
	rm -f *.o Demo

realclean:
	rm -f *.o ;\
        rm -f Demo 
