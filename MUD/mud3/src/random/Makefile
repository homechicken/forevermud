#Makefile for random subdirectory

# Uncomment to use GNU g++ compiler
CXX = g++

# Uncomment to use Intel's C++ compiler
#CXX = icpc -wd981

INCLUDE = -I.. -I../../conf -I../log -I../../3rdparty/boost/include

CXXFLAGS = -Wall -g -O2 -ansi -pthread

.PHONY: clean permissions

OBJ = random.o

all: libMUDrandom.a

libMUDrandom.a: $(OBJ)
	ar -crs $@ $(OBJ) && mv $@ ../../lib

test: rngtest.o random.o
	$(CXX) $(CXXFLAGS) -o $@ rngtest.o random.o

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

clean:
	@rm -f *.o *.a *.*~ test ../../lib/libMUDrandom.a

permissions:
	@chmod 644 *.cpp *.h Makefile
