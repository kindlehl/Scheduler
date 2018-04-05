#include <iostream>
#include "../include/menu.h"
#include <curses.h>
using namespace std;

int main(){
	initscr();	
	noecho();
	keypad(stdscr, TRUE);
	Menu mainMenu("/home/hunter/.schedule");
//	for(int i = 0; i < 10; i++)	
//		mainMenu.add(MenuItem((i*12947)%10, "First Event of the Program", "This is a testing event"));
	while(mainMenu)
		mainMenu.update();
	endwin();
	return 0;
}
