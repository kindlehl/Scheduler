#include "../include/menu.h"
//must use global in main, since multiple calls to getenv() actually modify the value returned for some 
//weird reason. It is actually in the getenv() documentation. MAKES NO SENSE
extern char* HOME;

//used this set to catch special characters when reading input
std::set<chtype> special = {KEY_DC, KEY_BACKSPACE, KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN};

//initalization of static member
bool Menu::run = true;

void clearScreen(){
	move(0,0);
	clrtobot();
}

void Menu::sortMenu(Menu& m){
	//sorts menu so that higher priority items show up first
	std::sort(m.menu_items.begin(), m.menu_items.end(), [](const MenuItem& left, const MenuItem& right){
			return left.timeRemaining() < right.timeRemaining();
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
			delch();
			str.erase(y*maxx+x - promptLen);
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
	char next;	
	//while more events exist, lol. I did this on purpose
	while(!file.eof() && (next = file.peek()) && next  != (next == '\t' || next == '\n' || next == '\v')){
		//add items linee-by-line
		menu_items.push_back(MenuItem(file));
	}
	file.close();	
	getmaxyx(stdscr, height, width);
}

Menu::~Menu(){
}

Menu::operator bool() const{
	return run;
}
void Menu::addItem(){
	curs_set(1);	
	const char* path =(std::string(HOME)+ "/.schedule_add").c_str();
	scr_dump(path);
	chtype c;
	std::smatch matches;
	std::vector<std::pair<std::string, std::regex> > userPrompts = {
		{"Please Enter a Description for This Event: ", std::regex(".*") },
		{"Please Enter a Name for This Event: ", std::regex(".*") },
		{"Please Enter a Due Date (MM/DD/YY HH:MM): ", std::regex("(\\d\\d?)/(\\d\\d?)/(\\d\\d) (\\d\\d?)\\:(\\d\\d)\\W*")}	
	};
	auto prompt = userPrompts.begin();

	std::vector<std::string> responses;
	for( ; prompt != userPrompts.end(); prompt++ ){
		clearScreen();
		addstr(prompt->first.c_str());	
		std::string response;
		do{
			c = getch();
			if(special.find(c) == special.end()){
				response+=static_cast<char>(c);
				addch(c);
			}else{
				handleSpecialKeys(getmaxx(stdscr), getmaxy(stdscr), getcurx(stdscr), getcury(stdscr), prompt->first.length(), c, response);
			}
		} while(c != 10);
		response.erase(response.length()-1);
		//if answer matches the prompt's format, add it to list of responses
		if(regex_match(response, prompt->second))
			responses.push_back(response);
		else //try again
			prompt--;
	}
	//construct item and add to list
	auto eventTime = createTime(responses[2], "(\\d\\d?)/(\\d\\d?)/(\\d\\d) (\\d\\d?)\\:(\\d\\d)\\W*");	
	
	menu_items.push_back(MenuItem(responses[0], responses[1], responses[2], eventTime));
	sortMenu(*this);
	scr_restore(path);
	curs_set(0);	
}

void Menu::update(){
	this->display();
	wmove(stdscr, 0,0);			//move cursor to top, so text gets written over	
	wrefresh(stdscr);
	chtype key = getch();
	if(key == 'r')
		remove();
	else if(key == 'a')
		addItem();
	else if(key == 'j')
		down();
	else if(key == 'k')
		up();
	else if(key == 'q')
		exit(0);
	else if(key == 'v')
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
		printField(menu_items[selectIndex].name(), width);
		addch('\n');
		printField(menu_items[selectIndex].description(), width);
		addch('\n');
		printField(std::to_string(menu_items[selectIndex].timeRemaining()), width); 
		addch('\n');
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
			file << item.name() << "|" << item.description() << "|" << item.dateString << "|" << item.eventTime << "|\n";
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
		printField(item->name(), NAME_SPACING);
		print("|");
		printField(item->description(), DESC_SPACING);
		print("|");
		printField(item->dateString, DATE_SPACING);
		//This line prints the number used to compare and sort the items of the menu
		printField(std::to_string(static_cast<std::time_t>(item->timeRemaining())), DEBUG_SPACING);
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
	sortMenu(*this);
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

void Menu::add(MenuItem new_item){
	static std::string count = "";
	count += "a";
	print(std::string("one a per menu item in mem: ") + count);
	menu_items.push_back(new_item);
	sortMenu(*this);
}



time_t createTime(std::string s, std::string regexp){
	std::smatch matches;
	std::regex_match(s, matches, std::regex(regexp));
	std::tm t;
	std::ofstream log("projectLog", std::ofstream::trunc);
	if(matches.size() == 6){
		t.tm_mon = stoi(matches[1]);
		t.tm_mday = stoi(matches[2]);
		t.tm_year = stoi(matches[3]) + 100;
		t.tm_hour = stoi(matches[4])-1;
		t.tm_min = stoi(matches[5]);
		return std::mktime(&t);
	}else
		return 0;
}
