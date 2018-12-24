ALL_OBJECTS=$(wildcard *.o)
NAME=scheduler
DAEMON_NAME=scheduled
CC=g++ -g -std=c++17
CXX_FLAGS= -pedantic -Wall -fpermissive -Wno-reorder


all: menuitem.o menu.o main.o logger.o
	$(CC) $(CXX_FLAGS) menuitem.o menu.o main.o logger.o -lncurses -o  $(NAME) 
debug.o: menuitem.o menu.o main.o
	$(CC) $(CXX_FLAGS) -D DEBUG menuitem.o menu.o main.o -lncurses 
daemon: daemon.o menuitem.o user.o
	$(CC) $(CXX_FLAGS) daemon.o menuitem.o user.o -o $(DAEMON_NAME) -lncurses 
daemon.o: src/daemon.cpp include/daemon.h 
	$(CC) $(CXX_FLAGS) -c src/daemon.cpp -o daemon.o 
user.o: src/user.cpp include/user.h 
	$(CC) $(CXX_FLAGS) -c src/user.cpp -o user.o
main.o: src/main.cpp menuitem.o menu.o
	$(CC)  $(CXX_FLAGS) -c -I/usr/include src/main.cpp
menuitem.o: include/menuitem.h src/menuitem.cpp 
	$(CC) $(CXX_FLAGS) -c src/menuitem.cpp
menu.o: include/menu.h src/menu.cpp menuitem.o
	$(CC) $(CXX_FLAGS) -c -I/usr/include src/menu.cpp
logger.o: include/logger.h src/logger.cpp
	$(CC) $(CXX_FLAGS) -c -I/usr/include src/logger.cpp 

clean: 
	rm -f *.o $(NAME)
