#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>

static void initLog(const char* ident = "Scheduler: "){
	openlog(ident, LOG_INFO, NULL);
}

static void logError(const char* message){
	syslog(LOG_ERR, message);
}

static void logDetail(const char* message){
	syslog(LOG_INFO, message);
}

#endif
