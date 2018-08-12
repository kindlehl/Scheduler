ALL_OBJECTS=$(wildcard *.o)
NAME=Scheduler
CC=g++ -g -std=c++17
CXX_FLAGS= -pedantic -Wall -fpermissive


all: menuitem.o menu.o main.o
	$(CC) $(CXX_FLAGS) menuitem.o menu.o main.o -lncurses -o  $(NAME) 
debug.o: menuitem.o menu.o main.o
	$(CC) $(CXX_FLAGS) -D DEBUG menuitem.o menu.o main.o -lncurses -o 
main.o: src/main.cpp menuitem.o menu.o
	$(CC)  $(CXX_FLAGS) -c -I/usr/include src/main.cpp -lncurses -lmenu.o
menuitem.o: include/menuitem.h src/menuitem.cpp 
	$(CC)  $(CXX_FLAGS) -c src/menuitem.cpp
menu.o: include/menu.h src/menu.cpp menuitem.o
	$(CC)  $(CXX_FLAGS) -c -I/usr/include src/menu.cpp -lncurses -lmenuitem.o
clean: 
	rm -f *.o $(NAME)
