#ifndef DAEMON_H
#define DAEMON_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/stat.h>

#define PASSWD_STR_AT(x) (passwd.at(int(x)))
#define PASSWD_INT_AT(x) (stoi(passwd.at(int(x))))

static std::string extract_string(std::istream& file){
	std::string contents, line;
	//while file has more content
	while(std::getline(file, line)){
		//add it onto contents var
		contents += line;
	}

	//return string of entire file
	return contents;
}

// enum to map failure to -1 and success to 0 for many system calls
enum error_state {
	FAILURE = -1,
	SUCCESS
};

//ordering of /etc/passwd fields
enum class EtcPasswdField {
	USERNAME,
	PASSWORD,
	UID,
	GID,
	COMMENT,
	HOME,
	SHELL
};

#endif
