#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>

static void initLog(const char* ident = "Scheduler: "){
	openlog(ident, LOG_INFO, NULL);
	syslog(LOG_ERR | LOG_USER, "Scheduler has been started");
}

static void logError(const char* message){
	syslog(LOG_ERR | LOG_USER, message);
}

static void logDetail(const char* message){
	syslog(LOG_INFO | LOG_USER, message);
}

#endif
