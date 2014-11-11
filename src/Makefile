# Makefile to build ectest

all: ectest 

ETHER_LOC=/opt/etherlab

CC=g++ 
CFLAGS=-c -Wall -std=c++0x -pthread -D_GLIBCXX_USE_NANOSLEEP
INCS= -I$(ETHER_LOC)/include -I../include -I$(ETHER_LOC)/tool
LOPTS=-Wl,-rpath=$(ETHER_LOC)/lib
LIBS=-L$(ETHER_LOC)/lib -lethercat -lpthread

ectest: ServerSimulation.o EtherCatServer.o ConfigLoader.o CommandQueue.o PdoEntryCache.o NameMap.o CyclicMotor.o tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o 
	$(CC) $(LOPTS) $(LIBS) -o $@ $^

ServerSimulation.o: ServerSimulation.cpp
	$(CC) $(CFLAGS) $(INCS) $<

EtherCatServer.o: EtherCatServer.cpp
	$(CC) $(CFLAGS) $(INCS) $<

ConfigLoader.o: ConfigLoader.cpp
	$(CC) $(CFLAGS) $(INCS) $<

CommandQueue.o: CommandQueue.cpp
	$(CC) $(CFLAGS) $(INCS) $<

PdoEntryCache.o: PdoEntryCache.cpp
	$(CC) $(CFLAGS) $(INCS) $<

NameMap.o: NameMap.cpp
	$(CC) $(CFLAGS) $(INCS) $<

CyclicMotor.o: CyclicMotor.cpp
	$(CC) $(CFLAGS) $(INCS) $<

tinystr.o: tinystr.cpp
	$(CC) $(CFLAGS) $(INCS) $<

tinyxml.o: tinyxml.cpp
	$(CC) $(CFLAGS) $(INCS) $<

tinyxmlerror.o: tinyxmlerror.cpp
	$(CC) $(CFLAGS) $(INCS) $<

tinyxmlparser.o: tinyxmlparser.cpp
	$(CC) $(CFLAGS) $(INCS) $<

clean:
	rm ectest.o ectest

touch:
	touch ectest.c



