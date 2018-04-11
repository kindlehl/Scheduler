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

bool allWhitespace(std::string str){
	return std::all_of(str.begin(),str.end(),isspace);
}

/*FILE STRUCTURE
 * NAME|DESCRIPTION|PRIORITY
 *
 * DESIRED BEHAVIOR
 * Extracts 1 token per DATA MEMBER, discards remaining tokens
 */
MenuItem::MenuItem(std::istream& lineFromFile, char delim){
	std::string line;
	std::getline(lineFromFile, line);
	std::istringstream mystream(line);
	//uses getline to extract tokens
	std::string token;
	//extract name	
	std::getline(mystream, token, delim);
	m_name = token;
	//extract description
	std::getline(mystream, token, delim);
	m_description = token;
	//extract priority level
	std::getline(mystream, token, delim);
	m_priority = atoi(token.c_str());
	
	selected = false;
	//discard remaining bits of line
	while(isspace(lineFromFile.peek())){
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

bool MenuItem::operator<(const MenuItem& m)const{
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
