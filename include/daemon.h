#ifndef DAEMON_H
#define DAEMON_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/stat.h>

std::string extract_string(std::istream& file);

enum error_state {
	FAILURE = -1,
	SUCCESS
};

#endif
