#include "../include/menu.h"

#define ALT_BACKSPACE 127 //this catches non-standard backspace signals (^?)

//must use global in main, since multiple calls to getenv() actually modify the value returned for some 
//weird reason. It is actually in the getenv() documentation. MAKES NO SENSE
extern std::string home;

/* These keybindings are intended to be modified by a config file eventually. Their definitions are mean to be taken literally 
 */
const unsigned int REMOVE_KEY = 'r';
const unsigned int ADD_KEY = 'a';
const unsigned int DOWN_KEY = 'j';
const unsigned int UP_KEY = 'k';
const unsigned int QUIT_KEY = 'q';
const unsigned int VIEW_KEY = 'v';

//use this set to catch special characters when reading input
std::set<chtype> special = {KEY_DC, ALT_BACKSPACE, KEY_BACKSPACE, KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN};

//initalization of static member
bool Menu::run = true;

void clearScreen() {
	move(0,0);
	clrtobot();
}

/* uses regex parsing to extract times, and modifies the string passed so that it strictly follows the MM/DD/YY HH:MM format. No missing digits
   */
std::time_t createDueTime(std::string& s, std::string regexp) {
	std::smatch matches;
	expandDateString(s);
	std::regex_match(s, matches, std::regex(regexp));
	std::tm t;
	if(matches.size() == 6) {
		t.tm_mon = stoi(matches[1]) - 1;
		t.tm_mday = stoi(matches[2]);
		t.tm_year = stoi(matches[3]) + 100;
		t.tm_hour = stoi(matches[4]);
		t.tm_min = stoi(matches[5]);
		return std::mktime(&t);
	}
	else{
		return 0;
	}
}

std::time_t createCompletionTime(std::string& s) {
	char suffix = *(s.end()-1);
	int multiplier = 0;
	switch(suffix) {
		case('d'):
			multiplier = 60 * 60 * 24;
			break;
		case('h'):
			multiplier = 60 * 60;
			break;
		case('m'):
			multiplier = 60;
			break;
	}

	s.erase(s.end() - 1, s.end());
	return stoi(s) * multiplier;
}


void Menu::sort() {
	//sorts menu so that higher priority items show up first
	std::sort(this->menu_items.begin(), this->menu_items.end(), [](const MenuItem& left, const MenuItem& right) {
			return (left.priority() < right.priority());
	});
}

static void handleSpecialKeys(int maxx, int maxy, int x, int y, int promptLen, chtype c, std::string& str) {
	switch(c) {
		/*
		case(KEY_LEFT):
			x -= (x != 0);
			move(y,x);
			break;
		case(KEY_RIGHT):
			x += (x != maxx);
			move(y,x);
			break;
		case(KEY_UP):
			y -= (y != 0);
			move(y,x);
			break;
		case(KEY_DOWN):
			y += (y != maxy);
			move(y,x);
			break;
			*/
		case(KEY_DC):
			noecho();
			delch();
			str.erase(y * maxx + x - promptLen);
			echo();
			break;
		case(KEY_BACKSPACE):
		case(ALT_BACKSPACE):
			//move cursor - most compact way to write... uses short-circuit evaluation to modify or keep the
			//current value for y, while adjusting x to what it needs to be. However, it requires this long
			//comment to explain it. Maybe it would have been better to be more explicit.
			x = (x == 0 && static_cast<unsigned long int>(y = y - 1) >= str.length()/maxx) ? maxx - 1 : x - 1;
			move(y, x);
			if(y * maxx + x < promptLen) {
				x = promptLen;
				move(y, x);
			}else{ 
				delch(); 
				str.erase(str.length() - 1);
			}
			refresh();
			break;
	}
}

//there is a blank element when the .schedule file is empty. Investigate this next
static void print(std::string q, int drawFlags = A_NORMAL) {
	//iterates through the string, applying special effects and printing	
	for(char c : q) {
		waddch(stdscr, c | drawFlags);
	}
}
//initializes menu with a file
Menu::Menu(std::string path) : message(WELCOME_MSG) {
	std::ifstream file(path, std::ios_base::in);
	if(!file) { //prints error message if filepath was invalid

		attrset(A_BOLD);
		std::string errorMsg("ERROR LOADING FILE: ");
		addnstr(errorMsg.c_str(), errorMsg.length());
		addnstr((path + "\n").c_str(), path.length());
		attroff(A_BOLD);

		exit(1);
	}
	
	std::string buffer, temp;

	//read entire file into 'buffer'
	while(std::getline(file, temp)) {
		buffer += temp;
	}

	file.close();	

	//update global string and xml_document object
	config_text = config_xml.allocate_string(buffer.c_str());
	config_xml.parse<0>(config_text);

	auto root_xml_node = config_xml.first_node();

	for(auto item_node = root_xml_node->first_node(); item_node != NULL; item_node = item_node->next_sibling())
		menu_items.push_back(MenuItem(item_node));

	getmaxyx(stdscr, height, width);
	this->sort();
}

Menu::~Menu() {
}

Menu::operator bool() const{
	return run;
}

void updateConfig() {
	std::ofstream file(conf_path, std::ios::out);
	file << config_xml;
	file.close();
	//add logic here to communicate with daemon process
}

// This enum represents the indexes into the responses variable just above this comment. I know this could be done with a c++11 array, but this is easier to maintain while its early in production
enum FieldMatches {
	NAME,
	DESCRIPTION,
	DATESTRING,
	COMPLETION_TIME,
	HOOK_EXPIRE
};

void Menu::addItem() {
	curs_set(1);	
	const std::string path = homedir + "/.scheduler/.schedule_add";
	scr_dump(path.c_str());
	std::smatch matches;
	std::regex match_anything (".*"); //matches anything and everything
	std::regex match_dates ("(\\d\\d?)/(\\d\\d?)/(\\d\\d) (\\d\\d?)\\:(\\d\\d)\\W*"); //matches dates + times in MM/DD/YYYY HH:MM format
	std::regex match_times ("([1-9]\\d*[hdm]).*"); //matches time string such as 2h or 2h 55m
	std::regex match_path ("([-/a-zA-Z_.]*[-a-zA-Z_.]+)"); //matches a filesystem path for linux system

	//pairs of answers along with the regex matches that they yield
	std::vector<std::pair<std::string, std::smatch>> responses;

	//pairs of prompts, where pair.first is the prompt to give the user and pair.second contains the regex to 
	//match against user inputs
	std::vector<std::pair<std::string, std::regex> > userPrompts = {
		{"Please Enter a Name for This Event: ", match_anything },
		{"Please Enter a Description for This Event: ", match_anything },
		{"Please Enter a Due Date (MM/DD/YY HH:MM): ", match_dates },	
		{"Please Enter the time this will take (number followed by m,h,d): ", match_times},
		{"Please Enter the path to a script to execute when an item expires", match_path}
	};

	for(auto prompt = userPrompts.begin(); prompt != userPrompts.end(); prompt++) {
		//wipe screen and print prompt
		clearScreen();
		addstr(prompt->first.c_str());	

		std::string response;

		chtype c;

		do{
			//get char
			c = getch();
			if(special.find(c) != special.end()) { //pass off control to handler function if c is a special control character
				handleSpecialKeys(getmaxx(stdscr), getmaxy(stdscr), getcurx(stdscr), getcury(stdscr), prompt->first.length(), c, response);
			}else if (c != 10){
				//add to response and blit to screen
				response += static_cast<char>(c);
				addch(c);
			}
		//c != newline
		} while(c != 10);

		std::smatch matches;

		//if answer matches the prompt's format, add it to list of responses
		if(Ex::run("menu.cpp at 202", [=, &matches] () -> bool { return regex_match(response, matches, prompt->second); } )) {
			responses.push_back(std::pair<std::string, std::smatch>(response, matches));
		}
		//if not, prompt again
		else {
			prompt--;
			continue;
		}		
	}

	//construct item and add to list
	
	std::string completionMatch = responses[COMPLETION_TIME].first;
	// THIS IS uNUSUED // auto eventTime = createDueTime(responses[DATESTRING].first, "(\\d\\d?)/(\\d\\d?)/(\\d\\d) (\\d\\d?)\\:(\\d\\d).*");	
	auto completionTime = createCompletionTime(completionMatch);	
	auto newItem = config_xml.allocate_node(rapidxml::node_element, "item");

	std::vector<rapidxml::xml_node<>*> nodes = {
		config_xml.allocate_node(rapidxml::node_element, "name", config_xml.allocate_string(responses[NAME].first.c_str())),
		config_xml.allocate_node(rapidxml::node_element, "description", config_xml.allocate_string(responses[DESCRIPTION].first.c_str())),
		config_xml.allocate_node(rapidxml::node_element, "datestring", config_xml.allocate_string(responses[DATESTRING].first.c_str())),
		config_xml.allocate_node(rapidxml::node_element, "completion_time", config_xml.allocate_string(std::to_string(completionTime).c_str())),
		config_xml.allocate_node(rapidxml::node_element, "hook_expire", config_xml.allocate_string(responses[HOOK_EXPIRE].first.c_str()))
	};
	for(auto node : nodes) {
		newItem->append_node(node);
	}
	config_xml.first_node()->append_node(newItem);
	menu_items.push_back(MenuItem(newItem));
	updateConfig();
	this->sort();
	scr_restore(path.c_str());
	curs_set(0);	
}

void Menu::update() {
	this->display();
	wmove(stdscr, 0,0);			//move cursor to top, so text gets written over	
	wrefresh(stdscr);
	chtype key = getch();
	bool valid_press = true;
	switch(key) {
		case REMOVE_KEY:
			remove();
			break;
		case ADD_KEY:
			addItem();
			break;
		case DOWN_KEY:
			down();
			break;
		case UP_KEY:
			up();
			break;
		case QUIT_KEY:
			exit(0);
			break;
		case VIEW_KEY:
			viewMenuItem();
			break;
		default:
			valid_press = false;
			this->message = "Hotkey not registered. Press h for a short list of all commands\n";
			break;
	}

	if(valid_press) {
		this->message = WELCOME_MSG;	
	}
}

void Menu::remove() {
	auto root_xml_node = config_xml.first_node();
	auto item_node_to_delete = root_xml_node->first_node("item");

	if ( item_node_to_delete ) {
		root_xml_node->remove_node(item_node_to_delete);
		menu_items.erase(menu_items.begin() + selectIndex);
		//calculate where the arrow should appear
		selectIndex = selectIndex >= menu_items.size() ? menu_items.size() - 1 : selectIndex;
		updateConfig();
	} else {
		logError("Could not delete node. Check the formatting of the XML Node. This \n \
				  might be because you tried to delete the node non-recursively");
	}
	
}

//changes the view to display the detailed description of an item along with other details. Dumps the window 
//into a file, then restores the file after the user does not want to view the event anymore.
void Menu::viewMenuItem() {
	//save screen to file and clear window
	scr_dump((homedir + "/.scheduler/.schedule_dump").c_str());

	do{
		clearScreen();

		std::string name = std::string("Name: ") + menu_items[selectIndex].name();
		std::string description = std::string("Description: ") + menu_items[selectIndex].description();
		std::string datestring = std::string("Due Date: ") + menu_items[selectIndex].datestring();
		std::string time_left = std::string("Time Left: ") + menu_items[selectIndex].timeRemainingString();
		std::string time_to_complete = std::string("Time It Will Comsume: ") + menu_items[selectIndex].timeToCompleteString();
		std::string hook_expire = std::string("Hook on Expire: ") + menu_items[selectIndex].hookExpire();

		addstr(name.c_str()); addstr("\n");
		addstr(description.c_str()); addstr("\n");
		addstr(datestring.c_str()); addstr("\n");
		addstr(time_left.c_str()); addstr("\n");
		addstr(time_to_complete.c_str()); addstr("\n");

		addstr("\nScripts to Execute on Hook:\n\n");
		addstr(hook_expire.c_str()); addstr("\n");

		refresh();

	} while(getch() != 'q');

	clearScreen();
	scr_restore((homedir + "/scheduler/.schedule_dump").c_str());
}

//this function is called when destructing a menu and when a signal is caught.
void Menu::exit(int sig) {
	run = false;
	std::fstream file(conf_path, std::ios::out);
	//write the xml back to the file
	file << config_xml;
	file.close();
}

//Prints a string up to $spaces number of characters, ending with spaces if there is excess and an elipses "..." when there is not enough spaces
//
//delimiter defaults to ' '
bool Menu::printField(std::string field, unsigned int spaces, char delimiter, stringtype type) {
	logDetail(field.c_str());
	if(type == HEADER) {
		std::string finished_field;
		int heading_spacing = (spaces - field.length())/2;
		bool odd_spacing = (spaces - field.length())%2;
		//if the field does not have even distribution of surrounding space
		if(odd_spacing) {
			finished_field = " ";
		}
		finished_field.insert(0, heading_spacing, delimiter);
		finished_field += field;
		finished_field.insert(finished_field.length(), heading_spacing, delimiter);
		addnstr(finished_field.c_str(), spaces);
		return false;
	}
		
	bool tooBig = false;
	std::string ending = "..."; //used if field is longer than spaces
	if(field.length() > spaces) {
		tooBig = true;
		int beginIndex = spaces - ending.length(); //where to insert the ending
		field.insert(beginIndex, ending);
	}else{
		//fills the field with spaces
		field.append(spaces - field.length(),delimiter);
	}
//	logDetail(field.c_str());
	addnstr(field.c_str(), spaces);	
	return tooBig;
}

void Menu::printMenu() {
	//prints basic menu headers
	if(!menu_items.empty()) {
		print("  ");
		printField("NAME", NAME_SPACING, ' ', NORMAL); print("|");
		printField("DESCRIPTION", DESC_SPACING, ' ', NORMAL); print("|");
		printField("DATE", DATE_SPACING, ' ', NORMAL); print("|");
		printField("DURATION ( hh:mm:ss )", DEBUG_SPACING, ' ', NORMAL);
		addch('\n');
	}
	for(auto item = menu_items.begin(); item != menu_items.end(); item++) {	
		if(item - menu_items.begin() == selectIndex && !menu_items.empty()) {
			//print arrow indicating current item
			addnstr("->", 2);
			attrset(A_STANDOUT);
		}
		else{
			//otherwise print spaces
			print("  ");
		}
		//prints up to NAME_SPACING characters
		printField(item->name(), NAME_SPACING, ' ', NORMAL); print("|");
		printField(item->description(), DESC_SPACING, ' ', NORMAL); print("|");
		printField(item->datestring(), DATE_SPACING, ' ', NORMAL); print("|");
		//This line prints the number used to compare and sort the items of the menu
		printField(item->timeToCompleteString(), DEBUG_SPACING, ' ', NORMAL);
		attroff(A_STANDOUT);
		addch('\n');
	}
	return;
}

void Menu::display() {
	//move cursor to top-left of window and erase all contents
	clearScreen();
	//print a message and the menu
	print(message);
	printMenu();
}
//change menu selection
void Menu::up() {
	if(selectIndex > 0)
		selectIndex--;
}


//change menu selection
void Menu::down() {
	if(selectIndex + 1 < menu_items.size())
		selectIndex++;
}

void expandDateString(std::string& datestring) {
	//two iterators start at beginning of datestring
	int spaceCount = 0;
	std::string::iterator first = datestring.begin(); 
	std::string::iterator second = first;
	int firstSpace = datestring.find(' ');

	if(datestring[firstSpace + 1] == ' ') {
		datestring.erase(firstSpace, 1);
	}

	while(second != datestring.end()) {
		if(*second == '/' || *second == ':' || *second == ' ') {
			if(second - first - spaceCount == 1) {
				//insert leading zero if its missing
				datestring.insert(first, '0');
				second = first = datestring.begin();
			}else{
				//all good, move past the slash
				first = ++second;
			}
			if(*second == ' ') {
				spaceCount++;
			}
		}
		second++;
	}
}

