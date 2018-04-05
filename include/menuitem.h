#ifndef MENUITEM_H
#define MENUITEM_H

#define MIN_PRIORITY 1
#define MAX_PRIORITY 5
#include <string>
#include <iostream>
#include <limits>

class MenuItem{
		
	public:
		MenuItem();
		MenuItem(int m_priority, std::string m_description, std::string m_name);
		explicit MenuItem(std::istream& , char delim = '|');
		MenuItem(const MenuItem &m);	
		MenuItem& operator=(const MenuItem &m);
		bool operator<(const MenuItem& m);
		std::string name();
		std::string description();
		int priority() const;
		bool active() const;
	private:
		bool selected;
		int m_priority;
		std::string m_description, m_name;

};

#endif 
