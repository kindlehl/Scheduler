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

time_t dateToSeconds(std::string datestring);

MenuItem::MenuItem(rapidxml::xml_node<>* item) :
	m_datestring(item->first_node("datestring")->value()), //extract date
	m_description(item->first_node("description")->value()), //extract description
	m_hook_expire(item->first_node("hook_expire")->value()), //extract date
	m_id(numMenus++),
	m_name(item->first_node("name")->value()), //extract name	
	m_selected(false),
	m_time_completion(atoi(item->first_node("completion_time")->value())) //extract time until event occurs
{
	//copy date from xml into seconds since epoch time
	struct tm timeval;
	memset(&timeval, 0, sizeof(struct tm));
	strptime(item->first_node("datestring")->value(), "%m/%d/%y %H:%M", &timeval);
	m_time_due = mktime(&timeval);
}

MenuItem::MenuItem(const MenuItem &m) : 
	m_datestring(m.m_datestring),
	m_description(m.m_description),
	m_hook_expire(m.m_hook_expire),
	m_id(m.m_id),
	m_name(m.m_name),
	m_selected(m.m_selected),
	m_time_completion(m.m_time_completion),
	m_time_due(m.m_time_due)
{
} 

MenuItem& MenuItem::operator=(const MenuItem &m){
	this->m_datestring = m.datestring();
	this->m_description = m.m_description;
	this->m_hook_expire = m.m_hook_expire;
	this->m_id = m.m_id;
	this->m_name = m.m_name;
	this->m_selected = m.m_selected;
	this->m_time_completion = m.m_time_completion;
	this->m_time_due = m.m_time_due;
	return *this;
}

bool MenuItem::operator<(const MenuItem& m) const {
	return this->timeRemaining() < m.timeRemaining();		//used by std::sort 
}

//returns remaining time in seconds
std::time_t MenuItem::timeRemaining() const {
	std::time_t curr = std::time(NULL);
	return std::difftime(m_time_due, curr);
}

std::string MenuItem::timeRemainingString() const {
	//seconds until event expires
	std::time_t seconds = this->timeRemaining();

	std::time_t days = seconds / 86400;
	seconds -= days * 86400;

	std::time_t hours = seconds / 3600;
	seconds -= hours * 3600;

	std::time_t minutes = seconds / 60;
	seconds -= minutes * 60;

	std::time_t s = seconds / 60;
	seconds -= s ;

	std::ostringstream str;

	str << std::to_string(days) + "d " <<
	std::to_string(hours) + "h " <<
	std::to_string(minutes) + "m " <<
	std::to_string(s) + "s ";

	return str.str();
}

double MenuItem::priority() const {
	return timeRemaining()/timeToComplete();
}

bool MenuItem::active() const {
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

std::time_t MenuItem::timeToComplete() const {
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

std::string MenuItem::hookExpire() const {
	return m_hook_expire;
}

std::string MenuItem::timeToCompleteString() const {
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

std::string MenuItem::timeLeftString() const {
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

//transforms string in form MM/DD/YY HH:MM to epoch time
std::time_t dateToSeconds(std::string datestring) {
	char tmp[3] = {0};
	struct tm t;
	memset(&t, 0, sizeof(struct tm));

	//copy month into t
	strncpy(tmp, datestring.c_str(), 2);
	t.tm_mon = atoi(tmp) - 1;

	//copy day into t
	strncpy(tmp, datestring.c_str() + 3, 2);
	t.tm_mday = atoi(tmp);

	//copy year into t
	strncpy(tmp, datestring.c_str() + 6, 2);
	t.tm_year = atoi(tmp) + 100;

	//copy hour into t
	strncpy(tmp, datestring.c_str() + 9, 2);
	t.tm_hour = atoi(tmp);

	//copy minute into t
	strncpy(tmp, datestring.c_str() + 12, 2);
	t.tm_min = atoi(tmp);

	return mktime(&t);
}
