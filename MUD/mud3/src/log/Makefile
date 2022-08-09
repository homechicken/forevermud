# mud/log Makefile

# Uncomment to use the GNU g++ compiler
CXX = g++

# Uncomment to use Intel's C++ compiler
#CXX = icpc -wd981

CXXFLAGS = -g -O2 -ansi -Wall -pthread

# modify these to point to your MySQL Libraries
INCLUDE = -I. -I.. -I../../conf -I../../3rdparty/boost/include

OBJ = log.o statEngine.o

.PHONY: clean permissions

# what does `make all` do?
all: libMUDlog.a

libMUDlog.a: $(OBJ)
	ar -crs $@ $(OBJ) && mv $@ ../../lib

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

clean:
	@rm -f *.o *.a *.*~

permissions:
	@chmod 644 *.cpp *.h Makefile
