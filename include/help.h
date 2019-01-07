#ifndef HELP_H
#define HELP_H

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <sys/fcntl.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

extern std::string homedir, conf_dir, conf_path;

class Profile;

bool fileExists(std::string str);
std::string extract_string(std::istream& file);
void execute_scripts(std::vector<std::string> scripts);
void setupFolderStructure();

#endif
