#include "../include/menuitem.h"
int MenuItem::numMenus = 0;
MenuItem::MenuItem() : m_priority(MAX_PRIORITY), m_description("No Description"), m_name("__blank__"){
	selected = false;
	//ID represents the line number in the file that this item represents
	ID = numMenus++;
}

MenuItem::MenuItem(int priority, std::string description, std::string name)
	: m_priority(priority), m_description(description), m_name(name){
	selected = false;	
	ID = numMenus++;
}


/*FILE STRUCTURE
 * NAME|DESCRIPTION|PRIORITY
 *
 * DESIRED BEHAVIOR
 * Extracts 1 token per DATA MEMBER, discards remaining tokens
 */
MenuItem::MenuItem(std::istream& lineFromFile, char delim){
	//uses getline to extract tokens
	std::string token;
	//extract name	
	std::getline(lineFromFile, token, delim);
	m_name = token;
	//extract description
	std::getline(lineFromFile, token, delim);
	m_description = token;
	//extract priority level
	std::getline(lineFromFile, token, delim);
	m_priority = atoi(token.c_str());
	
	selected = false;
	//discard remaining bits of line
	lineFromFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');	
	while(lineFromFile.peek() == ' ' || lineFromFile.peek() == '\n'){
		lineFromFile.ignore();
	}

	ID = numMenus++;
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
