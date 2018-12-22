#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>
#include <iostream>

void checkInit();
void logDetail(const char* message);
void logError(const char* message);

#endif
