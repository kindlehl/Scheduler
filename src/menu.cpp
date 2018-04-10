#include "../include/menu.h"

void Menu::sortMenu(Menu& m){
	//sorts menu so that higher priority items show up first
	std::stable_sort(m.menu_items.begin(), m.menu_items.end());
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
		print("ERROR LOADING FILE: ", A_BOLD);
		print(path, A_BOLD);
		print("\n");
	}

	//while more events exist
	while(!file.eof()){
		//add items line-by-line
		menu_items.push_back(MenuItem(file));
	}
	
	getmaxyx(stdscr, height, width);
}

Menu::~Menu(){
	
}

/* This constructor was replaced by the file-reading constructor
Menu::Menu(){

	//std::cout << "BEGINNING CONSTRUCTION" << std::endl;
	//char* home = getenv("$HOME");		//THIS CAUSES PROGRAM TO HANG FOR SOME REASON
	//print(strcat(home,"/.schedule"));
	loadMenuFromFile("/home/hunter/.schedule");	//CHANGE THIS LATER TO GENERALIZE 
	//loadMenuFromFile(strcat(home,"/.schedule"));	//original call
}
*/

Menu::operator bool() const{
	return run;
}

void Menu::update(){
	this->display();
	wmove(stdscr, 0,0);			//move cursor to top, so text gets written over	
	wrefresh(stdscr);
	chtype key = getch();
	if(key == 'j')
		down();
	else if(key == 'k')
		up();
	else if(key == 'q')
		Menu::exit();
	else if(key == 'v')
		view();
	else if(key == 'r')
		remove();
}

void Menu::remove(){
	std::string home = getenv("HOME");

	//exclude this line when re-writing file
	std::fstream file(home + ".schedule", std::ios::in);
	//read file to local buffer, excluding the deleted line
	for(int i = 0; file; i++){
		std::string line;
/*		if(i != menu_item[selectIndex].ID)
			//you were trying to figure out whether to delete right now or wait until destruction. Keep reading on catching signals, so you can write to the file before exiting no matter what.
		std::getline(file, line);
	*/
	
	}

	//remove from menu
	menu_items.erase(menu_items.begin()+selectIndex);
	//select the position of the cursor - cant delete last element of list otherwise
	selectIndex = selectIndex >= menu_items.size() ? menu_items.size()-1 : selectIndex;
}

//changes the view to display the detailed description of an item along with other details. Dumps the window into a file, then restores the file after the user does not want to view the event anymore.
void Menu::view(){
	//save screen to file and clear window
	scr_dump(".tempscreen");
	do{
		move(0,0);
		clrtobot();

		printField(menu_items[selectIndex].name(), width);
		addch('\n');
		printField(menu_items[selectIndex].description(), width);
		addch('\n');
	} while(getch() != 'q');
	move(0,0);
	clrtobot();
	scr_restore(".tempscreen");
}

void Menu::exit(){
	run = false;
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
			print("->");
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
	move(0,0);
	clrtobot();
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
	menu_items.push_back(new_item);
	sortMenu(*this);
}


