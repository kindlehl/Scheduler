ALL_OBJECTS=$(wildcard *.o)
CLIENT_NAME=scheduler
DAEMON_NAME=scheduled
CC=g++ 
CXX_FLAGS=-g -std=c++14 -pedantic -Wall -fpermissive -Wno-reorder


all: $(CLIENT_NAME) $(DAEMON_NAME) kill

$(DAEMON_NAME): daemon.o profile.o menuitem.o help.o logger.o
	$(CC) $(CXX_FLAGS) daemon.o profile.o menuitem.o help.o logger.o -o $(DAEMON_NAME) -lncurses 

daemon.o: src/daemon.cpp include/daemon.h menuitem.o profile.o include/globals.h
	$(CC) $(CXX_FLAGS) -c src/daemon.cpp
profile.o: src/profile.cpp include/profile.h include/daemon.h menuitem.o
	$(CC) $(CXX_FLAGS) -c src/profile.cpp

$(CLIENT_NAME): main.o
	$(CC) $(CXX_FLAGS) main.o menuitem.o menu.o help.o logger.o -lncurses -o $(CLIENT_NAME)

main.o: src/main.cpp help.o menuitem.o menu.o logger.o help.o include/globals.h
	$(CC) $(CXX_FLAGS) -c src/main.cpp 
menuitem.o: include/menuitem.h src/menuitem.cpp 
	$(CC) $(CXX_FLAGS) -c src/menuitem.cpp
menu.o: include/menu.h src/menu.cpp menuitem.o
	$(CC) $(CXX_FLAGS) -c -I/usr/include src/menu.cpp
	
help.o: include/help.h src/help.cpp logger.o menuitem.o
	$(CC) $(CXX_FLAGS) -c src/help.cpp
logger.o: include/logger.h src/logger.cpp
	$(CC) $(CXX_FLAGS) -c -I/usr/include src/logger.cpp 

kill:
	killall scheduled scheduler 3>&2 2>&1 >/dev/null || echo "Killed old processes"
clean: 
	rm -f *.o $(CLIENT_NAME) $(DAEMON_NAME)
