#include "loggermod/loggermod.h"

// Default name of file is RUM_log.txt
const char* filename = "RUM_log.txt";

void RUM_log(const std::string& type, const std::string& place, const std::string& msg){
    // fstream automatically creates the file if non-existing
    // log format is as follows
    // [TIME] INFO [PLACE] MESSAGE
    std::fstream logFile("writeable.txt", std::ios_base::app);
    if(logFile.is_open()){
        std::time_t log_time = time(nullptr);
        logFile << std::string(std::asctime(std::localtime(&log_time))) + "\t" + type + "\t[" + place + "]\t" + msg << std::endl;
        logFile.close();
    }
    else{
        std::ofstream ("writeable.txt");
        RUM_log(type, place, msg);
    }
}