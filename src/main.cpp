#include <iostream>
#include "../include/menu.h"
#include <curses.h>
#include <signal.h>
#include <functional>

using namespace std;

void sigintHandler(int);
//points to menu that will be written to file


int main(){
	initscr();	
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	string home = getenv("HOME");
	Menu mainMenu(home + "/.schedule");
	//signal(SIGINT, sigintHandler);
	while(mainMenu)
		mainMenu.update();
	endwin();
	return 0;
}
//when sigint is received, properly exit the menu;
void sigintHandler(int sig){
	Menu::run = false;
}

