#ifndef MENUITEM_H
#define MENUITEM_H


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
		bool operator<(const MenuItem& m) const;
		MenuItem(std::string m_description, std::string m_name,std::string date,  std::time_t eventTime, std::time_t completionTime);
		explicit MenuItem(std::istream& , char delim = '|');

		bool active() const;
		void activate();

		std::string description() const;
		void setDescription(std::string);

		std::string name() const;
		void setName(std::string);

		std::string datestring() const;
		void setDatestring(std::string);

		std::time_t timeToComplete() const;
		std::time_t setTimeToComplete(std::time_t);

		std::time_t timeRemaining() const;
		std::time_t time() const;
		void setTime(std::time_t);

		int ID() const;
	private:
		int m_id;
		bool m_selected;
		static int numMenus;
		std::string m_datestring;
		std::string m_description, m_name;
		std::time_t m_time_due, m_time_completion;	
};

#endif 
