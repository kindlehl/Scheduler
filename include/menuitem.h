#ifndef MENUITEM_H
#define MENUITEM_H

#define MIN_PRIORITY 1
#define MAX_PRIORITY 5
#include <string>
#include <iostream>
#include <limits>

class MenuItem{
		
	public:
		explicit MenuItem(std::istream& , char delim = '|');
		MenuItem();
		MenuItem(int m_priority, std::string m_description, std::string m_name);
		MenuItem(const MenuItem &m);	
		MenuItem& operator=(const MenuItem &m);
		bool active() const;
		bool operator<(const MenuItem& m);
		int priority() const;
		std::string name();
		std::string description();
		int ID;
	private:
		static int numMenus;
		
		int m_priority;
		bool selected;
		std::string m_description, m_name;
		
};

#endif 
