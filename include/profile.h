#ifndef PROFILE_H
#define PROFILE_H

#define PASSWD_STR_AT(x) (passwd.at(int(x)))
#define PASSWD_INT_AT(x) (stoi(passwd.at(int(x))))

#define EVENT_SIZE (sizeof(struct inotify_event) + NAME_MAX + 1)
#define INOTIF_OPTS (IN_CLOSE)

#include "../include/menuitem.h"
#include "../include/help.h"

#include <sys/inotify.h>
#include <limits.h>
#include <stdio.h>
#include <error.h>
#include <array>
#include <vector>
#include <string>

extern int inotify_fd;
class Profile;

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

std::vector<inotify_event*> checkConfigurationFiles(int inotify_fd);
Profile* findProfileByDescriptor (int wd, std::vector<Profile*> u);

class Profile{

	//Helper methods
	friend std::vector<inotify_event*> checkConfigurationFiles(int);
	friend Profile* checkConfigurationFiles(int, std::vector<Profile*>);

	public:
			Profile() = delete;
			Profile(std::array<std::string, 7> etc_passwd_fields);
			Profile(const Profile&) = delete;
			Profile& operator=(const Profile&) = delete;
			~Profile();

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
			std::string getPath() const { return PASSWD_STR_AT(EtcPasswdField::HOME) + "/.scheduler/.schedule"; }
			std::string getDir() const { return PASSWD_STR_AT(EtcPasswdField::HOME) + "/.scheduler"; }
			int getWD() const { return inotify_wd; }
	private:
			int iter_index = 0, inotify_wd = 0;
			std::array<std::string, 7> passwd;
			rapidxml::xml_document<>* xml_structure;
			char* xml_content;
			std::vector<MenuItem> items;
};


#endif
