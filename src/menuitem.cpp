#include "../include/menuitem.h"

MenuItem::MenuItem() : m_priority(MAX_PRIORITY), m_description("No Description"), m_name("__blank__"){
	selected = false;

}

MenuItem::MenuItem(int priority, std::string description, std::string name)
	: m_priority(priority), m_description(description), m_name(name){
	selected = false;	
}

bool MenuItem::active() const{
	return selected;
}

MenuItem::MenuItem(const MenuItem &m) 
	: m_priority(m.m_priority), m_description(m.m_description), m_name(m.m_name)
{
	selected = m.selected;	
}

MenuItem& MenuItem::operator=(const MenuItem &m){
	MenuItem tempItem(m);
	this->m_priority = tempItem.m_priority;
	this->m_name = tempItem.m_name;
	this->m_description = tempItem.m_description;
	return *this;
}

bool MenuItem::operator<(const MenuItem& m){
	return this->priority() < m.priority() ? false : true;		//used by std::sort 
}

std::string MenuItem::name(){
	return m_name;
}

std::string MenuItem::description(){
	return m_description;
}

int MenuItem::priority() const{
	return m_priority;
}
