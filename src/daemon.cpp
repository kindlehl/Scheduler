#include <unistd.h>
#include <sys/inotify.h>
#include <iostream>

#include "../include/logger.h"
#include "../include/menuitem.h"
#include "../include/daemon.h"
#include "../include/profile.h"

int main (int argc, char** argv) {
	std::string ident = "Scheduled";
	int cpid = fork();
	if (cpid < 0) {
		logError("Process failed to fork");
		exit(255);
	}else if (cpid > 0) {
		logDetail("Process forked successfully");
		exit(0);
	}

	//child process (daemon) now running
	
	int inotify_fd = inotify_init(); //initialize inotify subsystem for filesystem events

	if(inotify_fd == -1) {
		std::cerr << "Could not initialize inotify subsystem" << std::endl;
		exit(2);
	}

	std::vector<std::pair<rapidxml::xml_document<>* , char* >> all_configs;
	std::vector<Profile> users;
	std::ifstream etc_passwd("/etc/passwd");
	std::string token, line;

	while (std::getline(etc_passwd, line)) {
		int index = 0;
		
		//7 strings, one for each field in an /etc/passwd entry
		std::array<std::string, 7> temp_array;
		std::stringstream passwd_line(line, std::ios_base::in);

		//populate temp_array with the fields in the /etc/passwd entry
		while (std::getline(passwd_line, token, ':')) {
			temp_array[index++] = token;
		}

		struct stat xml_info;
		std::string config_path = std::string(temp_array[int(EtcPasswdField::HOME)]) + "/.schedule";

		if (temp_array[int(EtcPasswdField::HOME)] != "" && stat(config_path.c_str(), &xml_info) == SUCCESS) {
				std::cout << "[DEBUG] Successfully loaded file: " << config_path << std::endl;
				users.push_back(Profile(temp_array));
		}
	}

	////do forever
	//while(1) {
		//for(auto& u : users) { //for each profile, check events 
			//u.handleEvents();
		//}
	//}

	return 0;
}
