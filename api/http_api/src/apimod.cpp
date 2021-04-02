#include "httpapimod/apimod.hpp"
using json = nlohmann::json;
API::API(std::mutex &obj_mutex, DetectedObjects &ai_objects) : mtx(obj_mutex), objs(ai_objects) {
    this->init();
}

void API::start(const char* ip, int port) {
    server.listen(ip, port);
    PLOGI << "Initialized API with IP [" << ip << "] on PORT " << port;
}

void API::init() {
    server.Get("/online", [](const httplib::Request& req, httplib::Response& res) {
        PLOGI << "API call of type\t[GET]\tLocation\t/online";
        res.set_content("Yes", "text/plain");
    });

    server.Get("/detected_objects", [this](const httplib::Request& req, httplib::Response& res) {
        //TODO: If variable hasn't updated then keep the old json
        PLOGI << "API call of type\t[GET]\tLocation\t/detected_objects";
        PLOGD << "Copying array of objects to prevent stopping the AI thread.";
        std::unique_lock<std::mutex> lock(mtx);
        //Copy the array to prevent stopping the AI thread
        DetectedObjects obj_cpy = objs;
        lock.unlock();
        PLOGD << "Array copied.";
        json out;
        std::vector<json> objs_list;
        PLOGD<< "Creating JSON format for each object found in objs_list.";
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
        PLOGD << "Sending through API call the JSON formatted objects.";
        out["DetectedObjects"] = objs_list;
        res.set_content(out.dump(), "text/plain");
    });
    server.Get("/stop", [&](const httplib::Request& req, httplib::Response& res) {
        PLOGI << "API call of type\t[GET]\tLocation\t/stop\nClosing API...";
        res.set_content("Closing Server", "text/plain");
        server.stop();
        PLOGI <<"API closed.";
    });
}
