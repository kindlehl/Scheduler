#include <iostream>
#include "../include/menu.h"
#include <curses.h>
#include <signal.h>
#include <functional>
#include "../lib/rapidxml.hpp"

char* HOME;

char* config_text;
rapidxml::xml_document<> config_xml;

using namespace std;

void sigintHandler(int);
//points to menu that will be written to file


int main(){
	HOME = getenv("HOME");
	string configPath = string(HOME) + "/.schedule";
	initscr();	
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	Menu mainMenu(configPath);
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

