#include "../include/menuitem.h"
//definition of numMenus static variable
int MenuItem::numMenus = 0;

bool allWhitespace(std::string str);
//returns remaining time in minutes
std::time_t MenuItem::timeRemaining() const{
	std::time_t curr;
	std::time(&curr);
	return std::difftime(eventTime, curr);
}

MenuItem::MenuItem(std::string description, std::string name, std::string date, std::time_t timing)
	: eventTime(timing), dateString(date), selected(false), m_description(description), m_name(name){
	ID = numMenus++;

}

bool allWhitespace(std::string str){
	return std::all_of(str.begin(),str.end(),isspace);
}

/*FILE STRUCTURE
 * NAME|DESCRIPTION|DATE|INTERNAL TIME|
 *
 * DESIRED BEHAVIOR
 * Extracts 1 token per DATA MEMBER, discards remaining tokens
 */
MenuItem::MenuItem(std::istream& lineFromFile, char delim) : selected(false){
	//reduce confusion without polluting the namespace
	using std::string, std::getline, std::istringstream;
	//grab line from file, construct stringstream from line
	string line;
	getline(lineFromFile, line);
	istringstream mystream(line);

	//uses getline to extract tokens from stringstream, delimited by delim
	string token;
	//extract name	
	getline(mystream, token, delim);
	m_name = token;
	//extract description
	getline(mystream, token, delim);
	m_description = token;
	//extract date
	getline(mystream, token, delim);
	dateString = token;	
	//get internal date	
	getline(mystream, token, delim);
}

void MenuItem::setTime(std::time_t theTime){
	eventTime = theTime;
}

bool MenuItem::active() const{
	return selected;
}

MenuItem::MenuItem(const MenuItem &m) 
	:  eventTime(m.eventTime), dateString(m.dateString), m_description(m.m_description), m_name(m.m_name) 
{
	selected = m.selected;	
	ID = m.ID;
}

MenuItem& MenuItem::operator=(const MenuItem &m){
	//this temp variable allows self-copying
	MenuItem tempItem(m);
	this->eventTime = tempItem.eventTime;
	this->dateString = tempItem.dateString;
	this->m_name = tempItem.m_name;
	this->m_description = tempItem.m_description;
	return *this;
}

bool MenuItem::operator<(const MenuItem& m)const{
	return this->timeRemaining() < m.timeRemaining();		//used by std::sort 
}

std::string MenuItem::name(){
	return m_name;
}

std::string MenuItem::description(){
	return m_description;
}

