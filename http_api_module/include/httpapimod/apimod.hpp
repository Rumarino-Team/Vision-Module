#ifndef __API_MODULE__
#define __API_MODULE__
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <atomic>
#include "aimod/aimod.hpp"

class API {
public:
    API(std::mutex &obj_mutex, sl::Objects &ai_objects);
    void start(const char* ip, int port);
private:
    void init();
    httplib::Server server;
    std::mutex &mtx;
    DetectedObjects &objs;
};

#endif