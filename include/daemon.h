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

enum class EtcPasswdField {
	USERNAME,
	PASSWORD,
	UID,
	GID,
	COMMENT,
	HOME,
	SHELL
};

class Profile{
	public:
			Profile() = delete;
			explicit Profile(rapidxml::xml_document<>* node, std::array<std::string, 7> etc_passwd_fields);
			Profile(const Profile&);

			MenuItem& next() { return items[iter_index++]; }
			bool moreItems() { return static_cast<unsigned int>(iter_index) < items.size() - 1 || (iter_index = 0); }

			int getUID() const { return PASSWD_INT_AT(EtcPasswdField::UID); }
			int getGID() const { return PASSWD_INT_AT(EtcPasswdField::GID); }
			std::string getUsername() const { return PASSWD_STR_AT(EtcPasswdField::USERNAME); }
			std::string getPassword() const { return PASSWD_STR_AT(EtcPasswdField::PASSWORD); }
			std::string getComment() const { return PASSWD_STR_AT(EtcPasswdField::COMMENT); }
			std::string getHome() const { return PASSWD_STR_AT(EtcPasswdField::HOME); }
			std::string getShell() const { return PASSWD_STR_AT(EtcPasswdField::SHELL); }
			~Profile() {}
	private:
			int iter_index= 0;
			std::array<std::string, 7> passwd;
			std::vector<MenuItem> items;
};

//need to figure out how i want to copy items since they hold pointers to valid resources. Should probs use shared ptrs
Profile::Profile(const Profile& cp){
	iter_index = cp.iter_index;
	passwd = cp.passwd;
}

//expects array of nodes and a single line from the /etc/passwd file
Profile::Profile(rapidxml::xml_document<>* nodes, std::array<std::string, 7> passwd_array){
	auto root = nodes->first_node();
	do{
		items.push_back(MenuItem(root));
	} while((root = root->next_sibling()));
	passwd.swap(passwd_array);
}

#endif
