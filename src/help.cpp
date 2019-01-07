#include "../include/help.h"

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
			case -1: {
				//error forking process
				std::cerr << "Error forking in execute_scripts" << std::endl;
				exit(1);
				break;
			}
			case 0: {
				//Create command line argument array of '/bin/bash <path/to/script>'
				char* args[3] = {new char[strlen("/bin/bash") + 1], new char[s.length() + 1], nullptr};
				strcpy(args[0], "/bin/bash");
				strcpy(args[1], s.c_str());
				//replace current (child) process with bash execution context
				execv(args[0], args);
				break;
			}
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

void setupFolderStructure() {
	umask(0000); //clear process umask. Child processes inherit.

	//Initialize global paths
	homedir = std::string(getenv("HOME"));
    conf_dir = homedir + "/.scheduler";
    conf_path = conf_dir + "/.schedule";

	struct stat cdir, xml_file;
	int cdir_status, xml_file_status;

	cdir_status = stat(conf_dir.c_str(), &cdir); //stat user's conf_dir 

	if (cdir_status == -1) {
		// if the error is that the folder simply doesn't exist
		if (errno == ENOENT) {
			// create folder for user
			mkdir(conf_dir.c_str(), 00755); // rwxr-xr-x
		} else { // something horrible has gone wrong, abort
			perror(errno);
			exit(3);
		}
	}

	xml_file_status = stat(conf_path.c_str(), &xml_file); //stat user's conf_dir 

	if (xml_file_status == -1) {
		// if the error is that the file does not exist
		if (errno == ENOENT) {
			// create base xml file for user
			std::ofstream configFile(conf_path.c_str(), std::ios::out);
			configFile << "<items>\n</items>";
			configFile.close();
			
			//change mode since I don't trust user's umask since
			//idiots like me think its a good idea to change it
			chmod(conf_path.c_str(), 00644); // rw-r--r--
		}
	}
}
