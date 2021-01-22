#include "loggermod/loggermod.h"

// Default name of file is RUM_log.txt
const char* filename = "RUM_log.txt";

/**
 * Takes three strings, one for the type of information that is imperative for the reader to
 * know such as "INFO", "DEBUG" or "TEST". Another string for where the log is being accessed,
 * for example "AI", "ZED", or "HTTP". Lastly, the third string is the log that is to be appended
 * on the log file, the message per se.
 *
 * @param type  Imperative information for the reader
 * @param place Module or place that is accessing the method
 * @param msg   Log that is to be appended to the log file
 */

void RUM_log(const std::string& type, const std::string& place, const std::string& msg){
    // log format is as follows
    // [TIME] \n TYPE [PLACE] MESSAGE
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