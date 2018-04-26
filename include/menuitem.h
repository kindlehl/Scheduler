#ifndef MENUITEM_H
#define MENUITEM_H

#define MIN_PRIORITY 1
#define MAX_PRIORITY 5

#include <string>
#include <regex>
#include <iostream>
#include <limits>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <exception>

class MenuItem{
		
	public:
		MenuItem();
		MenuItem(const MenuItem &m);	
		MenuItem(std::string m_description, std::string m_name,std::string date,  std::time_t eventTime);
		MenuItem& operator=(const MenuItem &m);
		
		
		bool active() const;
		bool operator<(const MenuItem& m) const;
		explicit MenuItem(std::istream& , char delim = '|');
		int ID;
		std::string description();
		std::string name();
		std::time_t timeRemaining()const;
		std::time_t eventTime;		
		void setTime(std::time_t);
		std::string dateString;
	private:
		bool selected;
		static int numMenus;
		std::string m_description, m_name;
};
#endif 
