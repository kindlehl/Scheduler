ALL_OBJECTS=$(wildcard *.o)
NAME=scheduler
DAEMON_NAME=scheduled
CC=g++ -g -std=c++17
CXX_FLAGS= -pedantic -Wall -fpermissive -Wno-reorder


all: daemon client

daemon: daemon.o menuitem.o logger.o
	$(CC) $(CXX_FLAGS) daemon.o logger.o menuitem.o -o $(DAEMON_NAME) -lncurses 
daemon.o: src/daemon.cpp include/daemon.h 
	$(CC) $(CXX_FLAGS) -c src/daemon.cpp -o daemon.o

client: main

main: src/main.cpp menuitem.o menu.o logger.o
	$(CC) $(CXX_FLAGS) src/main.cpp menuitem.o menu.o logger.o -I/usr/include -lncurses -o $(NAME)
menuitem.o: include/menuitem.h src/menuitem.cpp 
	$(CC) $(CXX_FLAGS) -c src/menuitem.cpp
menu.o: include/menu.h src/menu.cpp menuitem.o
	$(CC) $(CXX_FLAGS) -c -I/usr/include src/menu.cpp
logger.o: include/logger.h src/logger.cpp
	$(CC) $(CXX_FLAGS) -c -I/usr/include src/logger.cpp 

clean: 
	rm -f *.o $(NAME)
