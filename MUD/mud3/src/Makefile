# MUD3 Project Makefile

# Uncomment to use the GNU g++ compiler
CXX = g++

# Uncomment to use Intel's compiler
#CXX = icpc -wd1419 -wd869 -wd981 -wd383
# Remark 383 causes log("my temp string") to error because it's a reference to a temporary object

CXXFLAGS = -g -O2 -ansi -Wall -pthread

INCLUDE = -I. -I../conf -I./log -I./commands -I./events -I./random -I../3rdparty/boost/include

DEFINE =

LINK = -L. -L../lib -L../3rdparty/boost/lib -lMUDcommands -lMUDlog -lMUDevents -lMUDrandom -lmysqlclient -lboost_regex -lboost_system -lboost_filesystem -lyaml-cpp

# top-level object files
TLOBJS =	socket.o socketDriver.o thread_functions.o client_socket.o main.o \
			commandHandler.o loadCommands.o banMap.o container.o living.o \
			sentient.o player.o playerDatabase.o messageDaemon.o chatChannel.o event.o \
			eventDaemon.o MySQL_Server.o Query.o mudsql.o fileio.o io.o message.o utility.o \
			room.o physical.o wearable.o readable.o milestone.o exit.o \
			connection.o objectFactory.o zoneMap.o global.o zone.o zoneDaemon.o \
			connStateClosed.o connStateLogin.o connStatePassword.o connStateCreate.o \
			connStatePlaying.o book.o runtimeConfig.o collection.o coin.o uuid.o

.PHONY: clean permissions

# what does 'all' do?
all:
	@rm -f mud
	make -C log && \
	make -C commands && \
	make -C events && \
	make -C random && \
	make mud

# program files
mud: $(TLOBJS)
	$(CXX) -o mud $(TLOBJS) $(LINK)

# object files
socket.o: socket.h socket.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c socket.cpp

socketDriver.o: socketDriver.h socketDriver.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c socketDriver.cpp

thread_functions.o: thread_functions.h thread_functions.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c thread_functions.cpp

client_socket.o: client_socket.h client_socket.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c client_socket.cpp

main.o: main.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c main.cpp

commandHandler.o: commandHandler.h commandHandler.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c commandHandler.cpp

loadCommands.o: loadCommands.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c loadCommands.cpp

banMap.o: banMap.h banMap.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c banMap.cpp

playerDatabase.o: playerDatabase.h playerDatabase.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c playerDatabase.cpp

messageDaemon.o: messageDaemon.h messageDaemon.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c messageDaemon.cpp

chatChannel.o: chatChannel.h chatChannel.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c chatChannel.cpp

event.o: event.h event.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c event.cpp

eventDaemon.o: eventDaemon.h eventDaemon.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c eventDaemon.cpp

MySQL_Server.o: MySQL_Server.h MySQL_Server.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c MySQL_Server.cpp

Query.o: Query.h Query.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c Query.cpp

mudsql.o: mudsql.h mudsql.cpp MySQL_Server.h Query.h
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c mudsql.cpp

fileio.o: fileio.h fileio.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c fileio.cpp

io.o: io.h io.cpp fileio.h mudsql.h
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c io.cpp

message.o: message.h message.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c message.cpp

physical.o: physical.cpp physical.h
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c physical.cpp

container.o: container.h container.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c container.cpp

wearable.o: wearable.cpp wearable.h
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c wearable.cpp

living.o: living.h living.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c living.cpp

sentient.o: sentient.h sentient.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c sentient.cpp

player.o: player.h player.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c player.cpp

room.o: room.h room.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c room.cpp

readable.o: readable.h readable.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c readable.cpp

milestone.o: milestone.h milestone.cpp physical.h readable.h
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c milestone.cpp

exit.o: exit.h exit.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c exit.cpp

connection.o: connection.h connection.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c connection.cpp

objectFactory.o: objectFactory.h objectFactory.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c objectFactory.cpp

zoneMap.o: zoneMap.h zoneMap.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c zoneMap.cpp

global.o: global.h global.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c global.cpp

utility.o: utility.h utility.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c utility.cpp

zone.o: zone.h zone.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c zone.cpp

zoneDaemon.o: zoneDaemon.h zoneDaemon.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c zoneDaemon.cpp

connStateClosed.o: connStateClosed.h connStateClosed.cpp connectionState.h
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c connStateClosed.cpp

connStateLogin.o: connStateLogin.h connStateLogin.cpp connectionState.h
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c connStateLogin.cpp

connStateCreate.o: connStateCreate.h connStateCreate.cpp connectionState.h
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c connStateCreate.cpp

connStatePassword.o: connStatePassword.h connStatePassword.cpp connectionState.h
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c connStatePassword.cpp

connStatePlaying.o: connStatePlaying.h connStatePlaying.cpp connectionState.h
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c connStatePlaying.cpp

book.o: book.h book.cpp physical.h readable.h
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c book.cpp

runtimeConfig.o: runtimeConfig.h runtimeConfig.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c runtimeConfig.cpp

collection.o: collection.h collection.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c collection.cpp

coin.o: coin.h coin.cpp item.h collection.h
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c coin.cpp

uuid.o: uuid.h uuid.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c uuid.cpp


# cleanup
clean:
	@rm -f *.*~ *.o ../lib/* ../bin/* mud
	make -C log clean
	make -C commands clean
	make -C events clean
	make -C random clean

permissions:
	@chmod 644 *.cpp *.h Makefile
	make -C log permissions
	make -C commands permissions
	make -C events permissions
	make -C random permissions
	
