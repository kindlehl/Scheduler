ALL_OBJECTS=$(wildcard *.o)
NAME=Scheduler
CC=g++ -g -std=c++17
CXX_FLAGS= -pedantic -Wall -fpermissive


all: menuitem menu main
	$(CC) $(CXX_FLAGS) menuitem.o menu.o main.o -lncurses -o  $(NAME) 
debug: menuitem menu main
	$(CC) $(CXX_FLAGS) -D DEBUG menuitem.o menu.o main.o -lncurses -o  $(NAME) 
main: src/main.cpp menuitem menu
	$(CC)  $(CXX_FLAGS) -c -I/usr/include src/main.cpp -lncurses -lmenu.o
menuitem: include/menuitem.h src/menuitem.cpp 
	$(CC)  $(CXX_FLAGS) -c src/menuitem.cpp
menu: include/menu.h src/menu.cpp menuitem
	$(CC)  $(CXX_FLAGS) -c -I/usr/include src/menu.cpp -lncurses -lmenuitem.o
clean: 
	rm -f *.o $(NAME)
