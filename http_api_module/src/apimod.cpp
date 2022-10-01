#include "httpapimod/apimod.hpp"
/**
 * In here we will change what the API return
 **/
using json = nlohmann::json;
API::API(std::mutex &obj_mutex, Objects &ai_objects) : mtx(obj_mutex), objs(ai_objects) {
    this->init();
}

void API::start(const char* ip, int port) {
    server.listen(ip, port);
}

void API::init() {
    server.Get("/online", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Yes", "text/plain");
    });

    server.Get("/detected_objects", [this](const httplib::Request& req, httplib::Response& res) {
        //TODO: If variable hasn't updated then keep the old json
        std::unique_lock<std::mutex> lock(mtx);
        //Copy the array to prevent stopping the AI thread
        Objects obj_cpy = objs;
        lock.unlock();
        json out;
        std::vector<json> objs_list;
        for (const DetectedObject &obj : obj_cpy) {
            json obj_json {
                    {"bounding_box", {
                        {"x", obj.bounding_box.x},
                        {"y", obj.bounding_box.y},
                        {"w", obj.bounding_box.width},
                        {"h", obj.bounding_box.height}
                    }},
                    {"id", obj.id},
                    {"name", obj.name},
                    {"distance", std::to_string(obj.distance)},
                    {"location", {
                        {"x", std::to_string(obj.location.x)},
                        {"y", std::to_string(obj.location.y)},
                        {"z", std::to_string(obj.location.z)}
                    }}
            };

            objs_list.push_back(obj_json);
        }
        out["DetectedObjects"] = objs_list;
        res.set_content(out.dump(), "text/plain");
    });
    server.Get("/stop", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Closing Server", "text/plain");
        server.stop();
    });
}
