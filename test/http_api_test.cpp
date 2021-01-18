#include <sys/stat.h>
#include <iostream>
#include <chrono>
#include <mutex>
#include <atomic>
#include <gtest/gtest.h>
#include "httpapimod/apimod.hpp"
using json = nlohmann::json;

//Initialize fake mutex
std::mutex obj_mutex;

//Initialize fake detected objects
DetectedObjects dummyObjs;
DetectedObject obj1;
DetectedObject obj2;

API api(obj_mutex, dummyObjs);

void start_server() {
    api.start("0.0.0.0", 8080);
}

std::string detectedobjects2json(const DetectedObjects& objs) {
    json out;
    std::vector<json> objs_list;
    for (const DetectedObject &obj : objs) {
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
    return out.dump();
}

TEST(HTTP_API, Start_Objects) {
    obj1.bounding_box = cv::Rect(1, 2, 3, 4);
    obj2.bounding_box = cv::Rect(5, 6, 7, 8);

    obj1.id = 1;
    obj1.name = "one";
    obj2.id = 2;
    obj2.name = "two";

    obj1.distance = 1.01;
    obj2.distance = 2.02;

    obj1.location.x = 1;
    obj1.location.y = 2;
    obj1.location.z = 3;
    obj2.location.x = 4;
    obj2.location.y = 5;
    obj2.location.z = 6;

    dummyObjs.push_back(obj1);
    dummyObjs.push_back(obj2);
}

std::thread server_thread(start_server);

TEST(HTTP_API, Start_Server) {
    // Expect a general confirmation stating that the server is online
    httplib::Client cli("0.0.0.0:8080");
    auto res = cli.Get("/online");
    EXPECT_TRUE(res->body == "Yes");
}

TEST(HTTP_API, Check_Object) {
    // Expect the detected object to come out as a string
    httplib::Client cli("0.0.0.0:8080");
    auto res = cli.Get("/detected_objects");
    EXPECT_TRUE(res->body == detectedobjects2json(dummyObjs));
}

TEST(HTTP_API, Check_Updated_Object) {
    // Expect the detected object's values to automatically change when our obj is edited
    dummyObjs[1].bounding_box = cv::Rect(0,0,0,0);
    dummyObjs[1].id = 0;
    dummyObjs[1].name = "zero";
    dummyObjs[1].distance = 0.0;
    dummyObjs[1].location.x = 0.0;
    dummyObjs[1].location.y = 0.0;
    dummyObjs[1].location.z = 0.0;

    httplib::Client cli("0.0.0.0:8080");
    auto res = cli.Get("/detected_objects");
    EXPECT_TRUE(res->body == detectedobjects2json(dummyObjs));
}

TEST(HTTP_API, Stop_Server) {
    // Expect a confirmation and the server to stop
    httplib::Client cli("0.0.0.0:8080");
    auto res = cli.Get("/stop");
    EXPECT_TRUE(res->body == "Closing Server");
}