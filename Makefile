ALL_OBJECTS=$(wildcard *.o)
NAME=Scheduler
CC=g++ -std=c++14



all: menuitem menu main
	$(CC) menuitem.o menu.o main.o -lncurses -o  $(NAME) 
main: src/main.cpp menuitem menu
	$(CC) -c -I/usr/include src/main.cpp -lncurses
menuitem: include/menuitem.h src/menuitem.cpp 
	$(CC) -c src/menuitem.cpp
menu: include/menu.h src/menu.cpp menuitem
	$(CC) -c -I/usr/include src/menu.cpp -lncurses
clean: 
	rm -f *.o $(NAME)
