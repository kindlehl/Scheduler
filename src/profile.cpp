#include "../include/profile.h"

Profile* findProfileByDescriptor (int wd, std::vector<Profile*> u) {
	for(auto& i : u) {
		if (i->getWD() == wd) 
			return i;
	}
	return NULL;
}

//checks primary inotify instance for events
std::vector<inotify_event*> checkConfigurationFiles(int inotify_fd) {
	struct inotify_event* buffer = (struct inotify_event*)calloc(4 * EVENT_SIZE, 1);

	if (buffer == NULL) {
		std::cerr << "Could not allocate memory for event" << std::endl;
		exit(4);
	}

	int bytes_read;
	bytes_read = read(inotify_fd, buffer, 4 * EVENT_SIZE);//sizeof(struct inotify_event));

	std::vector<inotify_event*> events;

	if (bytes_read > 0) {
		for(inotify_event* i = buffer; i < buffer + bytes_read; i += sizeof(inotify_event) + i->len) {
			if (i->wd == 0) {
				continue; //skip bogus file events
			}
			//move event into separate buffer to allow a single event to be freed.
			//If I add pointers in the center of buffer to the vector, only the first 
			//pointer could be freed. Now each pointer shall be freed to prevent confusion
			inotify_event* new_event_buffer = (inotify_event*)calloc(EVENT_SIZE, 1);
			memcpy(new_event_buffer, i, sizeof(inotify_event) + i->len);
			events.push_back(new_event_buffer);
			std::cerr << "[Debug] Found a(nother) event" << std::endl;
		}
	} 

	free(buffer);
	return events;
}

//need to figure out how i want to copy items since they hold pointers to valid resources. Should probs use shared ptrs
//NOT SURE WHAT ABOVE COMMENT MEANS, BUT CP CONSTRUCTOR WAS BLOCKING FOR SOME REASON
//Profile::Profile(const Profile& cp){
	//iter_index = cp.iter_index;
	//passwd = cp.passwd;
//}

void Profile::loadConfiguration() {
	//open file
	inotify_rm_watch(inotify_fd, inotify_wd); //remove watch temporarily
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

	inotify_wd = inotify_add_watch(inotify_fd, this->getDir().c_str(), INOTIF_OPTS); //re-add watch
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
	inotify_wd = inotify_add_watch(inotify_fd, this->getDir().c_str(), INOTIF_OPTS);
}

Profile::~Profile() {
	//remove inotify watch for this profile
	inotify_rm_watch(inotify_fd, this->inotify_wd);
	clear(); //deletes xml_structure and the xml_contents which was associated with the xml configuration
}
