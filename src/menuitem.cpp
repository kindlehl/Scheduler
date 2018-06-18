#include "../include/menuitem.h"
//definition of numMenus static variable
int MenuItem::numMenus = 0;
/* Constructs a MenuItem when given all of its data members
 * directly mutates member variables
 */
MenuItem::MenuItem(std::string description, std::string name, std::string date, std::time_t timing)
	: m_id(numMenus++), m_selected(false), m_datestring(date), m_description(description), m_name(name), m_time_due(timing){
}

/*FILE STRUCTURE
 * m_name|m_description|m_datestring|m_time_due|\n
 * DESIRED BEHAVIOR
 * Extracts 1 token per DATA MEMBER, discards rest of the line
 * Populates class members with file contents
 */
MenuItem::MenuItem(std::istream& file, char delim) : m_selected(false){
	//reduce confusion without polluting the namespace
	constexpr int numFields = 4;
	std::string token[numFields];

	for(int i = 0; i < numFields; i++){
		getline(file, token[i], '|'); //fill each string with a token from the file.
	}

	m_name = token[0]; //extract name	
	m_description = token[1]; //extract description
	m_datestring = token[2]; //extract date
	m_time_due = std::stoi(token[3]); //extract time until event occurs

	file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //ignore discard the rest of the line
	if(file.peek() == '\n')	
		//set eofbit in input stream if extra whitespace is found or eof 
		//occurs. This was introduced because of a bug where a menuitem
		//without any populated fields was created, and an exception was
		//thrown by the call to std::stoi above. DO NOT REMOVE
		file.setstate(std::ios_base::eofbit);
	m_id = numMenus++;
}

MenuItem::MenuItem(const MenuItem &m): m_id(m.m_id), m_selected(m.m_selected), m_datestring(m.m_datestring),m_description(m.m_description), m_name(m.m_name), m_time_due(m.m_time_due)
{
} 

MenuItem& MenuItem::operator=(const MenuItem &m){
	//this temp variable allows self-copying
	MenuItem tempItem(m);
	this->m_time_due = tempItem.m_time_due;
	this->m_datestring = tempItem.datestring();
	this->m_name = tempItem.m_name;
	this->m_description = tempItem.m_description;
	return *this;
}

bool MenuItem::operator<(const MenuItem& m) const{
	return this->timeRemaining() < m.timeRemaining();		//used by std::sort 
}

//returns remaining time in seconds
std::time_t MenuItem::timeRemaining() const{
	std::time_t curr;
	std::time(&curr);
	return std::difftime(m_time_due, curr);
}

bool MenuItem::active() const{
	return m_selected;
}

void MenuItem::activate(){
	m_selected = true;
}

std::string MenuItem::name() const{
	return m_name;
}

void MenuItem::setDatestring(std::string datestring){
	m_datestring = datestring;
}

std::string MenuItem::datestring() const{
	return m_datestring;
}

void MenuItem::setName(std::string name){
	m_name = name;
}

std::time_t MenuItem::time() const{
	return m_time_due;
}

void MenuItem::setTime(std::time_t time){
	m_time_due = time;
}

std::string MenuItem::description() const{
	return m_description;
}

void MenuItem::setDescription(std::string desc){
	m_description = desc;
}
