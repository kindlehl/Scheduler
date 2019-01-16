#include <iostream>
#include <curses.h>
#include <signal.h>
#include <functional>
#include <rapidxml/rapidxml.hpp>
#include <sys/stat.h>

#include "../include/globals.h"
#include "../include/menu.h"
#include "../include/help.h"
#include "../include/logger.h"

using namespace std;

char* config_text;

rapidxml::xml_document<> config_xml;


void sigintHandler(int);
//points to menu that will be written to file


int main(){
	//initialize configuration folder
	setupFolderStructure();

	initscr();	
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	Menu mainMenu(conf_path);

	//signal(SIGINT, sigintHandler);
	while (mainMenu) {
		mainMenu.update();
	}

	endwin();
	return 0;
}

//when sigint is received, properly exit the menu;
void sigintHandler(int sig) {
	Menu::run = false;
}

