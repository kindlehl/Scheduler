#include "../include/menu.h"

//this file uses this set to catch special characters when reading input
std::set<chtype> special = {KEY_DC, KEY_BACKSPACE};

//initalization of static member
bool Menu::run = true;

void clearScreen(){
	move(0,0);
	clrtobot();
}

void Menu::sortMenu(Menu& m){
	//sorts menu so that higher priority items show up first
	std::stable_sort(m.menu_items.begin(), m.menu_items.end());
}


static void handleSpecialKeys(int maxx, int maxy, int x, int y, int promptLen, chtype c){
	switch(c){
		case(KEY_DC):
			delch();
			break;
		case(KEY_BACKSPACE):
			//move cursor
			x = (x == 0 && (y = y-1) >= 0) ? maxx-1 : x-1;
			move(y, x);
			if(y*maxx+x <= promptLen){
				x = promptLen;
				move(y, x);
			}else{ delch(); }
			refresh();
			break;

	}
}
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
	exit(0);
}

Menu::operator bool() const{
	return run;
}

void Menu::addItem(){
	curs_set(1);	
	char* const path =strcat(getenv("HOME"), "/.schedule_add");
	scr_dump(path);
	clearScreen();
	std::string namePrompt("Please Enter a Name for This Event: ");
	addstr(namePrompt.c_str());	
	chtype c;
	std::string name;
	do{
		c = getch();
		if(special.find(c) == special.end()){
			name+=static_cast<char>(c);
			addch(c);
		}else{
			handleSpecialKeys(getmaxx(stdscr), getmaxy(stdscr), getcurx(stdscr),	getcury(stdscr), namePrompt.length(), c);
		}
	} while(c != KEY_ENTER);



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
		run = false;
	else if(key == 'v')
		view();
}

void Menu::remove(){
	menu_items.erase(menu_items.begin()+selectIndex);
	//calculate where the arrow should appear
	selectIndex = selectIndex >= menu_items.size() ? menu_items.size()-1 : selectIndex;
}

//changes the view to display the detailed description of an item along with other details. Dumps the window into a file, then restores the file after the user does not want to view the event anymore.
void Menu::view(){
	//save screen to file and clear window
	scr_dump(strcat(getenv("HOME"), "/.tempscreen"));
	do{
		clearScreen();
		printField(menu_items[selectIndex].name(), width);
		addch('\n');
		printField(menu_items[selectIndex].description(), width);
		addch('\n');
	} while(getch() != 'q');
	clearScreen();
	scr_restore(strcat(getenv("HOME"), "/.tempscreen"));
}

//this function is called when destructing a menu and when a signal is caught.
void Menu::exit(int sig){
	run = false;
	std::fstream file(strcat(getenv("HOME"), "/.schedule"), std::ios::out);
	//write menu_items back to file, overwriting it
	for(auto item : menu_items){
		file << item.name() << "|" << item.description() << "|" << item.priority() << "\n";
	}
	file.close();
}

//Prints a string up to $spaces number of characters, ending with spaces if there is excess and an elipses "..." when there is not enough spaces
//
//delimiter defaults to ' '
bool Menu::printField(std::string field, int spaces, char delimiter){
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
		if(item-menu_items.begin() == selectIndex){
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
		waddch(stdscr, item->priority()+48);			//this hack way of getting the priority as a one-string causes this to break on priority levels above 9
		print("|\n");
		attroff(A_STANDOUT);
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

void Menu::add(MenuItem new_item){
	static std::string count = "";
	count += "a";
	print(std::string("one a per menu item in mem: ") + count);
	menu_items.push_back(new_item);
	sortMenu(*this);
}


