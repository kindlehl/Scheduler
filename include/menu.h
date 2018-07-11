#ifndef MENU_H
#define MENU_H

//UPDATE THIS EVERYTIME YOU ADD A SEPARATOR OR ANYTHING THAT TAKES UP A STATIC AMOUNT OF SPACE ON THE MENU. THIS INCLUDES A '|' OR A "  ". THIS NUMBER SHOULD BE THE NUMBER OF SPACES USED IN MENU ENTRIES
#define USED_SPACE (2+3) //parenthasized to prevent fuckery
//2 spaces at beginning, 3 pipes between items


//the following definitions define how much space the windows take, relative to the window. Ensure that the following definitions add to one, or a runtime assertion will break the program.
#define NAME_PORT .3f 
#define DESC_PORT .29f
#define DATE_PORT .21f
#define DEBUG_PORT .20f


//these macros give the number of cells in the terminal that each piece of the menu should take up when printing. This way ensures that the program scales with window resizes.
#define NAME_SPACING NAME_PORT*(width-USED_SPACE)
#define DESC_SPACING DESC_PORT*(width-USED_SPACE)
#define DATE_SPACING DATE_PORT*(width-USED_SPACE)
#define DEBUG_SPACING DEBUG_PORT*(width-USED_SPACE)

#include "menuitem.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <curses.h>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <set>
#include <regex>
#include <exception>

std::time_t createTime(std::string& s, std::string regexp);
void expandDateString(std::string& datestring);

class Menu{
	public:
		Menu() = delete;
		Menu(const Menu& m) = delete;	
		~Menu();
		explicit Menu(std::string path);
		bool operator<(const Menu& m);
		operator bool() const;	
		void up();
		void down();
		void viewMenuItem();
		void addItem();
		void remove();	
		void sort();
		void update();
		void exit(int sig);	
		static bool run;
	private:
		int width, height;
		bool printField(std::string field, unsigned int spaces, char delimiter = ' ');
		void display();
		unsigned int selectIndex = 0;
		std::vector<MenuItem> menu_items;
		void printMenu();	
		std::vector<int> itemsToExclude;
};



#endif
