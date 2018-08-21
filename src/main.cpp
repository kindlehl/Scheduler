#include <iostream>
#include "../include/menu.h"
#include "../include/logger.h"
#include <curses.h>
#include <signal.h>
#include <functional>
#include <rapidxml/rapidxml.hpp>
#include <sys/stat.h>


char* HOME;
char* CONF_PATH;
char* config_text;
rapidxml::xml_document<> config_xml;

using namespace std;

void sigintHandler(int);
//points to menu that will be written to file


int main(){
	HOME = getenv("HOME");
    CONF_PATH = (char*)malloc(strlen(HOME) + strlen("/.schedule") + 1);
	CONF_PATH[0]='\0';
	initLog();

	strcat(CONF_PATH, HOME);
	strcat(CONF_PATH, "/.schedule");
	struct stat file_info;
	if(stat(CONF_PATH, &file_info)){//true if config file does not exist
		ofstream configFile(CONF_PATH, ios::out);
		configFile << "<items>\n</items>";
		configFile.close();
	}
	initscr();	
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	Menu mainMenu(CONF_PATH);
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

