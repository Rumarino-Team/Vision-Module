#ifndef __LOGGER_MOD_
#define __LOGGER_MOD_
#include <string>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>

void initPLOG();
void pLog(const std::string& log);

#endif
