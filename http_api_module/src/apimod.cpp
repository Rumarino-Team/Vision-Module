#include "httpapimod/apimod.hpp"
/**
 * In here we will change what the API return
 **/
using json = nlohmann::json;
API::API(std::mutex &obj_mutex, sl::Objects &ai_objects) : mtx(obj_mutex), objs(ai_objects) {
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
        for (const sl::Objects &obj : obj_cpy) {
            float distance = std::sqrt(position[0]*obj.position[0] + obj.position[1]*obj.position[1] + obj.position[2]*obj.position[2]);
            json obj_json {
                    {"bounding_box", {
                        {"A", obj.bounding_box[0]},
                        {"B", obj.bounding_box_2d[1]},
                        {"C", obj.bounding_box_2d[2]},
                        {"D", obj.bounding_box_2d[3]}
                    }},
                    {"label", obj.label},
                    {"raw_label", obj.raw_label},
                    {"id", obj.id},
                    {"distance", std::to_string(distance)},
                    {"confidence", std::to_string(obj.confidence)},
                    {"location", {
                        {"x", std::to_string(obj.position[0])},
                        {"y", std::to_string(obj.position[1])},
                        {"z", std::to_string(obj.position[2])}
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
