#include "../include/daemon.h"

int inotify_fd;

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
	
	inotify_fd = inotify_init1(IN_NONBLOCK); //initialize inotify subsystem for filesystem events
	if(inotify_fd == -1) {
		std::cerr << "Could not initialize inotify subsystem" << std::endl;
		exit(2);
	}

	std::vector<Profile*> users;
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
		std::string config_path = std::string(temp_array[int(EtcPasswdField::HOME)]) + "/.scheduler/.schedule";

		if (temp_array[int(EtcPasswdField::HOME)] != "" && stat(config_path.c_str(), &xml_info) == 0) {
			std::cerr << "[DEBUG] Successfully loaded file: " << config_path << std::endl;
			users.push_back(new Profile(temp_array));
			std::cerr << "[DEBUG] after profile constructor" << std::endl;
		}
	}

	std::vector<int> child_pids;

	//do forever
	while(1) {

		std::vector<inotify_event*> events = checkConfigurationFiles(inotify_fd);

		while(!events.empty()) {
			inotify_event* event = events.back();

			std::cerr << "FIELDS OF EVENT: " << event->wd << " " << event->mask << " " << event->name << std::endl;
			Profile* changed_profile = findProfileByDescriptor(event->wd, users);
			if (changed_profile && fileExists(changed_profile->getPath())) {
				//reload profile
				changed_profile->clear(); //causes double free :( (was actually caused by Profile::Profile(const Profile&))
				changed_profile->loadConfiguration();
			}
			//remove event and free it from vector
			free(event);
			events.pop_back();
		}

		for(auto& u : users) { //for each profile, check events 
			std::vector<std::string> scripts; // = u.handleEvents();
			
			int status, pid;

			//if handleEvents returned scripts then run them
			if(scripts.size() > 0){
				switch(pid) {
					case 0: 
						execute_scripts(scripts);
						break;
					case -1:
						std::cerr << "Error forking" << std::endl;
						break;
					default:
						child_pids.push_back(pid);
						break;
				}
			}

			//check for dead children
			for(auto iter = child_pids.begin(); iter != child_pids.end(); iter++) {
				if (*iter == waitpid(*iter, &status, WNOHANG)) {
					child_pids.erase(iter);
				}
			}

		}
	}

	return 0;
}
