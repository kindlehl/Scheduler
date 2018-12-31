#include "../include/profile.h"

//need to figure out how i want to copy items since they hold pointers to valid resources. Should probs use shared ptrs
Profile::Profile(const Profile& cp){
	iter_index = cp.iter_index;
	passwd = cp.passwd;
}

void Profile::loadConfiguration() {
	//get path to configuration
	const char* xml_filename = this->getPath().c_str();

	//open file
	std::ifstream xml_file(xml_filename);
	
	//create new xml data structure
	xml_structure = new rapidxml::xml_document<>;
	//use xml data structure to allocate string initialized with the contents of the configuration
	//file, so that the string's lifetime is the same as the xml data structure
	xml_content = xml_structure->allocate_string(extract_string(xml_file).c_str());

	//load xml values into data structures from allocated string. String must live as long as the data
	//structure since it uses pointers to the string to store the tags. That's what makes rapidxml so fast
	//is that it doesn't copy anything. It simply stores pointers to parts of the xml along with the length of
	//the xml tags/fields/whatever.
	xml_structure->parse<0>(xml_content);
	auto root = xml_structure->first_node();
	do{
		//intialize the menu items from each top-level xml node in the xml data structure
		items.push_back(MenuItem(root));
	} while((root = root->next_sibling())); //leep on chugging
}

void Profile::clear() {
	//erase each item in the collection
	items.erase(items.begin(), items.end());
	//free memory associated with old xml file
	delete xml_structure;
	xml_content = nullptr; //nullify content string since its memory is invalidated after freeing xml_structure
}

//expects array of nodes and a single line from the /etc/passwd file
Profile::Profile(std::array<std::string, 7> passwd_array){
	//copy the array of /etc/passwd strings into local structure
	passwd.swap(passwd_array);
	//finish loading configuration
	loadConfiguration();
}

Profile::~Profile() {
	clear(); //deletes xml_structure and the xml_contents which was associated with the xml configuration
}
