#include "../include/menuitem.h"
//definition of numMenus static variable
int MenuItem::numMenus = 0;
/* Constructs a MenuItem when given all of its data members
 * directly mutates member variables
 */

/* XML Format
 * <username>
 *	<item>
 *		<name>...</name>
 *		<description>...</description>
 *		...
 *	</item>
 *	<item>
 *		<name>...</name>
 *	
 * </username>
 */
MenuItem::MenuItem(rapidxml::xml_node<>* item) : m_selected(false){
	m_name = item->first_node("name")->value(); //extract name	
	m_description = item->first_node("description")->value(); //extract description
	m_datestring =  item->first_node("datestring")->value();//extract date
	m_time_completion =  atoi(item->first_node("completionTime")->value());//extract time until event occurs
	m_id = numMenus++;
}

MenuItem::MenuItem(const MenuItem &m): m_id(m.m_id), m_selected(m.m_selected), m_datestring(m.m_datestring),m_description(m.m_description), m_name(m.m_name), m_time_due(m.m_time_due), m_time_completion(m.m_time_completion)
{
} 

MenuItem& MenuItem::operator=(const MenuItem &m){
	//this temp variable allows self-copying
	MenuItem tempItem(m);
	this->m_time_due = tempItem.m_time_due;
	this->m_time_completion = tempItem.m_time_completion;
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

double MenuItem::priority(){
	return timeRemaining()/timeToComplete();
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

std::time_t MenuItem::timeToComplete() const{
	return m_time_completion;
}

void MenuItem::setTimeToComplete(std::time_t newTime){
	m_time_completion = newTime;
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

std::string MenuItem::timeToCompleteString(){
	std::time_t time_left = this->timeToComplete();
	int secondsPerHour = 60 * 60;
	int secondsPerMinute = 60;

	std::string h, m, s;

	int hoursLeft = time_left / secondsPerHour;
	time_left %= secondsPerHour;

	int minutesLeft = time_left / secondsPerMinute;
	time_left %= secondsPerMinute;

	int secondsLeft = time_left;

	//these if statments check for single digits
	//it turns 2:9:15 to 02:09:15
	if ( hoursLeft  <= 9 ){
		h = "0";
	}
	if ( minutesLeft  <= 9 ){
		m = "0";
	}
	if ( secondsLeft  <= 9 ){
		s = "0";
	}
		
	h += std::to_string(hoursLeft);
	m += std::to_string(minutesLeft);
	s += std::to_string(secondsLeft);

	return h + ":" + m + ":" + s;
}

std::string MenuItem::timeLeftString(){
	std::time_t time_left = this->timeRemaining();
	int secondsPerDay = 60 * 60 * 24;
	int secondsPerHour = 60 * 60;
	int secondsPerMinute = 60;

	int daysLeft = time_left / secondsPerDay;
	time_left %= secondsPerDay;

	int hoursLeft = time_left / secondsPerHour;
	time_left %= secondsPerHour;

	int minutesLeft = time_left / secondsPerMinute;
	time_left %= secondsPerMinute;

	int secondsLeft = time_left;

	return std::to_string(daysLeft) + " days and " + std::to_string(hoursLeft) + ":" + std::to_string(minutesLeft) + ":" + std::to_string(secondsLeft);

}
