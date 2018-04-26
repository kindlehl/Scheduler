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
		MenuItem& operator=(const MenuItem &m);
		MenuItem(std::string m_description, std::string m_name,std::string date,  std::time_t eventTime);
		explicit MenuItem(std::istream& , char delim = '|');

		bool operator<(const MenuItem& m) const;

		bool active() const;
		void activate();

		std::string description();
		void setDescription(std::string);

		std::string name();
		void setName(std::string);

		std::string datestring();
		void setDatestring(std::string);

		std::time_t timeRemaining() const;
		std::time_t time();
		void setTime(std::time_t);

		int ID();
	private:
		int m_id;
		bool m_selected;
		static int numMenus;
		std::string m_datestring;
		std::string m_description, m_name;
		std::time_t m_time;	
};
#endif 
