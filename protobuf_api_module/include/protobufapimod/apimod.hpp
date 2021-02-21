#ifndef __API_MODULE__
#define __API_MODULE__

#include <iostream>
#include <fcntl.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <google/protobuf/message.h>

#include "zedmod/zedmod.hpp"
#include "aimod/aimod.hpp"
#include "model.pb.h"

class proto {
public:
    proto(std::string output);
    ~proto();
    void send(DetectedObjects objects,const char* pipe);
    void set_frame(Frame frame, DetectedObject obj);
private:
    std::string output;
};
#endif