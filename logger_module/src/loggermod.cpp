#include "./loggermod.hpp"

// Default name of file is RUM_log.txt
std::string filename = "RUM_log-";

void initPLOG(const std::string& maxSeverity){
    time_t log_time = time(nullptr);
    std::string time = std::string(std::asctime(std::localtime(&log_time)));
    std::replace(time.begin(), time.end(), ' ', '-');
    filename += time + ".txt"; //RUM_LOG-Day-Mon-dd-hh:mm:ss-yyyy.txt
    if(maxSeverity == "info")
        plog::init(plog::info, filename.c_str());
    if(maxSeverity == "debug")
        plog::init(plog::debug, filename.c_str());
}

void pLog(const std::string& log){
    PLOGI << log;
}