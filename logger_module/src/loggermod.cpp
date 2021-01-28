#include "loggermod/loggermod.h"

// Default name of file is RUM_log.txt
const char* filename = "RUM_log.txt";

void initPLOG(){
    plog::init(plog::info, filename);
}

void pLog(std::string log){
    PLOGI << log;
}