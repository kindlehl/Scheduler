#include <unistd.h>
#include <unistd.h>
#include <iostream>
#include "../include/logger.h"
#include "../include/menuitem.h"
#include "../include/daemon.h"

int main (int argc, char** argv){
	std::string ident = "Scheduled";
	int cpid = fork();
	if (cpid < 0){
		logError("Process failed to fork");
		exit(255);
	}else if (cpid > 0){
		logDetail("Process forked successfully");
		exit(0);
	}

	std::vector<std::pair<rapidxml::xml_document<>*, char*>> all_configs;
	std::vector<Profile> users;
	std::ifstream etc_passwd("/etc/passwd");
	std::string token, line;

	while(std::getline(etc_passwd, line)) {
		int index = 0;
		std::array<std::string, 7> temp_array;
		std::stringstream passwd_line(line, std::ios_base::in);

		while(std::getline(passwd_line, token, ':')){
			temp_array[index++] = token;
		}

		struct stat xml_info;
		std::string config_path = std::string(temp_array[int(EtcPasswdField::HOME)]) + "/.schedule";
		if(temp_array[int(EtcPasswdField::HOME)] != ""){
			if(stat(config_path.c_str(), &xml_info) == SUCCESS){
				std::cout << "Success: Loading file: " << config_path << std::endl;
				std::ifstream xml_file(config_path);
				
				//causes memory leak
				rapidxml::xml_document<>* config_xml = new rapidxml::xml_document<>;
				char* config_text = config_xml->allocate_string(extract_string(xml_file).c_str());

				all_configs.push_back(std::pair<rapidxml::xml_document<>*, char*>(config_xml, config_text));
				config_xml->parse<0>(all_configs.back().second);

				if(config_xml->first_node()){
					users.push_back(Profile(config_xml, temp_array));
				}
			}else{
				std::cerr << "Error: No such file: " << config_path << std::endl;
				continue;
			}
		}else{
			continue;
		}
		
	}

	return 0;
}
