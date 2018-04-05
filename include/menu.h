#ifndef MENU_H
#define MENU_H
#include "menuitem.h"
#include <iostream>
#include <string>
#include <vector>
#include <curses.h>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <cstdlib>

class Menu{
	public:
		Menu();
		Menu(const Menu& m) = delete;			
		bool operator<(const Menu& m);
		operator bool() const;	
		void add(MenuItem m);
		void up();
		void down();
		//void select();
		void update();
		void exit();	
	private:
		bool run = true;
		void loadMenuFromFile(std::string path);
		void print(std::string q, int drawFlags = A_NORMAL);
		void display();
		int selectIndex = 0;
		std::vector<MenuItem> menu_items;
		void printMenu();	
};


#endif
