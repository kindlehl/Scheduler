#include "../include/menu.h"
//must use global in main, since multiple calls to getenv() actually modify the value returned for some 
//weird reason. It is actually in the getenv() documentation. MAKES NO SENSE
extern char* HOME;

/* These keybindings are intended to be modified by a config file eventually. Their definitions are mean to be taken literally 
 */
unsigned int REMOVE_KEY = 'r';
unsigned int ADD_KEY = 'a';
unsigned int DOWN_KEY = 'j';
unsigned int UP_KEY = 'k';
unsigned int QUIT_KEY = 'q';
unsigned int VIEW_KEY = 'v';

//use this set to catch special characters when reading input
std::set<chtype> special = {KEY_DC, KEY_BACKSPACE, KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN};

//initalization of static member
bool Menu::run = true;

void clearScreen(){
	move(0,0);
	clrtobot();
}

/* uses regex parsing to extract times, and modifies the string passed so that it strictly follows the MM/DD/YY HH:MM format. No missing digits
   */
std::time_t createDueTime(std::string& s, std::string regexp){
	std::smatch matches;
	std::regex_match(s, matches, std::regex(regexp));
	std::tm t;
	expandDateString(s);
	if(matches.size() == 6){
		try {
			t.tm_mon = stoi(matches[1])-1;
			t.tm_mday = stoi(matches[2]);
			t.tm_year = stoi(matches[3]) + 100;
			t.tm_hour = stoi(matches[4]);
			t.tm_min = stoi(matches[5]);
		} catch (const std::exception& e){
			std::cerr << "Exception caught in createDueTime; menu.cpp:28" << std::endl;
			std::cerr << e.what();
		}
		
		return std::mktime(&t);
	}
	else{
		return 0;
	}
}

std::time_t createCompletionTime(std::string& s){
	time_t seconds = 0;
	std::cout << s << std::endl;
	char suffix = *(s.end()-1);
	int multiplier = 0;
	switch(suffix){
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

void Menu::sort(){
	//sorts menu so that higher priority items show up first
	std::sort(this->menu_items.begin(), this->menu_items.end(), [](const MenuItem& left, const MenuItem& right){
			return (left.timeRemaining() < right.timeRemaining());
	});
}

static void handleSpecialKeys(int maxx, int maxy, int x, int y, int promptLen, chtype c, std::string& str){
	switch(c){
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
		case(KEY_DC):
			noecho();
			delch();
			str.erase(y*maxx+x - promptLen);
			echo();
			break;
		case(KEY_BACKSPACE):
			//move cursor - most compact way to write... uses short-circuit evaluation to modify or keep the
			//current value for y, while adjusting x to what it needs to be. However, it requires this long
			//comment to explain it. Maybe it would have been better to be more explicit.
			x = (x == 0 && (y = y-1) >= str.length()/maxx) ? maxx-1 : x-1;
			move(y, x);
			if(y*maxx+x < promptLen){
				x = promptLen;
				move(y, x);
			}else{ 
				delch(); 
				str.erase(str.length()-1);
			}
			refresh();
			break;
	}
}

//there is a blank element when the .schedule file is empty. Investigate this next
static void print(std::string q, int drawFlags = A_NORMAL){
	//iterates through the string, applying special effects and printing	
	for(char c : q){
		waddch(stdscr, c | drawFlags);
	}
}
//initializes menu with a file
Menu::Menu(std::string path){
	std::ifstream file(path, std::ios_base::in);
	if(!file){ //prints error message if filepath was invalid
		attrset(A_BOLD);
		std::string errorMsg("ERROR LOADING FILE: ");
		addnstr(errorMsg.c_str(), errorMsg.length());
		addnstr((path + "\n").c_str(), path.length());
		attroff(A_BOLD);
	}
	
	std::string buffer, temp;
	//while more events exist, lol. I did this on purpose
	while(file){
		std::getline(file, temp);
		buffer += temp;
	}
	file.close();	
	config_text = static_cast<char*>(malloc(buffer.size() + 1));
	memcpy(config_text, buffer.c_str(), buffer.size());
	config_text[buffer.size()] = '\0';	
	config_xml.clear();
	config_xml.parse<0>(config_text);
	for(auto xmlnode = config_xml.first_node()->first_node(); xmlnode; xmlnode = xmlnode->next_sibling())
		menu_items.push_back(MenuItem(xmlnode));
	getmaxyx(stdscr, height, width);
	this->sort();
}

Menu::~Menu(){
}

Menu::operator bool() const{
	return run;
}
void Menu::addItem(){
	curs_set(1);	
	const char* path = (std::string(HOME)+ "/.schedule_add").c_str();
	scr_dump(path);
	chtype c;
	std::smatch matches;
	std::regex match_anything (".*");
	std::regex match_dates ("(\\d\\d?)/(\\d\\d?)/(\\d\\d) (\\d\\d?)\\:(\\d\\d)\\W*");
	std::regex match_times ("([1-9]\\d*[hdm]).*");

	//vector of pairs of form - {"Prompt Text that shows before the cursor", "Vector of Regular Expressions that user's input must match"}
	std::vector<std::pair<std::string, std::regex> > userPrompts = {
		{"Please Enter a Description for This Event: ", match_anything },
		{"Please Enter a Name for This Event: ", match_anything },
		{"Please Enter a Due Date (MM/DD/YY HH:MM): ", match_dates },	
		{"Please Enter the time this will take (number followed by m,h,d): ", match_times}
	};

	std::vector<std::pair<std::string, std::smatch>> responses;
	for(auto prompt = userPrompts.begin(); prompt != userPrompts.end(); prompt++){
		clearScreen();
		addstr(prompt->first.c_str());	
		std::string response;
		do{
			c = getch();
			if(special.find(c) != special.end()){
				handleSpecialKeys(getmaxx(stdscr), getmaxy(stdscr), getcurx(stdscr), getcury(stdscr), prompt->first.length(), c, response);
			}else{
				response+=static_cast<char>(c);
				addch(c);
			}
		//c != newline
		} while(c != 10);

		response.erase(response.length()-1);
		std::smatch matches;

		//if answer matches the prompt's format, add it to list of responses

		if(regex_match(response, matches, prompt->second)){
			responses.push_back(std::pair<std::string, std::smatch>(response, matches));
		}
		else {
			prompt--;
			continue;
		}		
	}
	//construct item and add to list
	
	auto eventTime = createDueTime(responses[2].first, "(\\d\\d?)/(\\d\\d?)/(\\d\\d) (\\d\\d?)\\:(\\d\\d).*");	
	std::string completionMatch(responses[3].second[1]);
	auto completionTime = createCompletionTime(completionMatch);	
	menu_items.push_back(MenuItem(responses[0].first, responses[1].first, responses[2].first, eventTime, completionTime));
	this->sort();
	scr_restore(path);
	curs_set(0);	
}

void Menu::update(){
	this->display();
	wmove(stdscr, 0,0);			//move cursor to top, so text gets written over	
	wrefresh(stdscr);
	chtype key = getch();
	if(key == REMOVE_KEY)
		remove();
	else if(key == ADD_KEY)
		addItem();
	else if(key == DOWN_KEY)
		down();
	else if(key == UP_KEY)
		up();
	else if(key == QUIT_KEY)
		exit(0);
	else if(key == VIEW_KEY)
		viewMenuItem();
}

void Menu::remove(){
	menu_items.erase(menu_items.begin()+selectIndex);
	//calculate where the arrow should appear
	selectIndex = selectIndex >= menu_items.size() ? menu_items.size()-1 : selectIndex;
}

//changes the view to display the detailed description of an item along with other details. Dumps the window into a file, then restores the file after the user does not want to view the event anymore.
void Menu::viewMenuItem(){
	//save screen to file and clear window
	scr_dump((std::string(HOME) + "/.schedule").c_str());
	do{
		clearScreen();
		printField(menu_items[selectIndex].name() + "\n", width);
		printField(menu_items[selectIndex].description() + "\n", width);
		printField(std::to_string(menu_items[selectIndex].timeRemaining()) + "\n", width); 
	} while(getch() != 'q');
	clearScreen();
	scr_restore((std::string(HOME) + "/.schedule").c_str());
}

//this function is called when destructing a menu and when a signal is caught.
void Menu::exit(int sig){
	run = false;
	std::fstream file(std::string(HOME) + "/.schedule", std::ios::out);
	//write menu_items back to file, overwriting it
	for(auto item : menu_items){
			file << item.name() << "|" << item.description() << "|" << item.datestring() << "|" << item.time() << "|\n";
	}
	file.close();
}

//Prints a string up to $spaces number of characters, ending with spaces if there is excess and an elipses "..." when there is not enough spaces
//
//delimiter defaults to ' '
bool Menu::printField(std::string field, unsigned int spaces, char delimiter){
	bool tooBig = false;
	std::string ending = "..."; //used if field is longer than spaces
	if(field.length() > spaces){
		tooBig = true;
		int beginIndex = spaces-ending.length(); //where to insert the ending
		field.insert(beginIndex, ending);
	}else{
		//fills the field with spaces
		field.append(spaces-field.length(),delimiter);
	}
	addnstr(field.c_str(), spaces);	
	return tooBig;
}

void Menu::printMenu(){
	//prints basic menu
	for(auto item = menu_items.begin(); item != menu_items.end(); item++){	
		if(item-menu_items.begin() == selectIndex && !menu_items.empty()){
			//print arrow indicating current item
			addnstr("->", 2);
			attrset(A_STANDOUT);
		}
		else
		//otherwise print spaces
		print("  ");
		//prints up to NAME_SPACING characters
		printField(item->name(), NAME_SPACING); print("|");
		printField(item->description(), DESC_SPACING); print("|");
		printField(item->datestring(), DATE_SPACING);
		//This line prints the number used to compare and sort the items of the menu
		printField(std::to_string(static_cast<std::time_t>(item->timeToComplete())), DEBUG_SPACING);
		attroff(A_STANDOUT);
		addch('\n');
	}
	return;
}

void Menu::display(){
	std::string msg = "This is a test message to see if I can reliably print stuff out into the screen. Thanks for being patient.\n";
	//move cursor to top of window and erase all contents
	clearScreen();
	//print a message and the menu
	print(msg);
	printMenu();
}
//change menu selection
void Menu::up(){
	if(selectIndex > 0)
		selectIndex--;
}


//change menu selection
void Menu::down(){
	if(selectIndex+1 < menu_items.size())
		selectIndex++;
}

void expandDateString(std::string& datestring){
	//two iterators start at beginning of datestring
	int spaceCount = 0;
	std::string::iterator first = datestring.begin(); 
	std::string::iterator second = first;
	int firstSpace = datestring.find(' ');

	if(datestring[firstSpace + 1] == ' '){
		datestring.erase(firstSpace, 1);
	}

	while(second != datestring.end()){
		if(*second == '/' || *second == ':' || *second == ' '){
			if(second-first-spaceCount == 1){
				//insert leading zero if its missing
				datestring.insert(first, '0');
				//std::cout << datestring << std::endl;
				second = first = datestring.begin();
			}else{
				//all good, move past the slash
				first = ++second;
			}
			if(*second == ' '){
				spaceCount++;
			}
		}
		second++;
	}
}


