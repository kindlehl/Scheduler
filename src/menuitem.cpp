#include "../include/menuitem.h"
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
/*THIS FUNCTION WAS DUPLICATED HERE. IT WAS ORIGINALLY DEFINED IN SRC/MENU.CPP
*
time_t createTime(std::string s, std::string regexp){
	std::smatch matches;
	std::regex_match(s, matches, std::regex(regexp));
	std::tm t;
		t.tm_mon = stoi(matches[1]);
		t.tm_mday = stoi(matches[2]);
		t.tm_year = stoi(matches[3]) + 100;
		t.tm_hour = stoi(matches[4])-1;
		t.tm_min = stoi(matches[5]);
		return std::mktime(&t);
}
*/
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
	//reduce confusion
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
	if(token != ""){
		eventTime = stoi(token);	
		std::ofstream log("projectLog", std::ofstream::app);
		log.width(50);
		log << m_name << ": eventTime = ";
		log.width(10);
		log << eventTime << std::endl;
	}
	else
		eventTime = 1;
	//prune whitespace from file
	while(isspace(lineFromFile.peek())){
		lineFromFile.ignore();
	}
	//give unique ID
	ID = numMenus++;
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

