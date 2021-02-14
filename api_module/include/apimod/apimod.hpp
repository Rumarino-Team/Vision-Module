#ifndef __API_MODULE__
#define __API_MODULE__

#include <iostream>
#include <fcntl.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "model.pb.h"
#include <google/protobuf/message.h>
#include <google/protobuf/util/delimited_message_util.h>


class proto {
public:
    proto(const char * output);
    ~proto();
    void send(DetectedObjects &objects,const char* pipe);
    void send_object(DetectedObject obj, const char* pipe);
    void set_frame(Frame *frame, DetectedObject * obj);
private:
    std::string output;
};
#endif