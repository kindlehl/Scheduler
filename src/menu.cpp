#include "../include/menu.h"


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
	char key = getch();
	if(key == 'j')
		down();
	if(key == 'k')
		up();
	if(key == 'q')
		Menu::exit();
}

void Menu::exit(){
	run = false;
}




void Menu::printMenu(){
	std::sort(menu_items.begin(), menu_items.end());
	//prints basic menu
	for(auto item = menu_items.begin(); item != menu_items.end(); item++){	
		if(item-menu_items.begin() == selectIndex)
			print("->");
		else
			print("  ");
		print(item->name());
		print("|");
		print(item->description());
		print("|");
		waddch(stdscr, item->priority()+48);			//this hack way of getting the priority as a one-string causes this to break on priority levels above 9
		print("|\n");
	}
}

void Menu::display(){
	std::string msg = "This is a test message to see if I can reliably print stuff out into the screen. Thanks for being patient.\n";
		 
	print(msg);
	printMenu();
}
//change menu selection
void Menu::up(){
		selectIndex--;
}


//change menu selection
void Menu::down(){
		selectIndex++;
}

void Menu::add(MenuItem new_item){
	menu_items.push_back(new_item);
}


