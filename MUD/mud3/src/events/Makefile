# Makefile for MUD events

# Uncomment to use the GNU g++ compiler
CXX = g++

# Uncomment to use Intel's C++ compiler
#CXX = icpc -wd1419 -wd869
#Remark 1419 is about external declarations in .cpp files, which show up when -Wall is on (re extern Global glob)
#Remark #869 is about a function parameter that goes unused

INCLUDE = -I. -I.. -I../../conf -I../log -I../random -I../commands  -I../../3rdparty/boost/include

CXXFLAGS = -Wall -g -O2 -ansi -pthread

OBJ =	eBsotg.o

.PHONY: clean permissions

all: libMUDevents.a

libMUDevents.a: $(OBJ)
	ar -crs $@ $(OBJ) && mv $@ ../../lib

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

clean:
	@rm -f *.o *.*~ ../../lib/libMUDevents.a

permissions:
	@chmod 644 *.cpp *.h Makefile
