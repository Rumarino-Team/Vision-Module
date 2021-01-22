#ifndef __LOGGER_MOD_
#define __LOGGER_MOD_
#include <string>
#include <fstream>
#include <ctime>
#include <experimental/filesystem>

void RUM_log(const std::string& type, const std::string& place, const std::string& msg);

#endif
