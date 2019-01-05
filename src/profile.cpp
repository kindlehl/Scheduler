#include "../include/profile.h"

//need to figure out how i want to copy items since they hold pointers to valid resources. Should probs use shared ptrs
//NOT SURE WHAT ABOVE COMMENT MEANS, BUT CP CONSTRUCTOR WAS BLOCKING FOR SOME REASON
//Profile::Profile(const Profile& cp){
	//iter_index = cp.iter_index;
	//passwd = cp.passwd;
//}

void Profile::loadConfiguration() {
	//open file
	std::ifstream xml_file(this->getPath());

	if(!xml_file) {
		std::cerr << "ERROR OPENING CONFIGURATION" << std::endl;
	}
	
	//create new xml data structure
	xml_structure = new rapidxml::xml_document<>;
	if (!xml_structure) {
		std::cerr << "ERROR ALLOCATING XML_DOCUMENT" << std::endl;
	}
	//use xml data structure to allocate string initialized with the contents of the configuration
	//file, so that the string's lifetime is the same as the xml data structure
	xml_content = xml_structure->allocate_string(extract_string(xml_file).c_str());

	//load xml values into data structures from allocated string. String must live as long as the data
	//structure since it uses pointers to the string to store the tags. That's what makes rapidxml so fast
	//is that it doesn't copy anything. It simply stores pointers to parts of the xml along with the length of
	//the xml tags/fields/whatever.
	xml_structure->parse<0>(xml_content);
	auto root = xml_structure->first_node();
	if(!root) {
		std::cerr  << "[DEBUG] You gave the profile constructor a bad xml file:" << getPath()  << std::endl;
		std::cerr  << "[DEBUG] Contents of bad file: " << xml_content << std::endl;
		return;
	}
		
	auto child = root->first_node();
	while(child) {
		//intialize the menu items from each top-level xml node in the xml data structure
		items.push_back(MenuItem(child));
		std::cerr << "[DEBUG] After MenuItem constructor" << std::endl;
		child = child->next_sibling(); //move to next item
	}
}

void Profile::clear() {
	//erase each item in the collection
	items.clear(); //causes double free somehow
	iter_index = 0;
	
	//free memory associated with old xml file
	xml_structure->clear();
	xml_content = nullptr; //nullify content string since its memory is invalidated after freeing xml_structure
}

//expects array of nodes and a single line from the /etc/passwd file
Profile::Profile(std::array<std::string, 7> passwd_array){
	//copy the array of /etc/passwd strings into local structure
	passwd.swap(passwd_array);
	//finish loading configuration
	loadConfiguration();

	//setup inotify watches for modification or deletion
	//when a configuration file is modified, its associated profile
	//needs to be updated. When it is deleted, its associated profile
	//needs to be deleted since the user no longers has a configuration
	std::cerr << "Before waiting function" << std::endl;
	inotify_wd = inotify_add_watch(inotify_fd, this->getPath().c_str(), IN_MODIFY);
	std::cerr << "After waiting function" << std::endl;
}

//disabled since it is invoked when pushing temporary values into vector, which invalidates the copy in the vector
//Profile::~Profile() {
	//remove inotify watch for this profile
	//inotify_rm_watch(inotify_fd, this->inotify_wd);
	//clear(); //deletes xml_structure and the xml_contents which was associated with the xml configuration
//}
