#include <iostream>
#include "../include/menu.h"
#include <curses.h>
using namespace std;

int main(){
	initscr();	
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	string home = getenv("HOME");
	Menu mainMenu(home + "/.schedule");
	while(mainMenu)
		mainMenu.update();
	endwin();
	return 0;
}
