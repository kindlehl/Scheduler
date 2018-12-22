#include "../include/logger.h"

static bool logger_initialized = false;
void initLog(const char* ident);

void initLog(const char* ident = "Scheduler: "){
	openlog(ident, LOG_INFO, 0);
	syslog(LOG_ERR | LOG_USER, "Scheduler has been started");
}

void checkInit() {
	if (!logger_initialized) {
		initLog();
		logger_initialized = true;
	}
}

void logError(const char* message){
	checkInit();
	syslog(LOG_ERR | LOG_USER, message);
}

void logDetail(const char* message){
	checkInit();
	syslog(LOG_INFO | LOG_USER, message);
}

