#ifndef PROFILE_H
#define PROFILE_H

#define PASSWD_STR_AT(x) (passwd.at(int(x)))
#define PASSWD_INT_AT(x) (stoi(passwd.at(int(x))))

#include "../include/daemon.h"
#include "../include/menuitem.h"

#include <sys/inotify.h>
#include <array>
#include <vector>
#include <string>

extern int inotify_fd;

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

class Profile{
	public:
			Profile() = delete;
			explicit Profile(std::array<std::string, 7> etc_passwd_fields);

			void loadConfiguration();
			void clear();

			MenuItem next() { auto cur_item = items[iter_index]; iter_index = (iter_index + 1) % items.size(); return cur_item; }
			bool moreItems() { return static_cast<unsigned int>(iter_index) < items.size(); }
			MenuItem& findByInotifyDescriptor(int);

			//return information that can be found in /etc/passwd
			int getUID() const { return PASSWD_INT_AT(EtcPasswdField::UID); }
			int getGID() const { return PASSWD_INT_AT(EtcPasswdField::GID); }
			std::string getUsername() const { return PASSWD_STR_AT(EtcPasswdField::USERNAME); }
			std::string getPassword() const { return PASSWD_STR_AT(EtcPasswdField::PASSWORD); }
			std::string getComment() const { return PASSWD_STR_AT(EtcPasswdField::COMMENT); }
			std::string getHome() const { return PASSWD_STR_AT(EtcPasswdField::HOME); }
			std::string getShell() const { return PASSWD_STR_AT(EtcPasswdField::SHELL); }
			std::string getPath() const { return PASSWD_STR_AT(EtcPasswdField::HOME) + "/.schedule"; }
			int getWD() const { return inotify_wd; }


			~Profile() {}
	private:
			int iter_index = 0, inotify_wd = 0;
			std::array<std::string, 7> passwd;
			rapidxml::xml_document<>* xml_structure;
			char* xml_content;
			std::vector<MenuItem> items;
};


#endif
