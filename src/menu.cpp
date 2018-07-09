#include "../include/menu.h"
//must use global in main, since multiple calls to getenv() actually modify the value returned for some 
//weird reason. It is actually in the getenv() documentation. MAKES NO SENSE
extern char* HOME;

std::time_t createDueTime(std::string& s, std::string regexp);
void expandDateString(std::string& datestring);

/* These keybindings are intended to be modified by a config file eventually. Their definitions are mean to be taken literally 
 */
unsigned int REMOVE_KEY = 'r';
unsigned int ADD_KEY = 'a';
unsigned int DOWN_KEY = 'j';
unsigned int UP_KEY = 'k';
unsigned int QUIT_KEY = 'q';
unsigned int VIEW_KEY = 'v';

//used this set to catch special characters when reading input
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
		t.tm_mon = stoi(matches[1])-1;
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

std::time_t createCompletionTime(std::string& s){
	time_t seconds = 0;
	std::regex hour_matcher("(\\d{1,})hr?"); 
	std::regex minute_matcher("(\\d{1,})m(in)?"); 
	std::smatch hour_matches;	
	std::smatch minute_matches;	
	std::regex_match(s, hour_matches, hour_matcher);
	std::regex_match(s, minute_matches, minute_matcher);
	seconds += std::stoi(hour_matches[0]) * 60 * 60;
	seconds += std::stoi(minute_matches[0]) * 60;
	return seconds;
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
			//move cursor-most compact way to write... uses short-circuit evaluation to modify or keep the
			//current value for y, while adjusting x to what it needs to be
			x = (x == 0 && (y = y-1) >= 0) ? maxx-1 : x-1;
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
	//while more events exist, lol. I did this on purpose
	while(file.good() && file.peek() != std::ifstream::traits_type::eof() && !isspace(file.peek())){
		//add items linee-by-line
		menu_items.push_back(MenuItem(file));
	}
	file.close();	
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
	std::vector<std::regex> match_anything = {std::regex(".*")};
	std::vector<std::regex> match_dates = {std::regex("(\\d\\d?)/(\\d\\d?)/(\\d\\d) (\\d\\d?)\\:(\\d\\d)\\W*")};
	std::vector<std::regex> match_times = {std::regex("[^-]?\\d{1,}[hm]")};

	//vector of pairs of form - {"Prompt Text that shows before the cursor", "Vector of Regular Expressions that user's input must match"}
	std::vector<std::pair<std::string, std::vector<std::regex> > > userPrompts = {
		{"Please Enter a Description for This Event: ", match_anything },
		{"Please Enter a Name for This Event: ", match_anything },
		{"Please Enter a Due Date (MM/DD/YY HH:MM): ", match_dates },	
		{"Please Enter the time this will take (ex. 10m(in) 25h(r)", match_times}
	};

	std::vector<std::string> responses;
	for(auto prompt = userPrompts.begin() ; prompt != userPrompts.end(); prompt++ ){
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
		bool already_decremented = false;
		//if answer matches the prompt's format, add it to list of responses
		for_each(prompt->second.begin(), prompt->second.end(), [response, &already_decremented, &prompt, &responses](std::regex prompt_regex){
			if(regex_match(response, prompt_regex)){
				responses.push_back(response);
			}
			else if (!already_decremented){ //try again
				already_decremented = true;
				prompt--;
			}		
		});
	}
	//construct item and add to list
	auto eventTime = createDueTime(responses[2], "(\\d\\d?)/(\\d\\d?)/(\\d\\d) (\\d\\d?)\\:(\\d\\d)\\W*");	
	auto completionTime = createCompletionTime(responses[3]);	
	menu_items.push_back(MenuItem(responses[0], responses[1], responses[2], eventTime, completionTime));
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
	std::string::iterator first=datestring.begin(); 
	std::string::iterator second = first;
	while(second != datestring.end()){
		if(*second == '/' || *second == ':' || *second == ' '){
			if(second-first-spaceCount <= 1){
				//insert leading zero if its missing
				datestring.insert(first, '0');
				second = first = datestring.begin();
			}else{
				//all good, move past the slash
				first = ++second;
			}
		}
		if(*second == ' '){		
			spaceCount++;
		}
		second++;
	}
	return;
}


