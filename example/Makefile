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

LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
	LIBS = -lOpenCCL -lmetis_64bit -lm
	LD = $(CC) $(LDOPTIONS) -L. -L../Lib -L../libsrc/ -Lmetis
	METISLIB = ../Lib/libOpenCCL.a ../Lib/libmetis_64bit.a
else
	LIBS = -lOpenCCL -lmetis_32bit -lm
	LD = $(CC) $(LDOPTIONS) -L. -L../Lib -L../libsrc/ -Lmetis
	METISLIB = ../Lib/libOpenCCL.a ../Lib/libmetis_32bit.a
endif

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
