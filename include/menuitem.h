#ifndef MENUITEM_H
#define MENUITEM_H


#include <string>
#include <cstdlib>
#include <regex>
#include <iostream>
#include <limits>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <exception>
#include <rapidxml/rapidxml.h>

class Menu;

class MenuItem{
	friend class Menu; //debugging purposes for now
		
	public:
		MenuItem() = delete;
		MenuItem(const MenuItem &m);	
		MenuItem& operator=(const MenuItem &m);
		bool operator<(const MenuItem& m) const;
		explicit MenuItem(rapidxml::xml_node<>* item);

		bool active() const;
		void activate();

		std::string description() const;
		void setDescription(std::string);

		std::string name() const;
		void setName(std::string);

		std::string datestring() const;
		void setDatestring(std::string);

		std::time_t timeToComplete() const;
		void setTimeToComplete(std::time_t);

		std::string timeRemainingString() const;
		std::time_t timeRemaining() const;
		std::time_t time() const;
		void setTime(std::time_t);

		std::string hookExpire() const;

		double priority() const;
		std::string timeLeftString() const;
		std::string timeToCompleteString() const;

		int ID() const;
	private:
		bool m_selected;
		int m_id;
		static int numMenus;

		rapidxml::xml_node<>* m_xml_node;

		std::string m_datestring;
		std::string m_description;
		std::string m_hook_expire;
		std::string m_name;

		std::time_t m_time_completion;	
		std::time_t m_time_due;	
};

#endif 
