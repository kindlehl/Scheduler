#ifndef DAEMON_H
#define DAEMON_H

#define EVENT_SIZE (sizeof(struct inotify_event))

#include "../include/logger.h"
#include "../include/menuitem.h"
#include "../include/profile.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/fcntl.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

class Profile;

std::string extract_string(std::istream& file);
void execute_scripts(std::vector<std::string> scripts);
bool fileExists(std::string str);
Profile* findProfileByDescriptor (int wd, std::vector<Profile> u);
inotify_event checkConfigurationFiles(int inotify_fd, std::vector<Profile>& profiles);
// enum to map failure to -1 and success to 0 for many system calls
enum error_state {
	FAILURE = -1,
	SUCCESS
};


#endif
