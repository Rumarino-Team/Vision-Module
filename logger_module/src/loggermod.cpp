#include "loggermod/loggermod.hpp"

// Default name of file is RUM_log.txt
const char* filename = "RUM_log.txt";

void initPLOG(){
    plog::init(plog::info, filename);
}

void pLog(const std::string& log){
    PLOGI << log;
}