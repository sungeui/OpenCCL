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
LD = $(CC) $(LDOPTIONS) -L. -L..






INCLUDES = -I. -I../include

CFLAGS = $(COPTIONS) $(OPTFLAGS) $(INCLUDES)
LD = $(CC) -L. 


OBJS = LayoutGraph.o LayoutMetrics.o

.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp

libOpenCCL.a: $(OBJS)
	$(AR) $@ $(OBJS)

clean:
	rm -f *.o libOpenCCL.a

realclean:
	rm -f *.o libOpenCCL.a
