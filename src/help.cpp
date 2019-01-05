#ifndef HELP_H
#define HELP_H

#include "../include/daemon.h"
class Profile;

std::string extract_string(std::istream& file){
	std::string contents, line;
	//while file has more content
	while(std::getline(file, line)){
		//add it onto contents var
		contents += line;
	}

	//return string of entire file
	return contents;
}

//when this function is called, the execution of the program is on a child process.
//The main process is still looping in main checking for changes in configuration files
//and checking for events to fire off. When an event occurs, the process forks and the child 
//starts to execute this function. The process that initially calls this function then 
//forks itself for each script given, which is executed by the child. This function finally 
//exits when all of the child processes exit. MAY CAUSE TONS OF CHILDREN SINCE A SCRIPT MIGHT NOT EXIT
void execute_scripts(std::vector<std::string> scripts) {
	std::vector<int> child_pids;
	for(auto& s : scripts) {
		int pid = fork();
		switch(pid) {
			case -1:
				//error forking process
				std::cerr << "Error forking in execute_scripts" << std::endl;
				exit(1);
				break;
			case 0:
				//Create command line argument array of '/bin/bash <path/to/script>'
				char* args[3];
				args[0] = "/bin/bash";
				args[1] = s.c_str();
				args[2] = nullptr;
				//replace current (child) process with bash execution context
				execv(args[0], args);
				break;
			default:
				//add child PID to array to check with wait()
				child_pids.push_back(pid);
				break;
		}
	}

	//while there are still children
	while(child_pids.size()) {
		for(auto iter = child_pids.begin(); iter != child_pids.end(); iter++) {
			int status;	
			//wait for process $pid to exit
			int pid = waitpid(*iter, &status, WNOHANG);

			//if child exits
			if (pid == *iter) {
				std::cerr << "Child process " << *iter << " exited" << std::endl;
				//remove from list of children pids
				child_pids.erase(iter);
				break;
			}
		}
	}
}

bool fileExists(std::string str) {
	struct stat stat_instance;
	std::cerr << "STRING in fileExists " << str << std::endl;
	char filepath[512] = {0};
	memcpy(filepath, str.c_str(), 511);
	int result = stat(filepath, &stat_instance);
	if ( result == -1 ) {
		switch(errno) {
			case EACCES:
				std::cerr << "Access denied. Could not read " << str << std::endl;
				break;
			case EFAULT:
				std::cerr << "Error opening file due to c_str()" << str << std::endl;
				break;
			case ENOENT:
			case ENOTDIR:
				std::cerr << "BAD PATH" << str << std::endl;
				break;
		}
		return false;
	}
	return true;	
}

Profile* findProfileByDescriptor (int wd, std::vector<Profile*> u) {
	for(auto& i : u) {
		if (i->getWD() == wd) 
			return i;
	}
	return NULL;
}

inotify_event checkConfigurationFiles(int inotify_fd, std::vector<Profile*>& profiles) {
	char buffer[EVENT_SIZE]; //buffer to hold up to 5 inotify events
	int r = 0;

	do {
		//read events into buffer
		r += read(inotify_fd, buffer, EVENT_SIZE - r);
		if (r > 0) {
			std::cerr << "In reading loop for event" << std::endl;
		}
	} while(r != EVENT_SIZE && r > 0); //while read doesn't read an integer number of events

	struct inotify_event event;	
	event.wd = 0; //this will stay 0 if there are no events

	if(r){
		//move buffer into inotify event
		memmove(&event, buffer, EVENT_SIZE); 
	}

	//return all events found
	return event;
}

#endif
