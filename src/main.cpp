#include <iostream>
#include <curses.h>
#include <signal.h>
#include <functional>
#include <rapidxml/rapidxml.hpp>
#include <sys/stat.h>

#include "../include/menu.h"
#include "../include/logger.h"

using namespace std;

string homedir;
string conf_path;
char* config_text;

rapidxml::xml_document<> config_xml;


void sigintHandler(int);
//points to menu that will be written to file


int main(){
	//initialize global paths
	homedir = string(getenv("HOME"));
    conf_path = homedir + "/.schedule";

	struct stat file_info;

	if(stat(conf_path.c_str(), &file_info)){ //true if config file does not exist
		ofstream configFile(conf_path.c_str(), ios::out);
		//create bare-bones XML file
		configFile << "<items>\n</items>";
		configFile.close();
	}

	initscr();	
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	Menu mainMenu(conf_path);

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

