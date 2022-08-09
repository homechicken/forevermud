#Makefile for MUD commands

# Uncomment to use the GNU g++ compiler
CXX = g++

# Uncomment to use Intel's C++ compiler
#CXX = icpc -wd1419 -wd869
# Remark 1419 is about external declarations in .cpp files (re extern Global glob) and only appears because of -Wall
# Remark 869 is about unused function parameters and can be safely ignored

INCLUDE = -I. -I.. -I../../conf -I../log -I../events -I../random -I../../3rdparty/boost/include

CXXFLAGS = -Wall -g -ansi -O2 -pthread

OBJ =	command.o say.o quit.o uptime.o when.o stats.o idle.o who.o help.o ban.o \
		config.o save.o channel.o tell.o emote.o bsotg.o social.o socialData.o \
		look.o read.o alias.o test.o description.o status.o map.o create.o get.o \
		shutdown.o

.PHONY: clean permissions

all: libMUDcommands.a

libMUDcommands.a : $(OBJ)
	ar -crs $@ $(OBJ) && mv $@ ../../lib

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

clean:
	@rm -f *.o *.*~ ../../lib/libMUDcommands.a

permissions:
	@chmod 644 *.cpp *.h Makefile
